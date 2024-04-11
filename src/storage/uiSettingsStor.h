#ifndef _UI_SETTINGS_STOR_H
#define _UI_SETTINGS_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>
#include "common/enum/ThermalUnitsType.h"

class SettingsStorage
{
public:
    SettingsStorage()
    {
        timeTillOn.v = 1000 * 60 * 5;
        starCount.v = 128;
    };

    void init(Flash *flash)
    {
        this->flash = flash;
        this->timeTillOn.v = 1000 * 60 * 5; // 5 minutes
        this->starCount.v = 128;
        this->load();
    }

    void resetFlash();

    void load();
    void save();
    void destroy();

    bool isNeedSave() { return _needSave; }

    bool getKnobInvert() { return this->knobInvert; };
    void setKnobInvert(bool b)
    {
        this->knobInvert = b;
        this->_needSave = true;
    };

    bool getKnobShowUI() { return this->knobShowUI; };
    void setKnobShowUI(bool b)
    {
        this->knobShowUI = b;
        this->_needSave = true;
    };

    ThermalUnitsType getThermalUnit() { return this->thermal_unit; };
    void setThermalUnit(ThermalUnitsType tu, bool save)
    {
        this->thermal_unit = tu;
        this->_needSave = save;
    };

    bool getScreenSaverEnb() { return this->screenSaverEnabled; };
    void setScreenSaverEnb(bool b)
    {
        this->screenSaverEnabled = b;
        this->_needSave = true;
    };

    uint32_t getTimeTillScrnSaverOn() { return this->timeTillOn.v; };
    void setTimeTillScrnSaverOn(uint32_t timeTillOn)
    {
        this->timeTillOn.v = timeTillOn * 1000;
        this->_needSave = true;
    };

    uint16_t getScrnSaverStarCount() { return this->starCount.v; };
    void setScrnSaverStarCount(uint16_t starCount)
    {
        this->starCount.v = starCount;
        this->_needSave = true;
    };

    uint16_t getDispTypeOptions() { return this->disp_type_options; };
    void setDispTypeOptions(uint16_t opt)
    {
        this->disp_type_options = opt;
        this->_needSave = true;
    };

protected:
private:
    Flash *flash;

    const char *filename = "settings.dat";

    union byte_uint16_t
    {
        uint16_t v;
        uint8_t bytes[sizeof(uint16_t)];
    };

    union byte_uint32_t
    {
        uint32_t v;
        uint8_t bytes[sizeof(uint32_t)];
    };

    const uint _storLength = 1 +
                             sizeof(bool) +
                             sizeof(bool) +
                             sizeof(uint8_t) +
                             sizeof(bool) +
                             sizeof(uint32_t) +
                             sizeof(uint16_t) +
                             sizeof(uint8_t);

    bool _needSave = false;

    void saveSettingsFile(char *storageData);
    bool openSettingsFile(char *storageData);

    bool knobInvert = false;
    bool knobShowUI = true;
    ThermalUnitsType thermal_unit = ThermalUnitsType::CELSIUS;

    bool screenSaverEnabled = true;
    byte_uint32_t timeTillOn;
    byte_uint16_t starCount;

    uint8_t disp_type_options = 0;
};

extern SettingsStorage settingsStorage;

#endif