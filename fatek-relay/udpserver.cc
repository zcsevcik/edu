/*
 * udpserver.cc
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#include "udpserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <poll.h>
#include <unistd.h>

#include <cstring>

#include "address.h"

/* ==================================================================== */
udpserver::udpserver()
  : socket_fd { ::socket(AF_INET, SOCK_DGRAM, 0) }
{
}

/* ==================================================================== */
udpserver::~udpserver()
{
    if (socket_fd != -1) {
        ::close(socket_fd);
        socket_fd = -1;
    }
}

/* ==================================================================== */
bool udpserver::bind(unsigned short port)
{
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    std::memset(&sa.sin_zero[0], '\0', sizeof sa.sin_zero);

    return -1 != ::bind(native_handle(),
                        reinterpret_cast<struct sockaddr *>(&sa),
                        static_cast<socklen_t>(sizeof sa));
}

/* ==================================================================== */
bool udpserver::recv(void* buf, ssize_t* len, address& addr)
{
    ssize_t const maxlen = *len;
    *len = ::recvfrom(native_handle(), buf, *len, 0,
                      addr.to_sockaddr(), &(addr.size() = addr.max_size()));
    return (*len != -1 && *len < maxlen);
}

/* ==================================================================== */
bool udpserver::send(void const* buf, ssize_t len, address const& addr)
{
    return -1 != ::sendto(native_handle(), buf, len, 0,
                          addr.to_sockaddr(), addr.size());
}

/* ==================================================================== */
int udpserver::poll_read(int msec)
{
    struct pollfd set;
    set.fd = native_handle();
    set.events = POLLIN;
    set.revents = 0;

    return ::poll(&set, 1, msec);
}
