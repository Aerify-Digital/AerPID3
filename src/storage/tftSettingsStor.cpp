#include "tftSettingsStor.h"

void TftSettingsStorage::saveSettingsFile(char *storageData)
{
    flash->saveFile(filename, storageData, _storLength);
}

bool TftSettingsStorage::openSettingsFile(char *storageData)
{
    return flash->openFile(filename, storageData, _storLength);
}

void TftSettingsStorage::load()
{
    char data[_storLength];
    Serial.println("[TftSettingsStorage] > Setup: Loading data from Flash.");
    if (!openSettingsFile((char *)data))
    {
        Serial.println("[TftSettingsStorage] > Error: File not found!");
        return;
    }
    // Serial.println((char *)data);

    backlight_dim = data[0];
    backlight_level = data[1];
    backlight_level_min = data[2];
    backlight_dim_timeout.bytes[0] = data[3];
    backlight_dim_timeout.bytes[1] = data[4];

    _isLoaded = true;
}

void TftSettingsStorage::save()
{
    char data[_storLength];

    data[0] = backlight_dim;
    data[1] = backlight_level;
    data[2] = backlight_level_min;
    data[3] = backlight_dim_timeout.bytes[0];
    data[4] = backlight_dim_timeout.bytes[1];
    data[5] = 0;

    saveSettingsFile(data);

    this->_needSave = false;
}

void TftSettingsStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

TftSettingsStorage tftSettingsStorage;