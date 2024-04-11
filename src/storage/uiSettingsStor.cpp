#include "uiSettingsStor.h"

void SettingsStorage::saveSettingsFile(char *storageData)
{
    flash->saveFile(filename, storageData, _storLength);
}

bool SettingsStorage::openSettingsFile(char *storageData)
{
    return flash->openFile(filename, storageData, _storLength);
}

void SettingsStorage::resetFlash()
{
    Serial.println(F("Flash Erease All!"));
    flash->eraseAll();
}

void SettingsStorage::load()
{
    char data[_storLength];
    Serial.println("[SettingsStorage]>Setup: Loading data from Flash.");
    if (!openSettingsFile((char *)data))
    {
        Serial.println("[SettingsStorage]>Error: File not found!");
        return;
    }
    // Serial.println((char *)data);

    knobInvert = data[0];
    knobShowUI = data[1];
    thermal_unit = (ThermalUnitsType)data[2];
    screenSaverEnabled = data[3];
    timeTillOn.bytes[0] = data[4];
    timeTillOn.bytes[1] = data[5];
    timeTillOn.bytes[2] = data[6];
    timeTillOn.bytes[3] = data[7];
    starCount.bytes[0] = data[8];
    starCount.bytes[1] = data[9];
    disp_type_options = data[10];
}

void SettingsStorage::save()
{
    char data[_storLength];

    data[0] = knobInvert;
    data[1] = knobShowUI;
    data[2] = (uint8_t)thermal_unit;
    data[3] = screenSaverEnabled;
    data[4] = timeTillOn.bytes[0];
    data[5] = timeTillOn.bytes[1];
    data[6] = timeTillOn.bytes[2];
    data[7] = timeTillOn.bytes[3];
    data[8] = starCount.bytes[0];
    data[9] = starCount.bytes[1];
    data[10] = disp_type_options;
    data[11] = 0;

    saveSettingsFile(data);

    this->_needSave = false;
}

void SettingsStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

SettingsStorage settingsStorage;