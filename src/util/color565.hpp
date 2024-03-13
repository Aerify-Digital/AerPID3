#ifndef _COLOR_FIVE_SIX_FIVE_H_
#define _COLOR_FIVE_SIX_FIVE_H_

#include <stdint.h>

static uint16_t color565(const uint8_t r, const uint8_t g, const uint8_t b);

uint16_t color565(const uint8_t r, const uint8_t g, const uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#endif