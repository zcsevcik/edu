/*
* master_com.c:   Inteligentni teplotni cidlo s mcu PIC,
*                 kompatibilni s protokolem tqs3 Modbus RTU
*                 IMP projekt: Vlastni tema
*
* Date:           Wed, 12 Dec 2012 23:46:02 +0100
* Author(s):      Radek Sevcik <xsevci44@stud.fit.vutbr.cz>
*
* This file is part of mytqs.
*
* Original.
*/

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <modbus.h>

int main(int argc, char* argv[])
{
    char *dev = NULL;
    char slave = '1';

    if (argc >= 2)
        dev = argv[1];
    else
        dev = "\\\\.\\COM1";

    if (argc >= 3)
        slave = *argv[2];

    modbus_t* ctx = modbus_new_rtu(dev, 9600, 'N', 8, 1);

    if (ctx == NULL) {
        fprintf(stderr, "Unable to create the libmodbus context: %s\n",
            modbus_strerror(errno));
        return EXIT_FAILURE;
    }

#ifndef NDEBUG
    modbus_set_debug(ctx, true);
#endif

    modbus_set_slave(ctx, slave);

    struct timeval timeout_byte;
    timeout_byte.tv_sec = 1;
    timeout_byte.tv_usec = 5000;

    struct timeval timeout_response;
    timeout_response.tv_sec = 3;
    timeout_response.tv_usec = 5000;

    modbus_set_byte_timeout(ctx, &timeout_byte);
    modbus_set_response_timeout(ctx, &timeout_response);

#ifdef __linux__
    modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
#endif
    
    if (-1 != modbus_connect(ctx)) {
        uint16_t cfgen, id, baud;
        int16_t cached, imm;

#if 0
        if (-1 != modbus_read_registers(ctx, 0, 1, &cfgen))
            fprintf(stdout, "cfg enabled: %hu\n", cfgen);
#endif

        if (-1 != modbus_read_registers(ctx, 1, 1, &id))
            fprintf(stdout, "device id: %X\n", id);

        if (-1 != modbus_read_registers(ctx, 2, 1, &baud))
            fprintf(stdout, "baud rate: %hu\n", baud);

        if (-1 != modbus_read_input_registers(ctx, 0, 1, (uint16_t*)&cached))
            fprintf(stdout, "cached temperature: %.1f ""\xF8""C\n", (float)cached / 10.0f);

#if 0
        if (-1 != modbus_read_input_registers(ctx, 1, 1, (uint16_t*)&imm))
            fprintf(stdout, "immediate temperature: %.1f ""\xF8""C\n", (float)imm / 10.0f);
#endif

        // id zarizeni
        if (-1 != modbus_write_register(ctx, 0, 0xFF)) {
            if (-1 != modbus_write_register(ctx, 1, 0x31))
                fprintf(stdout, "device id set: %X\n", 0x31);
        }

        // adresa
        if (-1 != modbus_write_register(ctx, 0, 0xFF)) {
            if (-1 != modbus_write_register(ctx, 2, 0x06))
                fprintf(stdout, "baud rate set: 9600\n");
        }

        modbus_close(ctx);
    }

    modbus_free(ctx);
    return EXIT_SUCCESS;
}

