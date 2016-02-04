/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <alloca.h>
#include <assert.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <expat.h>

#include "log.h"
#include "udt.h"
#include "rdt.h"

#include "base64.h"

#define RDT_DATABUF_MAXLEN 8096
#define RDT_HEADER_MAXLEN 135
#define RDT_PACKET_RESEND_TIMES 15
#define RDT_PACKET_RESEND_INTERVAL 300

#define XML_ENCODING "UTF-8"
#define XML_ATTR_ID "id"
#define XML_ELEM_SEGMENT "rdt-segment"
#define XML_ELEM_HEADER "header"
#define XML_ELEM_DATA "data"
#define XML_ELEM_SYN "syn"
#define XML_ELEM_ACK "ack"
#define XML_ELEM_FIN "fin"
#define XML_ELEM_RST "rst"
#define XML_ELEM_SEQ "seq"
#define XML_ELEM_WND "wnd"

enum _xml_mystate {
    IN_DOCUMENT = 0,
    IN_SEGMENT,
    IN_HEADER,
    IN_DATA,

    IN_HEADER_WND,
    IN_HEADER_SEQ,
    IN_HEADER_ACK,
    IN_HEADER_SYN,
    IN_HEADER_RST,
    IN_HEADER_FIN
};

typedef struct _rdt_pack_st rdt_pack_t;
typedef struct _rdt_wnd_st rdt_wnd_t;
typedef struct _xmldata_st xmldata_t;

struct _rdt_pack_st {
    uint8_t flags;
    uint8_t window;
    uint16_t seqnum;
    uint16_t ack;
    char* data;
    size_t datalen;

    size_t sent_times;
#ifdef _WIN32
    clock_t sent_at;
#else
    struct timespec sent_at;
#endif
};

struct _rdt_wnd_st {
    uint8_t flags;
    rdt_pack_t* packet;
    rdt_wnd_t* next;
};

struct _rdt_st {
    XML_Parser xmlParser;
    uint8_t window_size;
    uint8_t cwnd;
    uint16_t nextseqnum;

    rdt_wnd_t* window_free;
    rdt_wnd_t* window;
    rdt_wnd_t* acks;

    bool eof_rcvd;
    bool eof_send;
    bool eof_sent;

    char* data;
    size_t datalen;
    char* pdata;
};

struct _xmldata_st {
    rdt_t* rdt;
    rdt_pack_t* pkt;
    enum _xml_mystate state;
};

static size_t
_rdt_packet_write(
        rdt_t*,
        rdt_pack_t*,
        char* buf,
        size_t len);

static rdt_pack_t*
_rdt_packet_parse(
        rdt_t*,
        const char* buf,
        size_t len);

static void
_rdt_packet_free(
        rdt_pack_t*);

static bool
_rdt_server_process(
        rdt_t*,
        rdt_pack_t*);

static void
_xml_elem_onstart(
        void* userdata,
        const char* name,
        const char** attrs);

static void
_xml_elem_onend(
        void* userdata,
        const char* name);

static void
_xml_elem_ondata(
        void* userdata,
        const char* str,
        int len);

#define WRITE(str, buf, offset, len) do { \
    size_t tmplen = strlen(str); \
    if (tmplen + offset > len) return 0; \
    memcpy(buf + offset, str, tmplen); \
    offset += tmplen; \
} while(0)

#define WRITE_INT(num, buf, offset, len) do { \
    char tmpbuf[32]; \
    int tmplen = sprintf(tmpbuf, "%ld", (long)num); \
    if (tmplen == -1) return 0; \
    if (tmplen + offset > len) return 0; \
    memcpy(buf + offset, tmpbuf, tmplen); \
    offset += tmplen; \
} while(0)

#define CREATE_PACKET(pkt) do { \
    (pkt) = (rdt_pack_t*)malloc(sizeof(rdt_pack_t)); \
    if (!(pkt)) { \
        ipk_perror(ENOMEM); \
        exit(EXIT_FAILURE); \
    } \
    memset((pkt), '\0', sizeof(rdt_pack_t)); \
} while(0)

#define CREATE_PACKET_WND(wnd, pkt) do { \
    (wnd) = (rdt_wnd_t*)malloc(sizeof(rdt_wnd_t)); \
    (pkt) = (rdt_pack_t*)malloc(sizeof(rdt_pack_t)); \
    if (!(wnd) || !(pkt)) { \
        ipk_perror(ENOMEM); \
        exit(EXIT_FAILURE); \
    } \
    memset((wnd), '\0', sizeof(rdt_wnd_t)); \
    memset((pkt), '\0', sizeof(rdt_pack_t)); \
    (wnd)->packet = (pkt); \
    (wnd)->next = NULL; \
    (wnd)->flags = RDT_WND_FLAG_USABLE; \
} while(0)

#define MAKE_TEMPSTRING(tmp, str, len) do { \
    (tmp) = (char*)alloca((len) + 1); \
    strncpy((tmp), (str), (len)); \
    (tmp)[len] = '\0'; \
} while(0)

#define BASE64_NEEDLENGTH(inlen) (3 * ((inlen) / 4) + 3)

void _xml_elem_onstart(void* userdata, const char* name, const char** attrs)
{
    (void)attrs;

    xmldata_t* data = (xmldata_t*)userdata;
    assert(data != NULL);
    assert(data->pkt != NULL);

    if (!strcmp(name, XML_ELEM_SEGMENT)) {
        data->state = IN_SEGMENT;
    }
    else if (!strcmp(name, XML_ELEM_HEADER)) {
        data->state = IN_HEADER;
    }
    else if (!strcmp(name, XML_ELEM_DATA)) {
        data->state = IN_DATA;
    }
    else if (!strcmp(name, XML_ELEM_ACK)) {
        data->state = IN_HEADER_ACK;
        data->pkt->flags |= RDT_FLAG_ACK;
    }
    else if (!strcmp(name, XML_ELEM_FIN)) {
        data->state = IN_HEADER_FIN;
        data->pkt->flags |= RDT_FLAG_FIN;
    }
    else if (!strcmp(name, XML_ELEM_RST)) {
        data->state = IN_HEADER_RST;
        data->pkt->flags |= RDT_FLAG_RST;
    }
    else if (!strcmp(name, XML_ELEM_SEQ)) {
        data->state = IN_HEADER_SEQ;
        data->pkt->flags |= RDT_FLAG_SEQ;
    }
    else if (!strcmp(name, XML_ELEM_SYN)) {
        data->state = IN_HEADER_SYN;
        data->pkt->flags |= RDT_FLAG_SYN;
    }
    else if (!strcmp(name, XML_ELEM_WND)) {
        data->state = IN_HEADER_WND;
        data->pkt->flags |= RDT_FLAG_WND;
    }
    else {
        _ipk_assert();
    }
}

void _xml_elem_onend(void* userdata, const char* name)
{
    xmldata_t* data = (xmldata_t*)userdata;
    assert(data != NULL);
    assert(data->pkt != NULL);

    if (!strcmp(name, XML_ELEM_SEGMENT)) {
        data->state = IN_DOCUMENT;
    }
    else if (!strcmp(name, XML_ELEM_HEADER)) {
        data->state = IN_SEGMENT;
    }
    else if (!strcmp(name, XML_ELEM_DATA)) {
        data->state = IN_SEGMENT;
    }
    else if (!strcmp(name, XML_ELEM_ACK)) {
        data->state = IN_HEADER;
    }
    else if (!strcmp(name, XML_ELEM_FIN)) {
        data->state = IN_HEADER;
    }
    else if (!strcmp(name, XML_ELEM_RST)) {
        data->state = IN_HEADER;
    }
    else if (!strcmp(name, XML_ELEM_SEQ)) {
        data->state = IN_HEADER;
    }
    else if (!strcmp(name, XML_ELEM_SYN)) {
        data->state = IN_HEADER;
    }
    else if (!strcmp(name, XML_ELEM_WND)) {
        data->state = IN_HEADER;
    }
    else {
        _ipk_assert();
    }
}

void _xml_elem_ondata(void* userdata, const char* str, int len)
{
    xmldata_t* data = (xmldata_t*)userdata;
    assert(data != NULL);
    assert(data->pkt != NULL);

    char* tmp = NULL;

    switch (data->state) {
        default:
            _ipk_assert();
            break;

        case IN_HEADER_ACK: {
            MAKE_TEMPSTRING(tmp, str, len);
            sscanf(tmp, "%hu", &data->pkt->ack);
        }   break;

        case IN_HEADER_SEQ: {
            MAKE_TEMPSTRING(tmp, str, len);
            sscanf(tmp, "%hu", &data->pkt->seqnum);
        }   break;

        case IN_HEADER_WND: {
            MAKE_TEMPSTRING(tmp, str, len);
            unsigned short utmp;
            sscanf(tmp, "%hu", &utmp);
            if (utmp < USHRT_MAX)
                data->pkt->window = (uint8_t)utmp;
            else
                _ipk_assert_val(utmp);
        }   break;

        case IN_DATA: {
            data->pkt->data = (char*)malloc(len);
            if (!data->pkt->data) {
                ipk_perror(ENOMEM);
                exit(EXIT_FAILURE);
            }

            data->pkt->datalen = len;
            memcpy(data->pkt->data, str, len);
        }   break;
    }
}

size_t _rdt_packet_write(rdt_t* rdt, rdt_pack_t* pkt, char* buf, size_t len)
{
    (void)rdt;
    if (buf == NULL) {
        return 0;
    }
    if (pkt == NULL) {
        return 0;
    }
    if (len == 0) {
        return 0;
    }

    size_t offset = 0;
    
    WRITE("<" XML_ELEM_SEGMENT " " XML_ATTR_ID "=\"xsevci44\"><" XML_ELEM_HEADER ">",
          buf, offset, len);

    if (pkt->flags & RDT_FLAG_SEQ) {
        WRITE("<" XML_ELEM_SEQ ">", buf, offset, len);
        WRITE_INT(pkt->seqnum, buf, offset, len);
        WRITE("</" XML_ELEM_SEQ ">", buf, offset, len);
    }
    if (pkt->flags & RDT_FLAG_ACK) {
        WRITE("<" XML_ELEM_ACK ">", buf, offset, len);
        WRITE_INT(pkt->ack, buf, offset, len);
        WRITE("</" XML_ELEM_ACK ">", buf, offset, len);
    }
    if (pkt->flags & RDT_FLAG_WND) {
        WRITE("<" XML_ELEM_WND ">", buf, offset, len);
        WRITE_INT(pkt->window, buf, offset, len);
        WRITE("</" XML_ELEM_WND ">", buf, offset, len);
    }
    if (pkt->flags & RDT_FLAG_SYN) {
        WRITE("<" XML_ELEM_SYN "/>", buf, offset, len);
    }
    if (pkt->flags & RDT_FLAG_RST) {
        WRITE("<" XML_ELEM_RST "/>", buf, offset, len);
    }
    if (pkt->flags & RDT_FLAG_FIN) {
        WRITE("<" XML_ELEM_FIN "/>", buf, offset, len);
    }
    WRITE("</" XML_ELEM_HEADER ">", buf, offset, len);

    if (pkt->data && pkt->datalen > 0) {
        WRITE("<" XML_ELEM_DATA ">", buf, offset, len);
        memcpy(buf + offset, pkt->data, pkt->datalen);
        offset += pkt->datalen;
        WRITE("</" XML_ELEM_DATA ">", buf, offset, len);
    }
    
    WRITE("</" XML_ELEM_SEGMENT ">", buf, offset, len);
    return offset;
}

rdt_pack_t* _rdt_packet_parse(rdt_t* rdt, const char* buf, size_t len)
{
    rdt_pack_t* pkt;
    CREATE_PACKET(pkt);

    xmldata_t userdata;
    userdata.rdt = rdt;
    userdata.pkt = pkt;
    userdata.state = IN_DOCUMENT;

    if (!XML_ParserReset(rdt->xmlParser, XML_ENCODING)) {
        _ipk_error_log("%s", XML_ErrorString(XML_GetErrorCode(rdt->xmlParser)));
        _rdt_packet_free(pkt);
        return NULL;
    }

    XML_SetElementHandler(rdt->xmlParser, &_xml_elem_onstart, &_xml_elem_onend);
    XML_SetCharacterDataHandler(rdt->xmlParser, &_xml_elem_ondata);
    XML_SetUserData(rdt->xmlParser, &userdata);

    if (XML_STATUS_OK != XML_Parse(rdt->xmlParser, buf, len, true)) {
        _ipk_error_log("%s", XML_ErrorString(XML_GetErrorCode(rdt->xmlParser)));
        _rdt_packet_free(pkt);
        return NULL;
    }

    return pkt;
}

void _rdt_packet_free(rdt_pack_t* pkt)
{
    if (pkt) {
        free(pkt->data);
        free(pkt);
    }
}

rdt_t* rdt_new()
{
    rdt_t* rdt = (rdt_t*)malloc(sizeof(rdt_t));
    if (rdt) {
        memset(rdt, '\0', sizeof(rdt_t));

        rdt->datalen = 0;
        rdt->data = (char*)malloc(RDT_DATABUF_MAXLEN);
        if (!rdt->data) {
            rdt_dispose(rdt);
            return NULL;
        }
        rdt->pdata = rdt->data;

        rdt->xmlParser = XML_ParserCreate(XML_ENCODING);
        if (!rdt->xmlParser) {
            rdt_dispose(rdt);
            return NULL;
        }
    }

    return rdt;
}

void rdt_dispose(rdt_t* rdt)
{
    if (rdt) {
        for (rdt_wnd_t *p = rdt->window, *q = NULL; p != NULL; p = q) {
            q = p->next;
            _rdt_packet_free(p->packet);
            free(p);
        }
        for (rdt_wnd_t *p = rdt->window_free, *q = NULL; p != NULL; p = q) {
            q = p->next;
            _rdt_packet_free(p->packet);
            free(p);
        }

        XML_ParserFree(rdt->xmlParser);
        free(rdt->data);
        free(rdt);
    }
}

bool rdt_client_start(rdt_t* rdt)
{
    if (!rdt) {
        return false;
    }

    srand((unsigned int)(uintptr_t)rdt);
    rdt->nextseqnum = rand() % 10 * 100;
    rdt->window_size = 1;
    rdt->cwnd = 1;

    rdt_pack_t* pkt;
    rdt_wnd_t* wnd;
    CREATE_PACKET_WND(wnd, pkt);

    pkt->flags = RDT_FLAG_SYN | RDT_FLAG_SEQ;
    pkt->seqnum = rdt->nextseqnum++;
    wnd->flags = RDT_WND_FLAG_USABLE;
    wnd->packet = pkt;

    rdt->window = wnd;
    return true;
}

bool rdt_server_start(rdt_t* rdt, uint8_t wndSize)
{
    if (!rdt) {
        return false;
    }

    rdt->window_size = wndSize;
    rdt->cwnd = wndSize;
    rdt_wnd_t* lastWnd = NULL;

    // nechame si +1 okno
    for (size_t i = 0; i <= rdt->window_size; ++i) {
        rdt_wnd_t* wnd = (rdt_wnd_t*)malloc(sizeof(rdt_wnd_t));
        if (!wnd) {
            goto return_false;
        }

        memset(wnd, '\0', sizeof(rdt_wnd_t));
        wnd->flags = RDT_WND_FLAG_USABLE;
        wnd->packet = NULL;
        wnd->next = lastWnd;

        lastWnd = wnd;
    }

    rdt->window_free = lastWnd;
    return true;

return_false:
    rdt->window_size = 0;
    for (rdt_wnd_t *p = lastWnd, *q = NULL; p != NULL; p = q) {
        q = p->next;
        free(p);
    }
    return false;
}

void rdt_client_step(rdt_t* rdt, const char* inbuf, size_t inbuflen,
                     char* outbuf, size_t* outbuflen)
{
    if (!rdt) {
        return;
    }

    if (inbuf && inbuflen > 0) {
        rdt_pack_t* pkt = _rdt_packet_parse(rdt, inbuf, inbuflen);
        if (!pkt) {
            _ipk_error_log("parsing packet failed");
            goto in_end;
        }

        if (pkt->flags & RDT_FLAG_ACK) {
            // nastaveni ack prislusnemu paketu
            for (rdt_wnd_t* wnd = rdt->window; wnd != NULL; wnd = wnd->next) {
                if (wnd->packet->seqnum + 1 == pkt->ack) {
                    _ipk_verbose_log("acked #%hu", pkt->ack);
                    if (rdt->cwnd < 128) {
                        rdt->cwnd *= 2;
                        _ipk_verbose_log("setting cwnd to %hu", rdt->cwnd);
                    }
                    else if (rdt->cwnd != UINT8_MAX) {
                        rdt->cwnd = UINT8_MAX;
                        _ipk_verbose_log("setting cwnd to %hu", rdt->cwnd);
                    }

                    wnd->flags = RDT_WND_FLAG_ACKED;
                    break;
                }
            }

            // odstraneni bloku potvrzenych paketu
            for (rdt_wnd_t *p = rdt->window, *q = NULL; p; p = q) {
                q = p->next;

                if (p->flags != RDT_WND_FLAG_ACKED) {
                    break;
                }

                if (p->packet->flags & RDT_FLAG_FIN) {
                    _ipk_verbose_log("received eof");
                    rdt->eof_rcvd = true;
                }
                else if (p->packet->flags & RDT_FLAG_RST) {
                    _ipk_verbose_log("received rst");
                    rdt->eof_rcvd = true;
                }
                else if (p->packet->flags & RDT_FLAG_SYN
                    && pkt->flags & RDT_FLAG_WND)
                {
                    rdt->window_size = pkt->window;
                    _ipk_verbose_log("window_size #%hu", pkt->window);

                    for (size_t i = 1; i < rdt->window_size; ++i) {
                        rdt_wnd_t* newWnd = (rdt_wnd_t*)malloc(sizeof(rdt_wnd_t));
                        if (!newWnd) {
                            ipk_perror(errno);
                            break;
                        }
                        memset(newWnd, '\0', sizeof(rdt_wnd_t));
                        newWnd->flags = RDT_WND_FLAG_USABLE;
                        newWnd->next = rdt->window_free;
                        rdt->window_free = newWnd;
                    }
                }

                _ipk_verbose_log("send_base set to %hu", p->packet->seqnum + 1);

                // uvolneni okna
                _rdt_packet_free(p->packet);
                p->packet = NULL;

                rdt->window = q;
                p->next = rdt->window_free;
                rdt->window_free = p;

            }
        }

        _rdt_packet_free(pkt);
    }
in_end:
    
    if (outbuf && *outbuflen > 0) {
        if (rdt->data && rdt->datalen > 0 && rdt->window_free) {
            // pripojime data
            size_t numfree = min(rdt->cwnd, rdt->window_size);
            rdt_wnd_t* lastWnd = NULL;

            for (rdt_wnd_t *p = rdt->window; p; p = p->next) {
                lastWnd = p;
            }

            for (rdt_wnd_t *p = rdt->window_free, *q = NULL;
                 p && rdt->datalen > 0 && numfree > 0;
                 p = q, --numfree)
            {
                q = p->next;

                rdt_pack_t* pkt;
                CREATE_PACKET(pkt);
                p->flags = RDT_WND_FLAG_USABLE;
                p->packet = pkt;
                pkt->flags = RDT_FLAG_SEQ;
                pkt->seqnum = rdt->nextseqnum++;

                size_t max_datalen = min(BASE64_NEEDLENGTH(UDT_PACKET_MAXLEN - RDT_HEADER_MAXLEN), rdt->datalen);
                size_t outlen = BASE64_LENGTH(max_datalen);

                char* buf = (char*)malloc(outlen);
                if (!buf) {
                    ipk_perror(ENOMEM);
                    exit(EXIT_FAILURE);
                }

                bool overflow = &rdt->pdata[max_datalen] > &rdt->data[RDT_DATABUF_MAXLEN];
                if (!overflow) {
                    base64_encode(rdt->pdata, max_datalen, buf, outlen);
                    rdt->pdata += max_datalen;
                }
                else {
                    size_t len_to_end = &rdt->data[RDT_DATABUF_MAXLEN] - rdt->pdata;
                    char* tmpbuf = (char*)alloca(max_datalen);

                    memcpy(tmpbuf, rdt->pdata, len_to_end);
                    memcpy(tmpbuf + len_to_end, rdt->data, max_datalen - len_to_end);

                    base64_encode(tmpbuf, max_datalen, buf, outlen);
                    rdt->pdata = &rdt->data[max_datalen - len_to_end];
                }

                pkt->data = buf;
                pkt->datalen = outlen;

                rdt->datalen -= max_datalen;

                _ipk_verbose_log("created data packet #%hu, %lu bytes left", pkt->seqnum, (unsigned long)rdt->datalen);

                if (lastWnd != NULL) {
                    lastWnd->next = p;
                }
                else {
                    rdt->window = p;
                }

                rdt->window_free = q;
                p->next = NULL;

                lastWnd = p;
            }
        }
        else if (rdt->eof_send && !rdt->eof_sent && rdt->window_free) {
            // dalsi data uz posilat nebudeme, posleme EOF
            rdt_wnd_t* lastWnd = NULL;
            for (rdt_wnd_t* p = rdt->window; p; p = p->next) {
                lastWnd = p;
            }

            rdt_wnd_t* wnd = rdt->window_free;
            rdt_pack_t* pkt;
            CREATE_PACKET(pkt);

            rdt->window_free = wnd->next;
            wnd->packet = pkt;
            wnd->next = NULL;
            wnd->flags = RDT_WND_FLAG_USABLE;
            pkt->flags = RDT_FLAG_FIN | RDT_FLAG_SEQ;
            pkt->seqnum = rdt->nextseqnum++;

            if (lastWnd != NULL) {
                lastWnd->next = wnd;
            }
            else {
                rdt->window = wnd;
            }

            lastWnd = wnd;
            rdt->eof_sent = true;
        }

        bool found = false;

        //_ipk_verbose_log("FILLED WNDs:");
        //for (rdt_wnd_t* p = rdt->window; p; p = p->next) {
        //    _ipk_verbose_log("wnd %p, packet #%hu", (void*)p, p->packet ? p->packet->seqnum : 0);
        //}

        //_ipk_verbose_log("FREE WNDs:");
        //for (rdt_wnd_t* p = rdt->window_free; p; p = p->next) {
        //    _ipk_verbose_log("wnd %p, packet #%hu", (void*)p, p->packet ? p->packet->seqnum : 0);
        //}

        for (rdt_wnd_t* wnd = rdt->window; wnd != NULL; wnd = wnd->next) {
            assert(wnd->packet != NULL);

            if (wnd->flags == RDT_WND_FLAG_USABLE) {
                size_t nbuf = _rdt_packet_write(rdt, wnd->packet, outbuf, *outbuflen);
                assert(nbuf > 0);

                *outbuflen = nbuf;
                wnd->flags = RDT_WND_FLAG_BUFFERED;
                found = true;

#ifdef _WIN32
                wnd->packet->sent_at = clock();
#else
                if (-1 == clock_gettime(CLOCK_MONOTONIC, &wnd->packet->sent_at)) {
                    ipk_perror(errno);
                }
#endif

                _ipk_verbose_log("sending #%hu", wnd->packet->seqnum);
                break;
            }
            else if (wnd->flags == RDT_WND_FLAG_BUFFERED) {
#ifdef _WIN32
                clock_t now = clock();
                clock_t diff = (now - wnd->packet->sent_at) / (CLOCKS_PER_SEC / 1000);
#else
                struct timespec now;
                if (-1 == clock_gettime(CLOCK_MONOTONIC, &now)) {
                    ipk_perror(errno);
                    continue;
                }

                //time_t diff = (now.tv_sec * 1000 + now.tv_nsec / 1000000) -
                //    (wnd->packet->sent_at.tv_sec * 1000 + wnd->packet->sent_at.tv_nsec / 1000000);
                time_t diff = (now.tv_sec - wnd->packet->sent_at.tv_sec) * 1000
                    + (now.tv_nsec - wnd->packet->sent_at.tv_nsec) / 1000000;
#endif
                
                //_ipk_verbose_log("diff: %ld", diff);

                if (diff > RDT_PACKET_RESEND_INTERVAL) {
                    if (wnd->packet->sent_times >= RDT_PACKET_RESEND_TIMES) {
                        // paket nemuze byt dorucen.. konec
                        _ipk_error_log("packet #%hu cannot be delivered after %d tries",
                            wnd->packet->seqnum, RDT_PACKET_RESEND_TIMES);
                        exit(EXIT_FAILURE);
                    }
                    
                    // nastavime mu priznak USABLE, ale sent_times mu zustane
                    // pri dalsim cyklu se dostane zase sem a bude mit priznak BUFFERED
                    // sent_at se nastavi az v dalsim cyklu
                    wnd->packet->sent_times++;
                    _ipk_verbose_log("resending packet #%hu, %zu tries", wnd->packet->seqnum, wnd->packet->sent_times);

                    wnd->flags = RDT_WND_FLAG_USABLE;
                    rdt->cwnd = 1; // slow-start
                }
            }
        }

        if (!found) {
            *outbuflen = 0;
        }
    }
}

void rdt_server_step(rdt_t* rdt, const char* inbuf, size_t inbuflen,
                     char* outbuf, size_t* outbuflen)
{
    if (!rdt) {
        return;
    }

    if (inbuf && inbuflen > 0) {
        rdt_pack_t* pkt = _rdt_packet_parse(rdt, inbuf, inbuflen);
        if (!pkt) {
            _ipk_error_log("parsing packet failed");
            goto in_end;
        }

        if (!(pkt->flags & RDT_FLAG_SEQ)) {
            // ignorujeme pakety bez seq. cisla
            _ipk_verbose_log("received packet w/o seqnum, drop it");
            _rdt_packet_free(pkt);
            goto in_end;
        }

        if (pkt->flags & RDT_FLAG_RST) {
            _ipk_verbose_log("received rst");
            _rdt_packet_free(pkt);

            rdt->eof_rcvd = true;
            goto in_end;
        }

        if (pkt->flags & RDT_FLAG_SYN) {
            rdt->nextseqnum = pkt->seqnum;
            rdt->eof_rcvd = false;
            rdt->eof_send = false;
            rdt->eof_sent = false;
            _ipk_verbose_log("received syn, seqnum %hu", rdt->nextseqnum);
        }

        // ackneme prijaty paket
        bool underflow = rdt->nextseqnum < rdt->window_size;
        bool overflow = rdt->nextseqnum > UINT16_MAX - rdt->window_size;
        bool in_range = false;

        if (!underflow && !overflow) {
            // <rcv_base-N; rcv_base+N)
            in_range = pkt->seqnum >= rdt->nextseqnum - rdt->window_size &&
                pkt->seqnum < rdt->nextseqnum + rdt->window_size;
        }
        else if (overflow) {
            // <base-N; MAX> u <0; N-(MAX-base)-1)
            // => not in <N-(MAX-base)-1; base-N)
            in_range = !(pkt->seqnum >= rdt->window_size - (UINT16_MAX - rdt->nextseqnum) - 1
                && pkt->seqnum <= rdt->nextseqnum - rdt->window_size);
        }
        else if (underflow) {
            // <0; base+N) u (MAX-N+base; MAX>
            // => not in <base+N; MAX-N+base>
            in_range = !(pkt->seqnum >= rdt->nextseqnum + rdt->window_size
                && pkt->seqnum <= UINT16_MAX - rdt->window_size + rdt->nextseqnum);
        }
        else {
            _ipk_assert();
            in_range = false;
        }

        //_ipk_debug_log("SEQ#=%hu,underflow=%x,overflow=%x", pkt->seqnum, underflow, overflow);

        if (in_range) {
            rdt_wnd_t* wnd;
            rdt_pack_t* ackpkt;
            CREATE_PACKET_WND(wnd, ackpkt);
            wnd->next = rdt->acks;
            rdt->acks = wnd;
            ackpkt->flags = RDT_FLAG_ACK | RDT_FLAG_WND;
            ackpkt->ack = pkt->seqnum + 1;
            ackpkt->window = rdt->cwnd;

            _ipk_verbose_log("send ack #%hu", ackpkt->ack);
            
            if (pkt->seqnum < rdt->nextseqnum) {
                // uz sme zpracovali, ack odeslali.. ignorujeme
                _ipk_verbose_log("ignoring packet #%hu", pkt->seqnum);
                _rdt_packet_free(pkt);
                goto in_end;
            }
        }
        else {
            // ignorujeme
            _ipk_verbose_log("underflow: %x overflow: %x", underflow, overflow);
            _ipk_verbose_log("ignoring packet #%hu, not in window, expected #%hu", pkt->seqnum, rdt->nextseqnum);

            _ipk_verbose_log("FILLED WNDs:");
            for (rdt_wnd_t* p = rdt->window; p; p = p->next) {
                _ipk_verbose_log("wnd %p, packet #%hu", (void*)p, p->packet ? p->packet->seqnum : 0);
            }

            _ipk_verbose_log("FREE WNDs:");
            for (rdt_wnd_t* p = rdt->window_free; p; p = p->next) {
                _ipk_verbose_log("wnd %p, packet #%hu", (void*)p, p->packet ? p->packet->seqnum : 0);
            }

            _rdt_packet_free(pkt);
            goto in_end;
        }

        // pridame do okna
        size_t numFullWnd = 0;
        rdt_wnd_t* prevWnd = NULL;
        rdt_wnd_t* actWnd = rdt->window_free;

        for (rdt_wnd_t* p = rdt->window; p && p->packet; p = p->next) {
            // najdeme okno, za ktere prijde nas packet
            if (pkt->seqnum > p->packet->seqnum) {
                ++numFullWnd;
                prevWnd = p;
            }
            else if (pkt->seqnum == p->packet->seqnum) {
                // paket uz mame ve fronte, je duplikovanej.. posleme ack a drop
                _ipk_verbose_log("ignoring packet #%hu, should be already acked", pkt->seqnum);
                goto in_ack;
            }
            else {
                break;
            }
        }

        assert(actWnd);
        actWnd->packet = pkt;

        // vyjmeme okno
        rdt->window_free = actWnd->next;
        if (prevWnd != NULL) {
            actWnd->next = prevWnd->next;
            prevWnd->next = actWnd;
        }
        else {
            actWnd->next = rdt->window;
            rdt->window = actWnd;
        }

in_ack:
        _ipk_debug_log("nextseqnum #%hu", rdt->nextseqnum);
        for (rdt_wnd_t *p = rdt->window; p != NULL; p = p->next) {
            _ipk_debug_log("wnd %p, packet #%hu", (void*)p, p->packet ? p->packet->seqnum : 0);
        }

        // zpracujeme pakety v okne podle poradi
        for (rdt_wnd_t *p = rdt->window, *q = NULL; p && p->packet 
             && p->packet->seqnum == rdt->nextseqnum; p = q)
        {
            q = p->next;

            if (!_rdt_server_process(rdt, p->packet)) {
                break;
            }

            // uvolnime pamet
            _rdt_packet_free(p->packet);
            p->packet = NULL;

            // uvolnime okno k dalsimu pouziti
            rdt->window = q;
            p->next = rdt->window_free;
            rdt->window_free = p;

            --numFullWnd;
            rdt->nextseqnum++;
            _ipk_verbose_log("rcv_base set to %hu", rdt->nextseqnum);
        }

        rdt->cwnd = rdt->window_size - numFullWnd;
    }    
in_end:

    if (outbuf && *outbuflen > 0) {
        if (rdt->acks != NULL) {
            rdt_wnd_t* q = rdt->acks->next;
            size_t nbuf = _rdt_packet_write(rdt, rdt->acks->packet, outbuf, *outbuflen);
            assert(nbuf > 0);

            *outbuflen = nbuf;
            _rdt_packet_free(rdt->acks->packet);
            free(rdt->acks);
            rdt->acks = q;
        }
        else {
            *outbuflen = 0;
        }
    }
}

bool rdt_client_done(rdt_t* rdt)
{
    if (!rdt) {
        return true;
    }

    if (rdt->eof_rcvd) {
        return true;
    }
    else {
        rdt->eof_send = true;
        return false;
    }
}

void rdt_server_done(rdt_t* rdt)
{
    (void)rdt;

}

bool _rdt_server_process(rdt_t* rdt, rdt_pack_t* pkt)
{
    // dorucime data
    if (pkt->data && pkt->datalen > 0) {
        struct base64_decode_context b64ctx;
        base64_decode_ctx_init(&b64ctx);

        size_t needlen = BASE64_NEEDLENGTH(pkt->datalen);
        if (needlen + rdt->datalen > RDT_DATABUF_MAXLEN) {
            return false;
        }

        size_t offset = rdt->pdata - rdt->data;
        char* tail = &rdt->data[(offset + rdt->datalen) % RDT_DATABUF_MAXLEN];
        bool overflow = tail - rdt->data + needlen > RDT_DATABUF_MAXLEN;

        if (!overflow) {
            if (!base64_decode_ctx(&b64ctx, pkt->data, pkt->datalen, tail, &needlen)) {
                _ipk_error_log("base64 decode error");
            }
        }
        else {
            char* tmpbuf = (char*)alloca(needlen);
                        
            if (!base64_decode_ctx(&b64ctx, pkt->data, pkt->datalen, tmpbuf, &needlen)) {
                _ipk_error_log("base64 decode error");
            }

            size_t len_to_end = &rdt->data[RDT_DATABUF_MAXLEN] - tail;
            memcpy(tail, tmpbuf, len_to_end);
            memcpy(rdt->data, tmpbuf + len_to_end, needlen - len_to_end);
        }

        rdt->datalen += needlen;
    }

    return true;
}

bool rdt_data_write(rdt_t* rdt, char* buf, size_t len)
{
    if (!rdt) {
        return false;
    }
    if (!buf) {
        return false;
    }
    if (len == 0) {
        return false;
    }
    if (len + rdt->datalen > RDT_DATABUF_MAXLEN) {
        return false;
    }

    size_t offset = rdt->pdata - rdt->data;
    char* tail = &rdt->data[(offset + rdt->datalen) % RDT_DATABUF_MAXLEN];
    bool overflow = tail - rdt->data + len > RDT_DATABUF_MAXLEN;

    if (!overflow) {
        memcpy(tail, buf, len);
    }
    else {
        size_t len_to_end = &rdt->data[RDT_DATABUF_MAXLEN] - tail;
        memcpy(tail, buf, len_to_end);
        memcpy(rdt->data, buf + len_to_end, len - len_to_end);
    }

    rdt->datalen += len;
    return true;
}

size_t rdt_data_read(rdt_t* rdt, char* buf, size_t len)
{
    if (!rdt) {
        return 0;
    }
    if (!buf) {
        return 0;
    }

    len = min(len, rdt->datalen);
    if (len == 0) {
        return 0;
    }

    bool overflow = &rdt->pdata[len] > &rdt->data[RDT_DATABUF_MAXLEN];
    if (!overflow) {
        memcpy(buf, rdt->pdata, len);
        rdt->pdata += len;
    }
    else {
        size_t len_to_end = &rdt->data[RDT_DATABUF_MAXLEN] - rdt->pdata;

        memcpy(buf, rdt->pdata, len_to_end);
        memcpy(buf + len_to_end, rdt->data, len - len_to_end);

        rdt->pdata = &rdt->data[len - len_to_end];
    }

    rdt->datalen -= len;
    if (rdt->datalen == 0)
        rdt->pdata = rdt->data;

    return len;
}
