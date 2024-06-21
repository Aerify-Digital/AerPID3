#ifndef _FAN_CONTROL_STOR_H
#define _FAN_CONTROL_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>

class FanControlStorage
{
public:
    FanControlStorage(){};

    void init(Flash *flash)
    {
        this->flash = flash;
        this->load();
    }

    void load();
    void save();
    void destroy();

    bool isNeedSave() { return _needSave; }

    bool getFanEnabled() { return this->fanEnabled; };
    void setFanEnabled(bool b)
    {
        this->fanEnabled = b;
        this->_needSave = true;
    };

    uint8_t getFanSpeed() { return this->fanSpeed; };
    void setFanSpeed(uint8_t speed)
    {
        this->fanSpeed = speed;
        this->_needSave = true;
    };

    uint8_t getFanMode() { return this->fanMode; };
    void setFanMode(uint8_t mode)
    {
        this->fanMode = mode;
        this->_needSave = true;
    };

protected:
private:
    Flash *flash;

    const char *filename = "fan.dat";

    const uint _storLength = 1 +
                             sizeof(bool) +
                             sizeof(uint8_t) +
                             sizeof(uint8_t);

    bool _needSave = false;

    void saveFile(char *storageData);
    bool openFile(char *storageData);

    bool fanEnabled = true;
    uint8_t fanSpeed = 128;
    uint8_t fanMode = 0;
};

extern FanControlStorage fanControlStorage;

#endif