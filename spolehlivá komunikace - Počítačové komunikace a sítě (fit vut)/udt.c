/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <string.h>

#include "log.h"
#include "udt.h"

ipk_sock_t udt_init(char* local_port)
{
    assert(local_port != NULL);

    struct addrinfo hints, *result;
    int gai_hr;

    memset(&hints, '\0', sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_ADDRCONFIG | AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    gai_hr = getaddrinfo("localhost", local_port, &hints, &result);
    if (gai_hr != 0) {
#ifdef _WIN32
        ipk_sock_perror(ipk_sock_errno);
#else
        _ipk_error_log("Chyba getaddrinfo(\"%s\", \"%s\"): %s",
            "localhost", local_port, gai_strerror(gai_hr));
#endif
        return IPK_SOCK_INVALID;
    }

    _ipk_verbose_log("Listening at localhost:%s", local_port);
    ipk_sock_t udt = IPK_SOCK_INVALID;

    for (struct addrinfo* p = result; p != NULL; p = p->ai_next) {
        udt = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        _ipk_verbose_log("Created socket(%d, %d, %d): #%x",
            p->ai_family, p->ai_socktype, p->ai_protocol, udt);

        if (udt == IPK_SOCK_INVALID) {
            continue;
        }

        if (!ipk_fd_block_mode(udt, false)) {
            _ipk_verbose_log("fnctl failed #%x", udt);
            ipk_sock_perror(ipk_sock_errno);
            ipk_sock_close(udt);
            udt = IPK_SOCK_INVALID;
            continue;
        }

        if (-1 == bind(udt, p->ai_addr, (socklen_t)p->ai_addrlen)) {
            _ipk_verbose_log("Bind failed #%x", udt);
            ipk_sock_perror(ipk_sock_errno);
            ipk_sock_close(udt);
            udt = IPK_SOCK_INVALID;
            continue;        
        }

        break;
    }

    freeaddrinfo(result);
    return udt;
}

bool udt_connect(ipk_sock_t udt, char* remote_port)
{
    assert(remote_port != NULL);

    struct addrinfo hints, *result;
    int gai_hr;

    memset(&hints, '\0', sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_flags = AI_ADDRCONFIG;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    gai_hr = getaddrinfo("localhost", remote_port, &hints, &result);
    if (gai_hr != 0) {
#ifdef _WIN32
        ipk_sock_perror(ipk_sock_errno);
#else
        _ipk_error_log("Chyba getaddrinfo(\"%s\", \"%s\"): %s",
            "localhost", remote_port, gai_strerror(gai_hr));
#endif
        return false;
    }

    _ipk_verbose_log("Connecting to localhost:%s", remote_port);
    if (-1 == connect(udt, result->ai_addr, (socklen_t)result->ai_addrlen)) {
        _ipk_verbose_log("Connect failed #%x", udt);
        ipk_sock_perror(ipk_sock_errno);
        return false;
    }

    return true;
}

int udt_recv(ipk_sock_t udt, char *buf, size_t nbytes)
{
    if (udt == IPK_SOCK_INVALID) {
        return -1;
    }
    if (buf == NULL) {
        return -1;
    }
    if (nbytes == 0) {
        return -1;
    }
    if (nbytes > UDT_PACKET_MAXLEN) {
        return -1;
    }

#ifdef _WIN32
    int nrecv = recv(udt, buf, nbytes, 0);
#else
    ssize_t nrecv = recv(udt, buf, nbytes, MSG_DONTWAIT);
#endif

    if (nrecv == -1) {
        return ipk_sock_is_recoverable(ipk_sock_errno) ? 0 : -1;
    }
    
    return nrecv;
}

int udt_send(ipk_sock_t udt, char *buf, size_t nbytes)
{
    if (udt == IPK_SOCK_INVALID) {
        return -1;
    }
    if (buf == NULL) {
        return -1;
    }
    if (nbytes == 0) {
        return -1;
    }
    if (nbytes > UDT_PACKET_MAXLEN) {
        return -1;
    }

#ifdef _WIN32
    int nsend = send(udt, buf, nbytes, 0);
#else
    ssize_t nsend = send(udt, buf, nbytes, MSG_DONTWAIT);
#endif

    if (nsend == -1) {
        return ipk_sock_is_recoverable(ipk_sock_errno) ? 0 : -1;
    }

    return nsend;
}
