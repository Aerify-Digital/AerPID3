#ifndef _MEASURE_MODE_STOR_H
#define _MEASURE_MODE_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>

class MeasureModeStorage
{
public:
    MeasureModeStorage() { mode = 0; };

    void init(Flash *flash)
    {
        this->flash = flash;
    }

    void load();
    void save();
    void destroy();

    bool isNeedSave() { return _needSave; }
    bool isLoaded() { return _isLoaded; }

    void setMode(uint8_t mode);
    uint8_t getMode();

protected:
private:
    Flash *flash;

    const char *filename = "measure_mode.dat";

    union byte_uint16_t
    {
        uint16_t v;
        uint8_t bytes[sizeof(uint16_t)];
    };

    const uint _storLength = sizeof(uint8_t) +
                             sizeof(uint8_t) +
                             1;

    bool _needSave = false;
    bool _isLoaded = false;

    void saveFile(char *storageData);
    bool openFile(char *storageData);

    uint8_t mode;
};

extern MeasureModeStorage measModeStorage;

#endif