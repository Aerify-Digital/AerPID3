#ifndef _AQUA_I2C_HELPER_H_
#define _AQUA_I2C_HELPER_H_

#include <Wire.h>

/* Quick helper function for checking if device exists */
static bool device_exists(uint8_t addr)
{
    int ret;
    // uint8_t rxdata;
    //  ret = i2c_read_blocking(i2c1, addr, &rxdata, 1, false);
    Wire.beginTransmission(addr);
    ret = Wire.endTransmission();
    return ret == 0;
}

#endif