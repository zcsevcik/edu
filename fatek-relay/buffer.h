/*
 * buffer.h
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/24
 *
 * This file is part of fatek-relay.
 */

#ifndef BUFFER_H_
#define BUFFER_H_

#include <sys/types.h>
#include <cstring>
#include <memory>
#include <new>

/* ==================================================================== */
class buffer {
public:
    typedef size_t              size_type;
    typedef ssize_t             difference_type;
    typedef char                value_type;
    typedef value_type*         pointer;
    typedef value_type const*   const_pointer;
    typedef value_type&         reference;
    typedef value_type const&   const_reference;

    buffer()
      : arr { new(std::nothrow) char[ max_size() ] }
      , ptr { data() }
    {
    }

    explicit operator bool() const {
        return static_cast<bool>(arr);
    }

    constexpr static size_type max_size() { return 592; }

    size_type       size() const    { return ptr - data(); }
    size_type       length() const  { return size(); }
    bool            empty() const   { return ptr == data(); }
    size_type       capacity() const { return max_size(); }

    pointer         data()          { return &arr[0]; }
    const_pointer   data() const    { return &arr[0]; }
    reference       front()         { return arr[0]; }
    const_reference front() const   { return arr[0]; }
    reference       back()          { return ptr[-1]; }
    const_reference back() const    { return ptr[-1]; }
    reference       operator[](size_type pos)       { return arr[pos]; }
    const_reference operator[](size_type pos) const { return arr[pos]; }

    pointer         begin()         { return &arr[0]; }
    const_pointer   begin() const   { return &arr[0]; }
    const_pointer   cbegin() const  { return &arr[0]; }
    pointer         end()           { return ptr; }
    const_pointer   end() const     { return ptr; }
    const_pointer   cend() const    { return ptr; }

    void clear()    { ptr = data(); }
    void pop_back() { --ptr; }
    void push_back(value_type c) { *ptr++ = c; }

    void append(pointer str, size_type len) {
        std::memcpy(ptr, str, len);
        ptr += len;
    }

    void commit(size_type len) {
        ptr += len;
    }

private:
    buffer(buffer const&) = delete;
    buffer& operator=(buffer const&) = delete;
    buffer(buffer &&) = delete;
    buffer& operator=(buffer &&) = delete;

private:
    std::unique_ptr<value_type []> arr;
    pointer ptr;
/* ==================================================================== */
}; /* class buffer */

#endif /* BUFFER_H_ */
