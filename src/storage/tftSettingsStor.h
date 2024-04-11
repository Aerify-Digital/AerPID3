#ifndef _TFT_SETTINGS_STOR_H
#define _TFT_SETTINGS_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>
#include "common/enum/ThermalUnitsType.h"

class TftSettingsStorage
{
public:
    TftSettingsStorage() { backlight_dim_timeout.v = 900; };

    void init(Flash *flash)
    {
        this->flash = flash;
    }

    void load();
    void save();
    void destroy();

    bool isNeedSave() { return _needSave; }
    bool isLoaded() { return _isLoaded; }

    bool getBacklightDim() { return this->backlight_dim; };
    void setBacklightDim(bool b)
    {
        this->backlight_dim = b;
        this->_needSave = true;
    };

    uint16_t getBacklightLevel() { return this->backlight_level; };
    void setBacklightLevel(uint16_t backlight_level)
    {
        this->backlight_level = backlight_level;
        this->_needSave = true;
    };

    uint16_t getBacklightLevelMin() { return this->backlight_level_min; };
    void setBacklightLevelMin(uint16_t backlight_level)
    {
        this->backlight_level_min = backlight_level;
        this->_needSave = true;
    };

    uint32_t getBacklightDimTimeout() { return this->backlight_dim_timeout.v; };
    void setBacklightDimTimeout(uint32_t backlight_dim_timeout)
    {
        this->backlight_dim_timeout.v = backlight_dim_timeout;
        this->_needSave = true;
    };

protected:
private:
    Flash *flash;

    const char *filename = "tft_dim.dat";

    union byte_uint16_t
    {
        uint16_t v;
        uint8_t bytes[sizeof(uint16_t)];
    };

    const uint _storLength = sizeof(bool) +
                             sizeof(uint8_t) +
                             sizeof(uint8_t) +
                             sizeof(uint16_t) +
                             1;

    bool _needSave = false;
    bool _isLoaded = false;

    void saveSettingsFile(char *storageData);
    bool openSettingsFile(char *storageData);

    bool backlight_dim;
    byte backlight_level;
    byte backlight_level_min;
    byte_uint16_t backlight_dim_timeout;
};

extern TftSettingsStorage tftSettingsStorage;

#endif