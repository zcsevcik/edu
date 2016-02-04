/*
 * address.cc
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#include "address.h"

#include <sys/socket.h>
#include <netdb.h>

#include <cstring>

/* ==================================================================== */
std::string address::to_string() const
{
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];

    int r = ::getnameinfo(to_sockaddr(), size(),
                          host, static_cast<socklen_t>(sizeof host),
                          port, static_cast<socklen_t>(sizeof port),
                          NI_DGRAM | NI_NUMERICHOST | NI_NUMERICSERV);
    if (r != 0) { return gai_strerror(r); }

    std::string result;
    result.append(host);
    result.push_back(':');
    result.append(port);
    return result;
}
