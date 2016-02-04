/*
 * Datum:   2012/04/14
 * Autor:   Radek Sevcik, xsevci44@stud.fit.vutbr.cz
 * Projekt: Spolehliva komunikace, projekt c. 3 pro predmet IPK
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "sock.h"
#include "log.h"

#include <assert.h>
#include <string.h>

bool ipk_sock_init()
{
    // kdyby se na !defined(_WIN32) nadefinovalo jako
    // #define ipk_sock_init() true, nastala by hlaska
    // warning: statement with no effect

#ifdef _WIN32
    struct WSAData wsadata;
    int hr = WSAStartup(WINSOCK_VERSION, &wsadata);
    if (hr != 0)
        ipk_sock_perror(hr);

    return hr == 0;
#else
    return true;
#endif
}

bool ipk_sock_free()
{
#ifdef _WIN32
    int hr = WSACleanup();
    if (hr != 0)
        ipk_sock_perror(ipk_sock_errno);

    return hr == 0;
#else
    return true;
#endif
}

bool ipk_sock_is_recoverable(int error)
{
#ifndef _WIN32
    return error == EAGAIN
        || error == EWOULDBLOCK;
#else
    return error == WSAEINPROGRESS
        || error == WSAEWOULDBLOCK;
#endif
}

void ipk_sock_perror(int error)
{
    if (error == 0)
        return;

    char* errmsg = NULL;

#ifndef _WIN32
    errmsg = strerror(error);
#else
    unsigned long hr = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        0U, error, LANG_NEUTRAL, (char *)&errmsg, 0U, NULL);
    assert(hr > 0);
#endif

    if (errmsg) {
        _ipk_error_log("Chyba socketu #%d: %s", error, errmsg);
    }
    else {
        _ipk_error_log("Chyba socketu #%d", error);
    }

#ifdef _WIN32
    LocalFree(errmsg);
#endif
}

bool ipk_fd_block_mode(ipk_sock_t sock, bool blocking)
{
    assert(sock != IPK_SOCK_INVALID);

#ifdef _WIN32
    unsigned long argp = blocking ? 0 : 1;
    if (0 != ioctlsocket(sock, FIONBIO, &argp)) {
        ipk_sock_perror(ipk_sock_errno);
        return false;
    }
#else
    int flags = fcntl(sock, F_GETFL);
    if (flags == -1) {
        ipk_sock_perror(ipk_sock_errno);
        return false;
    }

    flags = blocking ? flags & ~O_NONBLOCK : flags | O_NONBLOCK;

    if (-1 == fcntl(sock, F_SETFL, flags)) {
        ipk_sock_perror(ipk_sock_errno);
        return false;
    }
#endif

    return true;
}
