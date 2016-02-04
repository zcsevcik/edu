/*
 * serialport.cc
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#include "serialport.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <termios.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

/* ==================================================================== */
serialport::serialport(char const* ttyS)
  : tty_fd { ::open(ttyS, O_RDWR | O_NOCTTY | O_NDELAY
                          /* O_EXLOCK | O_TTY_INIT */ ) }
{
}

/* ==================================================================== */
serialport::~serialport()
{
    if (tty_fd != -1) {
        ::close(tty_fd);
        tty_fd = -1;
    }
}

/* ==================================================================== */
bool serialport::isatty() const {
    return ::isatty(native_handle());
}

/* ==================================================================== */
bool serialport::drain() const {
    return 0 == ::tcdrain(native_handle());
}

/* ==================================================================== */
std::string serialport::to_string() const
{
    return ::ttyname(native_handle());
}

/* ==================================================================== */
bool serialport::set_options(baud_rate baud_rate,
                             character_size character_size,
                             parity parity,
                             stop_bits stop_bits,
                             flow_control flow_control)
{
    struct termios tio;
    memset(&tio, '\0', sizeof tio);
    ::cfmakeraw(&tio);
    if (!set_option(tio, baud_rate))        return false;
    if (!set_option(tio, character_size))   return false;
    if (!set_option(tio, parity))           return false;
    if (!set_option(tio, stop_bits))        return false;
    if (!set_option(tio, flow_control))     return false;

    tio.c_cflag |= ( CLOCAL | CREAD );
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;

    flush();
    return -1 != ::tcsetattr(native_handle(), TCSANOW, &tio);
}

/* ==================================================================== */
bool serialport::flush() {
    return -1 != ::tcflush(native_handle(), TCIFLUSH);
}

/* ==================================================================== */
bool serialport::read(void* buf, ssize_t* len)
{
    ssize_t const maxlen = *len;
    ssize_t offset = 0;

  again:
    ssize_t result = ::read(native_handle(),
                            &static_cast<char *>(buf)[offset],
                            maxlen - offset);
    if (result > 0) {
        offset += result;
        goto again;
    }
    else if (result == 0) {
        *len = offset;
        return 1;
    }
    else {
        return 0;
    }
}

/* ==================================================================== */
bool serialport::write(void const* buf, ssize_t len)
{
    ssize_t offset = 0;

  again:
    ssize_t result = ::write(native_handle(),
                             &static_cast<char const*>(buf)[offset],
                             len - offset);
    if (result > 0) {
        offset += result;
        if (offset == len)
            return 1;
        goto again;
    }
    else {
        return 0;
    }
}

/* ==================================================================== */
int serialport::poll_read(int msec)
{
    struct pollfd set;
    set.fd = native_handle();
    set.events = POLLIN;
    set.revents = 0;

    return ::poll(&set, 1, msec);
}

/* ==================================================================== */
serialport::restore_tio::restore_tio(serialport& tty_p)
  : tty(tty_p)
  , tio()
{
    ::tcgetattr(tty.native_handle(), &tio);
}

/* ==================================================================== */
serialport::restore_tio::~restore_tio()
{
    ::tcsetattr(tty.native_handle(), TCSANOW, &tio);
}

/* ==================================================================== */
serialport::baud_rate::baud_rate(unsigned intval)
  : speed(B0)
{
    switch (intval) {
    default:
    case 0u: speed = B0; break;
    case 50u: speed = B50; break;
    case 75u: speed = B75; break;
    case 110u: speed = B110; break;
    case 134u: speed = B134; break;
    case 150u: speed = B150; break;
    case 200u: speed = B200; break;
    case 300u: speed = B300; break;
    case 600u: speed = B600; break;
    case 1200u: speed = B1200; break;
    case 1800u: speed = B1800; break;
    case 2400u: speed = B2400; break;
    case 4800u: speed = B4800; break;
    case 9600u: speed = B9600; break;
    case 19200u: speed = B19200; break;
    case 38400u: speed = B38400; break;
    case 57600u: speed = B57600; break;
    case 115200u: speed = B115200; break;
    case 230400u: speed = B230400; break;
    case 460800u: speed = B460800; break;
    case 500000u: speed = B500000; break;
    case 576000u: speed = B576000; break;
    case 921600u: speed = B921600; break;
    case 1000000u: speed = B1000000; break;
    case 1152000u: speed = B1152000; break;
    case 1500000u: speed = B1500000; break;
    case 2000000u: speed = B2000000; break;
    case 2500000u: speed = B2500000; break;
    case 3000000u: speed = B3000000; break;
    case 3500000u: speed = B3500000; break;
    case 4000000u: speed = B4000000; break;
    }
}

bool serialport::baud_rate::set(storage_type& tio) const {
    if (speed == B0) {
        errno = EINVAL;
        return false;
    }

    return 0 == ::cfsetspeed(&tio, speed);
}

/* ==================================================================== */
serialport::character_size::character_size(unsigned intval)
  : cs(intval)
{
}

bool serialport::character_size::set(storage_type& tio) const {
    if (cs < 5 || cs > 8)
        return false;

    tio.c_cflag &= ~CSIZE;
    switch (cs) {
    case 5: tio.c_cflag |= CS5; break;
    case 6: tio.c_cflag |= CS6; break;
    case 7: tio.c_cflag |= CS7; break;
    case 8: tio.c_cflag |= CS8; break;
    }

    return true;
}

/* ==================================================================== */
serialport::stop_bits::stop_bits(type t)
  : stopb(t)
{
}

bool serialport::stop_bits::set(storage_type& tio) const {
    switch (stopb) {
    case one: tio.c_cflag &= ~CSTOPB; break;
    case two: tio.c_cflag |=  CSTOPB; break;
    }

    return true;
}

/* ==================================================================== */
serialport::parity::parity(type t)
  : parb(t)
{
}

bool serialport::parity::set(storage_type& tio) const {
    switch (parb) {
    case none:
        tio.c_iflag |= IGNPAR;
        tio.c_cflag &= ~(PARENB | PARODD);
        break;
    case even:
        tio.c_iflag &= ~(IGNPAR | PARMRK);
        tio.c_iflag |= INPCK;
        tio.c_cflag |= PARENB;
        tio.c_cflag &= ~PARODD;
        break;
    case odd:
        tio.c_iflag &= ~(IGNPAR | PARMRK);
        tio.c_iflag |= INPCK;
        tio.c_cflag |= (PARENB | PARODD);
        break;
    }

    return true;
}

/* ==================================================================== */
serialport::flow_control::flow_control(type t)
  : flowctl(t)
{
}

bool serialport::flow_control::set(storage_type& tio) const {
    switch (flowctl) {
    case none:
        tio.c_iflag &= ~(IXOFF | IXON);
        tio.c_cflag &= ~CRTSCTS;
        break;
    case software:
        tio.c_iflag |= IXOFF | IXON;
        tio.c_cflag &= ~CRTSCTS;
        break;
    case hardware:
        tio.c_iflag &= ~(IXOFF | IXON);
        tio.c_cflag |= CRTSCTS;
        break;
    }

    return true;
}
