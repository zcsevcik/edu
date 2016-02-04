/*
 * udpserver.h
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include <sys/types.h>

/* ==================================================================== */
class address;

/* ==================================================================== */
class udpserver {
    typedef int handle_type;

public:
    udpserver();
    ~udpserver();

    handle_type native_handle() { return socket_fd; }
    bool bind(unsigned short port);

    bool recv(void      * buf, ssize_t* len, address      &);
    bool send(void const* buf, ssize_t  len, address const&);

    int poll_read(int msec);

private:
    udpserver(udpserver const&) = delete;
    udpserver& operator=(udpserver const&) = delete;

    udpserver(udpserver &&) = delete;
    udpserver& operator=(udpserver &&) = delete;

private:
    handle_type socket_fd;
/* ==================================================================== */
}; // class udpserver

#endif // UDPSERVER_H_
