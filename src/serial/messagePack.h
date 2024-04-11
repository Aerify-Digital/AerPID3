/* =============================================================================
 * Copyright (c) 2015-2024 AerifyDigital LLC.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#ifndef _MESSAGE_PACK_H
#define _MESSAGE_PACK_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <vector>
#include "IPAddress.h"

#include <algorithm>
#include <iterator>
#include <numeric>

class MessagePack
{
public:
    MessagePack(uint8_t ver_major, uint8_t ver_minor, uint8_t ver_build, uint8_t nver_major, uint8_t nver_minor, uint8_t nver_build)
    {
        this->xVER_MAJOR = ver_major;
        this->xVER_BUILD = ver_build;
        this->xVER_MINOR = ver_minor;
        this->NET_VER_MAJOR = nver_major;
        this->NET_VER_MINOR = nver_minor;
        this->NET_VER_BUILD = nver_build;
    };
    ~MessagePack() { dispose(); }

    /* Fetches and builds the init message into a buffer.  Returns size of packed. */
    uint fetchInitMessage();

    uint8_t *getPack() { return _packed; }
    uint getPackSize() { return _packedSize; }
    bool isPackComplete();
    void dispose();

    void setIP(IPAddress ip);
    void setSSID(const char *ssid);
    void setHostname(const char *name);
    void setUpTime(long time);
    void setBumpTemp(double temp);
    void setBumpTime(uint16_t time);
    void setAutoOffTime(uint16_t time);
    void setLedMode(uint8_t mode);
    void setLedColor(uint8_t r, uint8_t g, uint8_t b);
    void setLedBrightness(uint8_t val);
    void setFavName(uint8_t fav, char *name);
    void setFavTemp(uint8_t fav, double temp);
    void setTempAdjAmt(double temp);
    void setTemp(uint16_t temp);
    void setSetTemp(uint16_t temp);
    void setAvgTemp(uint16_t temp);
    void setPID(double p, double i, double d);
    void setUnitType(uint8_t t);
    void setBoleanBitmap(uint8_t byt);

protected:
private:
    void pack(const uint8_t in);
    void pack(const uint8_t *in, const uint size);
    void packComplete();

    bool _packComplete = false;
    bool _packing = false;
    uint _packSize = 0;
    uint _packedSize = 0;
    uint8_t *_packed = (uint8_t *)malloc(_packSize);

    // firmware version
    uint8_t xVER_MAJOR;
    uint8_t xVER_MINOR;
    uint8_t xVER_BUILD;
    // ui version
    uint8_t NET_VER_MAJOR;
    uint8_t NET_VER_MINOR;
    uint8_t NET_VER_BUILD;

    uint8_t IP_ADDRESS[4];      // 4 bytes
    uint8_t SSID[32];           // 32 bytes
    uint8_t HOSTNAME[253];      // 253 bytes
    uint8_t UPTIME[4];          // 4 bytes
    uint8_t BUMP_TEMP[2];       // 2 bytes
    uint8_t BUMP_TIME[2];       // 2 bytes
    uint8_t AUTO_OFF_TIME[2];   // 2 bytes
    uint8_t LED_MODE[1];        // 1 byte
    uint8_t LED_COLOR[3];       // 3 byte
    uint8_t LED_BRIGHTNESS[1];  // 1 byte
    uint8_t FAV1_NAME[64];      // 64 bytes
    uint8_t FAV1_TEMP[2];       // 2 bytes
    uint8_t FAV2_NAME[64];      // 64 bytes
    uint8_t FAV2_TEMP[2];       // 2 bytes
    uint8_t FAV3_NAME[64];      // 64 bytes
    uint8_t FAV3_TEMP[2];       // 2 bytes
    uint8_t FAV4_NAME[64];      // 64 bytes
    uint8_t FAV4_TEMP[2];       // 2 bytes
    uint8_t TEMP_ADJ_AMOUNT[2]; // 2 bytes
    uint8_t TEMP[2];            // 2 bytes
    uint8_t SET_TEMP[2];        // 2 bytes
    uint8_t AVG_TEMP[2];        // 2 bytes
    uint8_t PID_P[8];           // 8 bytes
    uint8_t PID_I[8];           // 8 bytes
    uint8_t PID_D[8];           // 8 bytes
    uint8_t UNIT_TYPE[1];       // 1 byte
    uint8_t BOOLEAN_BITMAP[1];  // 1 byte
};

// extern MessagePack messagePack;

#endif
