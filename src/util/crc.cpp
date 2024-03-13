#include "crc.h"

namespace CRC
{
  // crc function
  uint8_t crc(const uint8_t *b, size_t len)
  {
    uint8_t crc = 0;
    while (len--)
    {
      uint8_t inbyte = *b++;
      for (uint8_t i = 8; i; i--)
      {
        uint8_t mix = (crc ^ inbyte) & 0x01;
        crc >>= 1;
        if (mix)
        {
          crc ^= 0x8C;
        }
        inbyte >>= 1;
      }
    }
    return crc;
  }

  /* Data Checksum Helpers */

  uint8_t crc_bytes(const uint8_t *b, int offset)
  {
    size_t len = sizeof(b) / sizeof(*b);
    return crc_bytes(b, offset, len);
  }

  uint8_t crc_bytes(const uint8_t *b, int offset, size_t len)
  {
    size_t l = offset + len;
    uint8_t crc = 0;
    uint i = 0;
    while (i++ < len)
    {
      uint8_t inbyte = *b++;
      if (i < offset)
      {
        continue;
      }
      for (uint8_t i = 8; i; i--)
      {
        uint8_t mix = (crc ^ inbyte) & 0x01;
        crc >>= 1;
        if (mix)
        {
          crc ^= 0x8C;
        }
        inbyte >>= 1;
      }
    }

    /*for (uint8_t i = offset; i < offset + len; i++)
    {
      uint8_t inbyte = b[i];
      for (uint8_t i = 8; i; i--)
      {
        uint8_t mix = (crc ^ inbyte) & 0x01;
        crc >>= 1;
        if (mix)
        {
          crc ^= 0x8C;
        }
        inbyte >>= 1;
      }
    }*/
    return crc;
  }

  /* data crc checks */

  bool crc_check(uint8_t *crc_b, uint8_t check, size_t len)
  {
    return crc_check(crc_bytes(crc_b, len), check);
  }

  bool crc_check(uint8_t crc, uint8_t check)
  {
    return crc == check;
  }

  bool crc_check_cmd(uint8_t addr, uint8_t cmd, uint8_t arg, uint8_t check)
  {
    uint8_t crc_b[3];
    crc_b[0] = addr;
    crc_b[1] = cmd;
    crc_b[2] = arg;
    return crc_check(crc_bytes(crc_b, 3), check);
  }
}