#include "measureStor.h"

void MeasureModeStorage::saveFile(char *storageData)
{
    flash->saveFile(filename, storageData, _storLength);
}

bool MeasureModeStorage::openFile(char *storageData)
{
    return flash->openFile(filename, storageData, _storLength);
}

void MeasureModeStorage::load()
{
    char data[_storLength];
    Serial.println("[MeasureModeStorage] > Setup: Loading data from Flash.");
    if (!openFile((char *)data))
    {
        Serial.println("[MeasureModeStorage] > Error: File not found!");
        return;
    }

    mode = data[0];

    _isLoaded = true;
}

void MeasureModeStorage::save()
{
    char data[_storLength];

    data[0] = mode;
    data[1] = 0;
    data[2] = 0;

    saveFile(data);

    this->_needSave = false;
}

void MeasureModeStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

void MeasureModeStorage::setMode(uint8_t mode)
{
    MeasureModeStorage::mode = mode;
    MeasureModeStorage::_needSave = true;
}

uint8_t MeasureModeStorage::getMode()
{
    return MeasureModeStorage::mode;
}

MeasureModeStorage measModeStorage;