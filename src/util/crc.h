#ifndef _CRC_H
#define _CRC_H

#include <stdio.h>

/* Data Checksum Helper */
namespace CRC
{
    uint8_t crc(const uint8_t *b, size_t len);
    uint8_t crc_bytes(const uint8_t *b, int offset);
    uint8_t crc_bytes(const uint8_t *b, int offset, size_t len);
    bool crc_check(uint8_t *crc_b, uint8_t check, size_t len);
    bool crc_check(uint8_t crc, uint8_t check);
    bool crc_check_cmd(uint8_t addr, uint8_t cmd, uint8_t arg, uint8_t check);
};

#endif
