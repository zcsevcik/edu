/*
 * address.h
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#ifndef ADDRESS_H_
#define ADDRESS_H_

#include <sys/socket.h>
#include <string>

/* ==================================================================== */
class address {
public:
    typedef socklen_t               size_type;
    typedef struct sockaddr_storage storage_type;
    typedef struct sockaddr*        pointer;
    typedef struct sockaddr const*  const_pointer;

public:
    address()
      : salen(max_size())
      , sa()
    {
    }

    std::string to_string() const;

    pointer       to_sockaddr();
    const_pointer to_sockaddr() const;

    size_type&       size();
    size_type const& size() const;

    constexpr static size_type max_size() {
        return sizeof(storage_type);
    }

private:
    address(address const&) = delete;
    address& operator=(address const&) = delete;

    address(address &&) = delete;
    address& operator=(address &&) = delete;

private:
    size_type salen;
    storage_type sa;
/* ==================================================================== */
}; // class address

/* ==================================================================== */
inline address::pointer address::to_sockaddr() {
    return reinterpret_cast<struct sockaddr*>(&sa);
}

inline address::const_pointer address::to_sockaddr() const {
    return reinterpret_cast<struct sockaddr const*>(&sa);
}

/* ==================================================================== */
inline address::size_type& address::size() {
    return salen;
}

inline address::size_type const& address::size() const {
    return salen;
}

/* ==================================================================== */

#endif // ADDRESS_H_
