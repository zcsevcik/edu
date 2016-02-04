/*
 * serialport.h
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#ifndef SERIALPORT_H_
#define SERIALPORT_H_

#include <sys/types.h>
#include <termios.h>
#include <string>

/* ==================================================================== */
class serialport {
public:
    typedef struct termios  storage_type;
    typedef int             handle_type;

public:
    class baud_rate;
    class character_size;
    class stop_bits;
    class parity;
    class flow_control;

public:
    class restore_tio;

public:
    explicit serialport(char const* ttyS);
    ~serialport();

    bool set_options(baud_rate, character_size, parity, stop_bits, flow_control);

    handle_type native_handle() { return tty_fd; }
    handle_type native_handle() const { return tty_fd; }
    std::string to_string() const;

public:
    bool isatty() const;
    bool flush();
    bool read(void * buf, ssize_t* len);
    bool write(void const* buf, ssize_t len);
    int poll_read(int msec);
    bool drain() const;

private:
    serialport(serialport const&) = delete;
    serialport& operator=(serialport const&) = delete;

    serialport(serialport &&) = delete;
    serialport& operator=(serialport &&) = delete;

    template<class T>
    bool set_option(storage_type &tio, T const& opt) {
        return opt.set(tio);
    }

private:
    handle_type tty_fd;
/* ==================================================================== */
}; // class serialport

/* ==================================================================== */
class serialport::restore_tio {
    serialport &tty;
    storage_type tio;

public:
    explicit restore_tio(serialport &);
    ~restore_tio();
}; // class serialport::restore_tio

/* ==================================================================== */
class serialport::baud_rate {
    speed_t speed;

public:
    explicit baud_rate(unsigned = 9600);
    bool set(storage_type &) const;
}; // class serialport::baud_rate

/* ==================================================================== */
class serialport::character_size {
    unsigned cs;

public:
    explicit character_size(unsigned = 7);
    bool set(storage_type &) const;
}; // class serialport::character_size

/* ==================================================================== */
class serialport::stop_bits {
public:
    enum type { one, two };
    explicit stop_bits(type = one);
    bool set(storage_type &) const;

private:
    type stopb;

}; // class serialport::stop_bits

/* ==================================================================== */
class serialport::parity {
public:
    enum type { none, odd, even };
    explicit parity(type = even);
    bool set(storage_type &) const;

private:
    type parb;
}; // class serialport::parity

/* ==================================================================== */
class serialport::flow_control {
public:
    enum type { none, software, hardware };
    explicit flow_control(type = none);
    bool set(storage_type &) const;

private:
    type flowctl;
}; // class serialport::flow_control

#endif // SERIALPORT_H_
