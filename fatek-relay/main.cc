/*
 * main.cc
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/19
 *
 * This file is part of fatek-relay.
 */

#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "address.h"
#include "buffer.h"
#include "packet.h"
#include "serialport.h"
#include "udpserver.h"

#define DEFAULT_PORT            500
#define XSTR(s) #s
#define STR(s)  XSTR(s)

struct {
    char const*                 stty = "/dev/ttyS0";
    serialport::baud_rate       baud_rate { 9600 };
    serialport::character_size  character_size { 7 };
    serialport::parity          parity { serialport::parity::even };
    serialport::stop_bits       stop_bits { serialport::stop_bits::one };
    serialport::flow_control    flow_control { serialport::flow_control::none };
} static args;

/* ==================================================================== */
[[noreturn]] static void die(char const* s) {
    fprintf(stderr, "%s\n", s);
    exit(1);
}

static void network_error(address const& addr, const char *reason) {
    fprintf(stderr, "[%s] DROP: %s\n", addr.to_string().c_str(), reason);
}

/* ==================================================================== */
[[noreturn]] static void usage() {
    extern char const* __progname;
    fprintf(stderr, "Usage: %s /dev/ttyS0 9600 7E1\n", __progname);
    exit(1);
}

/* ==================================================================== */
static constexpr unsigned long const_hash(char const* s, size_t idx)
{   /* djb2 */
    return ( idx != (size_t) -1 )
       ? ( ( 33UL * const_hash( s, idx-1 ) ) ^ (unsigned long) s[idx] )
       :     5381UL;
}

static constexpr unsigned long operator "" _hash(char const *s, size_t len) {
    return const_hash( s, len - 1 );
}

static serialport::baud_rate to_baud_rate(char const* baud_rate) {
    char const* p_s;
    unsigned long hash = 5381UL;
    for ( p_s = baud_rate; ('0' <= *p_s && *p_s <= '9'); ++p_s ) {
        hash = ( 33UL * hash ) ^ (unsigned long) *p_s;
    }
    if (*p_s != '\0') goto invalid;

    /* some bauds are unsupported; TERMIOS(3) */

    switch ( hash ) {
    case "0"_hash:      return serialport::baud_rate(0);
    case "4800"_hash:   return serialport::baud_rate(4800);
    case "9600"_hash:   return serialport::baud_rate(9600);
    case "19200"_hash:  return serialport::baud_rate(19200);
    case "38400"_hash:  return serialport::baud_rate(38400);
    case "76800"_hash:  return serialport::baud_rate(76800);
    case "153600"_hash: return serialport::baud_rate(153600);
    case "307200"_hash: return serialport::baud_rate(307200);
    case "614400"_hash: return serialport::baud_rate(614400);
    case "7200"_hash:   return serialport::baud_rate(7200);
    case "14400"_hash:  return serialport::baud_rate(14400);
    case "28800"_hash:  return serialport::baud_rate(28800);
    case "57600"_hash:  return serialport::baud_rate(57600);
    case "115200"_hash: return serialport::baud_rate(115200);
    case "230400"_hash: return serialport::baud_rate(230400);
    case "921600"_hash: return serialport::baud_rate(921600);
    default: { invalid: die("invalid baud rate"); }
    }
}

static serialport::parity to_parity(char c) {
    typedef serialport::parity parity_t;
    switch (c) {
    case 'N': return parity_t(parity_t::none);
    case 'O': return parity_t(parity_t::odd);
    case 'E': return parity_t(parity_t::even);
    default: die("invalid parity");
    }
}

static serialport::stop_bits to_stop_bits(char stopb) {
    typedef serialport::stop_bits stop_bits_t;
    switch (stopb) {
    case '1': return stop_bits_t(stop_bits_t::one);
    case '2': return stop_bits_t(stop_bits_t::two);
    default: die("invalid stop bits");
    }
}

static serialport::character_size to_character_size(char cs) {
    switch (cs) {
    case '7': return serialport::character_size(7);
    case '8': return serialport::character_size(8);
    default: die("invalid character size");
    }
}

/* ==================================================================== */
static bool is_udpserver(udpserver& srv) {
    if (srv.native_handle() == -1) {
        perror("socket()");
        return false;
    }
    return true;
}

static bool init_udpserver(udpserver& srv) {
    if (!srv.bind(DEFAULT_PORT)) {
        perror("bind(* " STR(DEFAULT_PORT) "/udp)");
        return false;
    }
    return true;
}

/* ==================================================================== */
static bool is_stty(serialport& com) {
    if (com.native_handle() == -1) {
        perror(args.stty);
        return false;
    }
    if (!com.isatty()) {
        fprintf(stderr, "not an tty\n");
        return false;
    }
    return true;
}

static bool init_stty(serialport& com) {
    if (!com.set_options(args.baud_rate, args.character_size, args.parity,
                         args.stop_bits, args.flow_control))
    {
        perror(com.to_string().c_str());
        return false;
    }

    return true;
}

/* ==================================================================== */
static void parse_args(int argc, char* argv[]) {
    switch (argc) {
    default: usage(); return;
    case 4:
        args.character_size = to_character_size((argv[3])[0]);
        args.parity         = to_parity(        (argv[3])[1]);
        args.stop_bits      = to_stop_bits(     (argv[3])[2]);
    case 3:
        args.baud_rate      = to_baud_rate(      argv[2]);
    case 2:
        args.stty           =                    argv[1];
    }
}

/* ==================================================================== */
volatile std::sig_atomic_t stop = 0;

static void on_sigint(int) {
    stop = 1;
}

/* ==================================================================== */
enum class packet_error {
    ok, sys, need_more, format, checksum, data_follows, timedout,
};

static packet_error
get_request(udpserver& udp, buffer& buf, address& from, packet& req)
{
    buf.clear();

    ssize_t len = buf.capacity();
    if (!udp.recv(buf.end(), &len, from))
        return packet_error::sys;
    buf.commit(len);

    int result = packet_parse(buf.data(), buf.size(), &req);
    if (result == 0)
        return packet_error::format;
    else if (result == -1)
        return packet_error::need_more;

    if (req.computed_checksum != req.checksum)
        return packet_error::checksum;
    if (buf.size() > req.length)
        return packet_error::data_follows;

    return packet_error::ok;
}

static int
post_request(serialport &stty, buffer const& buf)
{
    if (!stty.write(buf.data(), buf.size()))
        return false;
    if (!stty.drain())
        perror(stty.to_string().c_str());

    return true;
}

static packet_error
get_response(serialport& stty, buffer& buf, packet& rsp)
{
    buf.clear();

    do {
    if (stty.poll_read(1000) <= 0)
        return packet_error::timedout;

    ssize_t len = buf.capacity() - buf.size();
    if (!stty.read(buf.end(), &len))
        return packet_error::sys;
    buf.commit(len);

    if (buf.front() != STX)
        return packet_error::format;
    } while (buf.back() != ETX);

    int result = packet_parse(buf.data(), buf.size(), &rsp);
    if (result == 0)
        return packet_error::format;
    else if (result == -1)
        return packet_error::need_more;

    if (rsp.computed_checksum != rsp.checksum)
        return packet_error::checksum;
    if (buf.size() > rsp.length)
        return packet_error::data_follows;

    return packet_error::ok;
}

static int
post_response(udpserver &udp, buffer const& buf, address const& to)
{
    if (!udp.send(buf.data(), buf.size(), to))
        return false;

    return true;
}

static int process(serialport &stty, udpserver &udp) {
    address sender;
    buffer buf;

    if (!buf) {
        perror(nullptr);
        return 1;
    }

    while (!stop) {

    /* because of INT, TERM signals */
    if (udp.poll_read(3000) <= 0) continue;


    packet request_packet;
    switch ( get_request(udp, buf, sender, request_packet) ) {
    default:
    case packet_error::ok:
        break;
    case packet_error::sys:
        network_error(sender, "system error");
        perror(nullptr);
        continue;
    case packet_error::need_more:
        network_error(sender, "unexpected packet end");
        continue;
    case packet_error::format:
        network_error(sender, "invalid packet");
        continue;
    case packet_error::checksum:
        network_error(sender, "checksum error");
        continue;
    case packet_error::data_follows:
        network_error(sender, "unknown data after end");
        continue;
    }


    if ( !post_request(stty, buf) ) {
        network_error(sender, "write to tty failed");
        perror(nullptr);
        continue;
    }


    packet response_packet;
    switch ( get_response(stty, buf, response_packet) ) {
    default:
    case packet_error::ok:
        break;
    case packet_error::timedout:
        network_error(sender, "timedout");
        continue;
    case packet_error::sys:
        network_error(sender, "system error");
        perror(nullptr);
        continue;
    case packet_error::need_more:
        network_error(sender, "unexpected packet end");
        continue;
    case packet_error::format:
        network_error(sender, "invalid packet");
        continue;
    case packet_error::checksum:
        network_error(sender, "checksum error");
        continue;
    case packet_error::data_follows:
        network_error(sender, "unknown data after end");
        continue;
    }


    if (request_packet.station_no != response_packet.station_no) {
        network_error(sender, "station no. error");
        continue;
    }
    if (request_packet.command_no != response_packet.command_no) {
        network_error(sender, "command no. error");
        continue;
    }


    if ( !post_response(udp, buf, sender) ) {
        network_error(sender, "system error");
        perror(nullptr);
        continue;
    }


    };

    return 0;
}

/* ==================================================================== */
int main(int argc, char* argv[])
{
    parse_args(argc, argv);

    serialport stty(args.stty);
    if (!is_stty(stty)) return 1;

    udpserver udp;
    if (!is_udpserver(udp)) return 1;

    serialport::restore_tio restore_stty_options(stty);
    if (!init_stty(stty)) return 1;
    if (!init_udpserver(udp)) return 1;

    if (std::signal(SIGINT,  &on_sigint)
            == SIG_ERR) { perror("signal()"); return 1; }
    if (std::signal(SIGTERM, &on_sigint)
            == SIG_ERR) { perror("signal()"); return 1; }

    return process(stty, udp);
}
