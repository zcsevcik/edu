/*
 * packet.h
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#ifndef PACKET_H_
#define PACKET_H_

#include <cstddef>

/* ==================================================================== */
constexpr char STX = '\002';
constexpr char ETX = '\003';

/* ==================================================================== */
struct packet {
    size_t length;
    unsigned char station_no;
    unsigned char command_no;
    unsigned char checksum;
    unsigned char computed_checksum;
};

/* ==================================================================== */
int packet_parse(char const *s, size_t len, packet *st);

/* ==================================================================== */

#endif // PACKET_H_
