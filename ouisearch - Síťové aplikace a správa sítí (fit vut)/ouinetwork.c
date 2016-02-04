/*
 * ouinetwork.c:   Mapovani adresoveho prostoru IPv6 pomoci OUI
 *                 ISA projekt #1: Programovani sitove sluzby
 *
 * Date:           2012/10/18
 * Author(s):      Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 *
 * This file is part of ouisearch.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "ouinetwork.h"

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#if defined(BSD)
# include <sys/endian.h>
#else
# include <endian.h>
#endif

#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/icmp6.h>
#include <netdb.h>

#include "oui_internal.h"
#include "ouidb.h"
#include "ouiqueue.h"

#if defined(__FreeBSD__)
# ifndef s6_addr8
#  define s6_addr8 __u6_addr.__u6_addr8
# endif
# ifndef s6_addr16
#  define s6_addr16 __u6_addr.__u6_addr16
# endif
# ifndef s6_addr32
#  define s6_addr32 __u6_addr.__u6_addr32
# endif
#endif

// ----------------------------------------------------------------------------
typedef void (*in6_yield_fn)(const struct in6_addr*, void* arg);

typedef struct {
    struct in6_addr prefix;
    uint8_t         prefixlen;
    in6_found_fn    callback;
    void*           arg;
    uint16_t        packets_per_sec;
} thread_params_t;

// ----------------------------------------------------------------------------
static bool _scan_done;
static pthread_t _thread;
static int _sockfd;

// ----------------------------------------------------------------------------
static void _generate_eui64s(
        const struct in6_addr* prefix,
        const uint8_t oui[3],
        in6_yield_fn,
        void* obj);

static void _generate_subnets(
        struct in6_addr,
        uint8_t prefixlen,
        in6_yield_fn,
        void* obj);

static void _subnet_result(
        const struct in6_addr*,
        void* obj);

static void _oui_result(
        const uint8_t oui[3],
        const char* name,
        void* obj);

static void _eui64_result(
        const struct in6_addr*,
        void* obj);

// ----------------------------------------------------------------------------
static inline
void _addr6cpy(struct in6_addr* lhs,
               const struct in6_addr* rhs)
{
    assert(lhs != NULL);
    assert(rhs != NULL);

    // je tohle rychlejsi nez memcpy??
    lhs->s6_addr32[0] = rhs->s6_addr32[0];
    lhs->s6_addr32[1] = rhs->s6_addr32[1];
    lhs->s6_addr32[2] = rhs->s6_addr32[2];
    lhs->s6_addr32[3] = rhs->s6_addr32[3];
}

static inline
void _normalize_subnet(struct in6_addr* addr, uint8_t prefixlen)
{
    assert(addr != NULL);
    assert(prefixlen > 0 && prefixlen <= 64);

    const uint64_t subnet_max =
        (UINT64_C(1) << (64 - prefixlen)) - UINT64_C(1);

    const uint64_t net_mask = subnet_max ^ UINT64_MAX;

    addr->s6_addr32[0] &= htobe32(net_mask >> UINT32_C(32));
    addr->s6_addr32[1] &= htobe32(net_mask & UINT32_MAX);
    addr->s6_addr32[2] = UINT32_C(0);
    addr->s6_addr32[3] = UINT32_C(0);
}

static inline
bool _addr6inrange(const struct in6_addr* prefix,
                   uint8_t prefixlen,
                   struct in6_addr addr)
{
    assert(prefix != NULL);
    assert(prefixlen > 0 && prefixlen <= 64);

    _normalize_subnet(&addr, prefixlen);

    if (addr.s6_addr32[0] == prefix->s6_addr32[0] &&
        addr.s6_addr32[1] == prefix->s6_addr32[1])
    { return true; }

    return false;
}


int _addr2name6(struct in6_addr addr,
                char* buf, size_t len)
{
    struct sockaddr_in6 sockaddr = {
        .sin6_family = AF_INET6,
        .sin6_addr = addr,
    };

    return getnameinfo(
        (struct sockaddr*)&sockaddr, (socklen_t)sizeof(sockaddr),
        buf, (socklen_t)len,
        NULL, (socklen_t)0,
        NI_NUMERICHOST);
}

// ----------------------------------------------------------------------------
static
void* _threadproc(void* arg)
{
    thread_params_t* params = (thread_params_t*)arg;
    assert(params != NULL);
    assert(params->callback != NULL);

    // pid potrebujeme pro kontrolu, zda-li zprava patri nam
    pid_t pid = getpid();

    // ------------------------------------------------------------------------
    // nastavime si odesilana data vc. par bajtu payloadu
    // ------------------------------------------------------------------------
    struct t_pack {
        struct icmp6_hdr icmp6_echoreq;
        char payload[4];
    };

    struct t_pack packet = {
        .icmp6_echoreq = {
            .icmp6_type = ICMP6_ECHO_REQUEST,
            .icmp6_code = 0,
        }
    };
    packet.icmp6_echoreq.icmp6_seq = 0;
    packet.icmp6_echoreq.icmp6_id = htobe16(pid);

    for (size_t i = 0; i < _countof(packet.payload); ++i) {
        packet.payload[i] = 'A' + (char)(i % 26);
    }

    // ------------------------------------------------------------------------
    // naplnime struktury pro sendmsg
    // ------------------------------------------------------------------------
    struct sockaddr_in6 sa = {
        .sin6_family = AF_INET6,
    };

    struct pollfd fds = {
        .fd = _sockfd,
        .events = POLLIN | POLLOUT,
        .revents = 0,
    };

    struct iovec iov = {
        .iov_base = &packet,
        .iov_len = sizeof(packet),
    };

    struct msghdr msg = {
        .msg_name = &sa,
        .msg_namelen = (socklen_t)sizeof(sa),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };

    // ------------------------------------------------------------------------
    // promenne pro omezovac rychlosti
    // ------------------------------------------------------------------------
    const double interval = 1E+3f / (double)params->packets_per_sec;

    double t0 = 0.0f;
    double dt = 0.0f;

    const int timeout_ms = 100;
    size_t endwait_sec = 3 * (1000 / timeout_ms);

    do {
        // --------------------------------------------------------------------
        if (_terminate) {
            break;
        }
        if (!endwait_sec) {
            break;
        }

        // cas zaktualizujeme pouze tehdy, kdyz jsme odeslali zpravu
        bool sent = false;

        // D.E.Knuth - The Art of Computer Programming: Seminumerical algorithms
        bool interval_gt = (dt - interval) > ( (fabs(dt) < fabs(interval) ? fabs(interval) : fabs(dt)) * DBL_EPSILON);
        if (interval_gt && !_scan_done) {
            fds.events |= POLLOUT;
        }
        else {
            fds.events &= ~POLLOUT;
        }

        // po dokonceni skenovani jeste prijima pakety po dobu 3s
        if (_scan_done) {
            --endwait_sec;
        }

        // ----------------------------------------------------------------
        // cekame na udalost
        // ----------------------------------------------------------------
        int count = poll(&fds, 1, timeout_ms);
        if (count == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            _terminate = true;
            break;
        }
        else if (count > 0) {
            // ----------------------------------------------------------------
            // dostali jsme odpoved ?
            // ----------------------------------------------------------------
            if (fds.revents & POLLIN) {
                struct sockaddr_in6 sockaddr;
                socklen_t sockaddr_len = sizeof(sockaddr);

                char buf[1280]; // RFC2460 <http://tools.ietf.org/html/rfc2460#section-5>
                ssize_t recvlen = recvfrom(fds.fd, buf, _countof(buf), MSG_WAITALL,
                    (struct sockaddr*)&sockaddr, &sockaddr_len);

                if (recvlen == -1) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    _terminate = true;
                    break;
                }

                // diky filtru vime, ze jsme dostali ICMPv6 zpravu
                // zkontrolujeme, jestli patri nam a je v nasem rozsahu
                struct icmp6_hdr* reply = (struct icmp6_hdr*)buf;
                if (_addr6inrange(&params->prefix, params->prefixlen, sockaddr.sin6_addr) &&
                    (size_t)recvlen >= sizeof(struct icmp6_hdr) &&
                    reply->icmp6_type == ICMP6_ECHO_REPLY &&
                    reply->icmp6_code == 0 &&
                    be16toh(reply->icmp6_id) == pid)
                {
                    params->callback(sockaddr.sin6_addr, params->arg);
                }

            }
            // ----------------------------------------------------------------
            // jsme pripraveni zapisovat ? mame adresu, kam zpravu poslat ?
            // ----------------------------------------------------------------
            if (fds.revents & POLLOUT) {
                if (ouiqueue_pop(&sa.sin6_addr)) {
                    ssize_t sendlen = sendmsg(fds.fd, &msg, 0);
                    if (sendlen == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        _terminate = true;
                        break;
                    }
                    else if (sendlen > 0) {
                        sent = true;
                    }
                }
            }
        }

        // zaktualizujeme cas pro omezovac rychlosti
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        double t = 1E+3f * now.tv_sec + 1E-6f * now.tv_nsec;

        dt = t - t0;
        if (sent) {
            t0 = t;
        }
    } while (1);

    return NULL;
}

// ----------------------------------------------------------------------------
static
void _generate_eui64(const struct in6_addr* prefix,
                     const uint8_t oui[3],
                     const uint8_t nic[3],
                     struct in6_addr* eui64)
{
    assert(prefix != NULL);
    assert(oui != NULL);
    assert(nic != NULL);
    assert(eui64 != NULL);

    // RFC4291 <http://tools.ietf.org/html/rfc4291#appendix-A>
    eui64->s6_addr32[0] = prefix->s6_addr32[0];
    eui64->s6_addr32[1] = prefix->s6_addr32[1];
    eui64->s6_addr[8] = oui[0] ^ UINT8_C(0x02);
    eui64->s6_addr[9] = oui[1];
    eui64->s6_addr[10] = oui[2];
    eui64->s6_addr[11] = UINT8_C(0xFF);
    eui64->s6_addr[12] = UINT8_C(0xFE);
    eui64->s6_addr[13] = nic[0];
    eui64->s6_addr[14] = nic[1];
    eui64->s6_addr[15] = nic[2];
}

void _generate_eui64s(const struct in6_addr* subnet,
                      const uint8_t oui[3],
                      in6_yield_fn yield_return,
                      void* obj)
{
    if (!yield_return) {
        return;
    }

    if (_oui_debug()) {
        char ip_first[INET6_ADDRSTRLEN], ip_last[INET6_ADDRSTRLEN];
        struct in6_addr eui64_first, eui64_last;
        const uint8_t nic_first[] = { 0x00, 0x00, 0x00 };
        const uint8_t nic_last[]  = { 0xFF, 0xFF, 0xFF };

        _generate_eui64(subnet, oui, nic_first, &eui64_first);
        _generate_eui64(subnet, oui, nic_last, &eui64_last);

        _addr2name6(eui64_first, ip_first, _countof(ip_first));
        _addr2name6(eui64_last, ip_last, _countof(ip_last));

        _oui_log("Skenuji rozsah eui64: %s - %s", ip_first, ip_last);
    }

    for (uint32_t i = 0; i < UINT32_C(0xFFFFFF); ++i) {
        struct in6_addr eui64;
        const uint8_t nic[] = {
            i >> UINT32_C(16),
            i >> UINT32_C(8),
            i & UINT32_C(0xFF)
        };

        if (_terminate)
            break;

        _generate_eui64(subnet, oui, nic, &eui64);
        yield_return(&eui64, obj);
    }
}

void _generate_subnets(struct in6_addr net, uint8_t prefixlen,
                       in6_yield_fn yield_return, void* obj)
{
    if (!yield_return) {
        return;
    }

    const uint64_t subnet_max =
        (UINT64_C(1) << (64 - prefixlen)) - UINT64_C(1);

    if (_oui_debug()) {
        char ip_first[INET6_ADDRSTRLEN], ip_last[INET6_ADDRSTRLEN];
        struct in6_addr subnet_first, subnet_last;

        _addr6cpy(&subnet_first, &net);
        _addr6cpy(&subnet_last, &net);

        subnet_last.s6_addr32[0] |= htobe32(subnet_max >> UINT32_C(32));
        subnet_last.s6_addr32[1] |= htobe32(subnet_max & UINT32_MAX);

        _addr2name6(subnet_first, ip_first, _countof(ip_first));
        _addr2name6(subnet_last, ip_last, _countof(ip_last));

        _oui_log("Skenuji sit: %s/%hu", ip_first, prefixlen);
        _oui_log("rozsah podsiti: %s/64 - %s/64", ip_first, ip_last);
    }

    for (uint64_t i = 0; i <= subnet_max; ++i) {
        struct in6_addr subnet;
        _addr6cpy(&subnet, &net);

        subnet.s6_addr32[0] |= htobe32(i >> UINT32_C(32));
        subnet.s6_addr32[1] |= htobe32(i & UINT32_MAX);

        if (_terminate)
            break;

        yield_return(&subnet, obj);
    }
}

void _oui_result(const uint8_t oui[3], const char* name, void* obj)
{
    (void)name;
    (void)obj;

    const struct in6_addr* subnet = (const struct in6_addr*)obj;
    _generate_eui64s(subnet, oui, &_eui64_result, NULL);
}

void _subnet_result(const struct in6_addr* subnet, void* obj)
{
    (void)obj;

    ouidb_foreach(&_oui_result, (void*)subnet);
}

void _eui64_result(const struct in6_addr* eui64, void* obj)
{
    (void)obj;

    while (!_terminate) {
        // po 100ms intervalech cekame, az se ve fronte uvolni misto
        struct timespec time;
        clock_gettime(CLOCK_REALTIME, &time);
        time.tv_nsec += 100000000L; // 100msec

        // normalizace
        if (time.tv_nsec >= 1000000000L) {
            time.tv_sec += 1;
            time.tv_nsec -= 1000000000L;
        }

        // ulozime adresu
        if (ouiqueue_push(*eui64, &time))
            break;
    }
}

// ----------------------------------------------------------------------------
bool oui_scan_network(struct in6_addr prefix, uint8_t prefixlen,
                      uint16_t packets_per_sec,
                      in6_found_fn callback, void* arg)
{
    int pth_hr;
    _scan_done = false;

    // zkontrolujeme adresu
    if (prefixlen == 0 || prefixlen > 64)
    {
        fprintf(stderr, "Tenhle prefix pouzit nepujde.\n");
        return false;
    }
    if (IN6_IS_ADDR_UNSPECIFIED(&prefix) ||
        IN6_IS_ADDR_LOOPBACK(&prefix) ||
        IN6_IS_ADDR_MULTICAST(&prefix) ||
        IN6_IS_ADDR_V4MAPPED(&prefix) ||
        IN6_IS_ADDR_V4COMPAT(&prefix) ||
        // o strukture unicast adresy s prefixem 000 nic nevime
        !(prefix.s6_addr[0] & UINT8_C(0xE0)))
    {
        fprintf(stderr, "Tenhle prefix pouzit nepujde.\n");
        return false;
    }

    // vytvorime soket
    _sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    if (_sockfd == -1) {
        fprintf(stderr, "%s\n", strerror(errno != EPERM ? errno : EACCES));
        return false;
    }

    // vyfiltrujeme zpravy typu ICMP6_ECHO_REPLY #129
    struct icmp6_filter filter;
    ICMP6_FILTER_SETBLOCKALL(&filter);
    ICMP6_FILTER_SETPASS(ICMP6_ECHO_REPLY, &filter);
    if (-1 == setsockopt(_sockfd, IPPROTO_ICMPV6, ICMP6_FILTER,
                         &filter, (socklen_t)sizeof(struct icmp6_filter)))
    {
        fprintf(stderr, "%s\n", strerror(errno));
        close(_sockfd);
        return false;
    }

    // znormalizujeme adresu site
    _normalize_subnet(&prefix, prefixlen);

    // vytvorime vlakno pro obsluhu soketu
    thread_params_t threadparam = {
        .prefix = prefix,
        .prefixlen = prefixlen,
        .packets_per_sec = packets_per_sec,
        .callback = callback,
        .arg = arg,
    };

    pth_hr = pthread_create(&_thread, NULL, &_threadproc, &threadparam);
    if (pth_hr != 0) {
        fprintf(stderr, "%s\n", strerror(pth_hr));
        close(_sockfd);
        return false;
    }

    // zacneme generovat adresy pro skenovani
    _generate_subnets(prefix, prefixlen, &_subnet_result, NULL);

    // pockame na vlakno, az vse zpracuje
    _scan_done = true;
    pth_hr = pthread_join(_thread, NULL);
    if (pth_hr != 0) {
        fprintf(stderr, "%s\n", strerror(pth_hr));
    }

    // uvolnime po sobe prostredky
    close(_sockfd);

    return true;
}
