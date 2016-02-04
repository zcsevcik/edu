/*
 * packet.cc
 *
 * Author(s):      Radek Sevcik <zcsevcik@gmail.com>
 * Date:           2015/08/21
 *
 * This file is part of fatek-relay.
 */

#include "packet.h"

/* ==================================================================== */
inline char get_next(char const*& s, size_t& len) {
    if (len-- > 0)
        return *s++;
    return '\0';
}

/* ==================================================================== */
inline bool is_hexa(char c) {
    return ('0' <= c && c <= '9') || ('A' <= c && c <= 'F');
}

/* ==================================================================== */
inline unsigned char hex_to_uint8(unsigned char high, unsigned char low) {
    unsigned char out = 0;
    out += ( (high >= 'A') ? (high - 'A' + 0xA) : (high - '0') ) << 4;
    out += ( (low  >= 'A') ? (low  - 'A' + 0xA) : (low  - '0') ) & 0x0F;
    return out;
}

/* ==================================================================== */
static bool get_hex(char const*& s, size_t& len,
                    unsigned char* out, unsigned char* crc)
{
    unsigned char c_h, c_l;
    if (!is_hexa( c_h = get_next(s, len) )) return false;
    if (!is_hexa( c_l = get_next(s, len) )) return false;
    *out = hex_to_uint8(c_h, c_l);
    *crc += c_h + c_l;
    return true;
}

/* ==================================================================== */
static bool get_crc(char const* const& s, size_t const&,
                    unsigned char* out, unsigned char* crc)
{
    char const* hex_crc = &s[-3];
    size_t len_crc = 2;
    unsigned char subtract_crc = 0;

    if (!get_hex(hex_crc, len_crc, out, &subtract_crc))
        return false;

    *crc -= subtract_crc;
    return true;
}

/* ==================================================================== */
int packet_parse(char const *s, size_t len, packet *st)
{
    st->computed_checksum = 0;
    st->length = len;

    if (get_next(s, len) != STX)                                   return false;
    st->computed_checksum = STX;
    if (!get_hex(s, len, &st->station_no, &st->computed_checksum)) return false;
    if (!get_hex(s, len, &st->command_no, &st->computed_checksum)) return false;

    for (unsigned char c; (c = get_next(s, len)) != ETX;) {
        if (__builtin_expect((c == '\0'), 0))
            goto unexpected_eof;
        st->computed_checksum += c;
    }

    if (!get_crc(s, len, &st->checksum, &st->computed_checksum))   return false;

    st->length -= len;
    return true;

  unexpected_eof:
    st->length -= len + 1;
    return -1;
}
