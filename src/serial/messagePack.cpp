#include "messagePack.h"

void MessagePack::packComplete()
{
    _packing = false;
    _packComplete = true;
}

bool MessagePack::isPackComplete()
{
    return _packComplete;
}

void MessagePack::dispose()
{
    // Free the buffer memory
    free(_packed);
    _packSize = 0;
    _packed = (uint8_t *)malloc(_packSize);
}

void MessagePack::pack(const uint8_t *in, const uint size)
{
    if (!_packing)
    {
        // Start a new pack
        _packSize = 0;
        _packedSize = 0;
        _packed = (uint8_t *)realloc(_packed, _packSize);
        _packing = true;
    }
    // If buffer is full, allocate more memory
    if (_packedSize == _packSize)
    {
        _packSize += size;
        _packed = (uint8_t *)realloc(_packed, _packSize);
    }
    for (int i = 0; i < size; i++)
    {
        _packed[_packedSize] = in[i];
        _packedSize++;
    }
}

void MessagePack::pack(const uint8_t in)
{
    if (!_packing)
    {
        // Start a new pack
        _packSize = 0;
        _packedSize = 0;
        _packed = (uint8_t *)realloc(_packed, _packSize);
        _packing = true;
    }
    // If buffer is full, allocate more memory
    if (_packedSize == _packSize)
    {
        _packSize += 1;
        _packed = (uint8_t *)realloc(_packed, _packSize);
    }
    _packed[_packedSize] = in;
    _packedSize++;
}

uint MessagePack::fetchInitMessage()
{
    // init cmd
    pack(0x20);
    // version
    pack(xVER_MAJOR);
    pack(xVER_MINOR);
    pack(xVER_BUILD);
    pack(NET_VER_MAJOR);
    pack(NET_VER_MINOR);
    pack(NET_VER_BUILD);

    // network data block
    pack(IP_ADDRESS, 4);
    pack(SSID, 32);
    pack(HOSTNAME, 253);
    // times data block
    pack(UPTIME, 4);
    pack(BUMP_TEMP, 2);
    pack(BUMP_TIME, 2);
    pack(AUTO_OFF_TIME, 2);
    // led data block
    pack(LED_MODE, 1);
    pack(LED_COLOR, 3);
    pack(LED_BRIGHTNESS, 1);

    // favs block
    pack(FAV1_NAME, 64);
    pack(FAV1_TEMP, 2);
    pack(FAV2_NAME, 64);
    pack(FAV2_TEMP, 2);
    pack(FAV3_NAME, 64);
    pack(FAV3_TEMP, 2);
    pack(FAV4_NAME, 64);
    pack(FAV4_TEMP, 2);

    // temperature
    pack(TEMP_ADJ_AMOUNT, 2);
    pack(TEMP, 2);
    pack(SET_TEMP, 2);
    pack(AVG_TEMP, 2);
    // pid
    pack(PID_P, 8);
    pack(PID_I, 8);
    pack(PID_D, 8);

    // temperature unit type
    pack(UNIT_TYPE, 1);

    // boolean flags bitmap
    pack(BOOLEAN_BITMAP, 1);

    packComplete();

    return getPackSize();
}

void MessagePack::setIP(IPAddress ip)
{
    IP_ADDRESS[0] = ip[0];
    IP_ADDRESS[1] = ip[1];
    IP_ADDRESS[2] = ip[2];
    IP_ADDRESS[3] = ip[3];
}

void MessagePack::setSSID(const char *ssid)
{
    for (uint i = 0; i < 32; i++)
    {
        SSID[i] = 0;
    }
    for (uint i = 0; i < strlen(ssid); i++)
    {
        if (i >= 32)
        {
            break;
        }
        SSID[i] = ssid[i];
    }
}

void MessagePack::setHostname(const char *name)
{
    for (uint i = 0; i < 253; i++)
    {
        HOSTNAME[i] = 0;
    }
    for (uint i = 0; i < strlen(name); i++)
    {
        if (i >= 253)
        {
            break;
        }
        HOSTNAME[i] = name[i];
    }
}

void MessagePack::setUpTime(long time)
{
    // convert from an unsigned long int to a 4-byte array
    UPTIME[0] = (int)((time >> 24) & 0xFF);
    UPTIME[1] = (int)((time >> 16) & 0xFF);
    UPTIME[2] = (int)((time >> 8) & 0XFF);
    UPTIME[3] = (int)((time & 0XFF));
}

void MessagePack::setBumpTemp(double temp)
{
    uint16_t t = static_cast<uint16_t>(10.0 * temp);
    BUMP_TEMP[0] = (int)((t >> 8) & 0XFF);
    BUMP_TEMP[1] = (int)((t & 0XFF));
}

void MessagePack::setBumpTime(uint16_t time)
{
    BUMP_TIME[0] = (int)((time >> 8) & 0XFF);
    BUMP_TIME[1] = (int)((time & 0XFF));
}

void MessagePack::setAutoOffTime(uint16_t time)
{
    AUTO_OFF_TIME[0] = (int)((time >> 8) & 0XFF);
    AUTO_OFF_TIME[1] = (int)((time & 0XFF));
}

void MessagePack::setLedMode(uint8_t mode)
{
    LED_MODE[0] = mode;
}

void MessagePack::setLedColor(uint8_t r, uint8_t g, uint8_t b)
{
    LED_COLOR[0] = r;
    LED_COLOR[1] = g;
    LED_COLOR[2] = b;
}

void MessagePack::setLedBrightness(uint8_t val)
{
    LED_BRIGHTNESS[0] = val;
}

void MessagePack::setFavName(uint8_t fav, char *name)
{
    switch (fav)
    {
    case 1:
        for (int i = 0; i < 64; i++)
        {
            FAV1_NAME[i] = 0;
        }
        for (uint i = 0; i < strlen(name); i++)
        {
            if (i >= 64)
            {
                break;
            }
            FAV1_NAME[i] = name[i];
        }
        break;
    case 2:
        for (int i = 0; i < 64; i++)
        {
            FAV2_NAME[i] = 0;
        }
        for (uint i = 0; i < strlen(name); i++)
        {
            if (i >= 64)
            {
                break;
            }
            FAV2_NAME[i] = name[i];
        }
        break;
    case 3:
        for (int i = 0; i < 64; i++)
        {
            FAV3_NAME[i] = 0;
        }
        for (uint i = 0; i < strlen(name); i++)
        {
            if (i >= 64)
            {
                break;
            }
            FAV3_NAME[i] = name[i];
        }
        break;
    case 4:
        for (int i = 0; i < 64; i++)
        {
            FAV4_NAME[i] = 0;
        }
        for (uint i = 0; i < strlen(name); i++)
        {
            if (i >= 64)
            {
                break;
            }
            FAV4_NAME[i] = name[i];
        }
        break;

    default:
        break;
    }
}

void MessagePack::setFavTemp(uint8_t fav, double temp)
{
    uint16_t t = static_cast<uint16_t>(10.0 * temp);
    switch (fav)
    {
    case 1:
        FAV1_TEMP[0] = (int)((t >> 8) & 0XFF);
        FAV1_TEMP[1] = (int)((t & 0XFF));
        break;
    case 2:
        FAV2_TEMP[0] = (int)((t >> 8) & 0XFF);
        FAV2_TEMP[1] = (int)((t & 0XFF));
        break;
    case 3:
        FAV3_TEMP[0] = (int)((t >> 8) & 0XFF);
        FAV3_TEMP[1] = (int)((t & 0XFF));
        break;
    case 4:
        FAV4_TEMP[0] = (int)((t >> 8) & 0XFF);
        FAV4_TEMP[1] = (int)((t & 0XFF));
        break;

    default:
        break;
    }
}

void MessagePack::setTempAdjAmt(double temp)
{
    uint16_t t = static_cast<uint16_t>(10.0 * temp);
    TEMP_ADJ_AMOUNT[0] = (t >> 8) & 0xff;
    TEMP_ADJ_AMOUNT[1] = t & 0xff;
}

void MessagePack::setTemp(uint16_t temp)
{
    TEMP[0] = (temp >> 8) & 0xff;
    TEMP[1] = temp & 0xff;
}

void MessagePack::setSetTemp(uint16_t temp)
{
    SET_TEMP[0] = (temp >> 8) & 0xff;
    SET_TEMP[1] = temp & 0xff;
}

void MessagePack::setAvgTemp(uint16_t temp)
{
    AVG_TEMP[0] = (temp >> 8) & 0xff;
    AVG_TEMP[1] = temp & 0xff;
}

void MessagePack::setPID(double kP, double kI, double kD)
{
    union byteDouble
    {
        double value;
        char bytes[sizeof(double)];
    };

    byteDouble xP;
    xP.value = kP;
    byteDouble xI;
    xI.value = kI;
    byteDouble xD;
    xD.value = kD;

    for (uint j = 0; j < 8; j++)
    {
        PID_P[j] = xP.bytes[j];
        PID_I[j] = xI.bytes[j];
        PID_D[j] = xD.bytes[j];
    }
}

void MessagePack::setUnitType(uint8_t t)
{
    UNIT_TYPE[0] = t;
}

void MessagePack::setBoleanBitmap(uint8_t byt)
{
    BOOLEAN_BITMAP[0] = byt;
}