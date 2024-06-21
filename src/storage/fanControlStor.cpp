#include "fanControlStor.h"

void FanControlStorage::saveFile(char *storageData)
{
    flash->saveFile(filename, storageData, _storLength);
}

bool FanControlStorage::openFile(char *storageData)
{
    return flash->openFile(filename, storageData, _storLength);
}

void FanControlStorage::load()
{
    char data[_storLength];
    Serial.println("[FanControlStorage]>Setup: Loading data from Flash.");
    if (!openFile((char *)data))
    {
        Serial.println("[FanControlStorage]>Error: File not found!");
        return;
    }
    // Serial.println((char *)data);

    fanEnabled = data[0];
    fanSpeed = data[1];
    fanMode = data[2];
}

void FanControlStorage::save()
{
    char data[_storLength];

    data[0] = fanEnabled;
    data[1] = fanSpeed;
    data[2] = fanMode;
    data[3] = 0;

    saveFile(data);

    this->_needSave = false;
}

void FanControlStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

FanControlStorage fanControlStorage;
