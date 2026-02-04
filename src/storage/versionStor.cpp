#include "versionStor.h"

void VersionStorage::init(Flash *flash, AppVersion version)
{
    VersionStorage::flash = flash;

    if (!exists())
    {
        save(version.getVerArr(), version.getVerArr());
        Serial.println("> Saved Default App Version!");
    }
    load();

    xVersion nVer = version.getVer();
    if (currentVersion[0] != nVer.major || currentVersion[1] != nVer.minor || currentVersion[2] != nVer.build)
    {
        Serial.print("> App Version Change Detected!  New Version: ");
        Serial.println(version.get());
        priorVersion[0] = currentVersion[0];
        priorVersion[1] = currentVersion[1];
        priorVersion[2] = currentVersion[2];
        currentVersion[0] = nVer.major;
        currentVersion[1] = nVer.minor;
        currentVersion[2] = nVer.build;
        save(currentVersion, priorVersion);
        _versionChanged = true;
    }
}

// ==========================================================================
// ==========================================================================
// Web Auth File

void VersionStorage::save()
{
    save(currentVersion, priorVersion);
}

void VersionStorage::save(const uint *current, const uint *previous)
{
    char storageData[fileLength];

    uint k = 0;

    for (int j = 0; j < 3; j++)
    {
        UIntByte curr;
        curr.num = current[j];
        for (int i = 0; i < sizeof(uint); i++)
        {
            storageData[k++] = curr.array[i];
        }
    }
    for (int j = 0; j < 3; j++)
    {
        UIntByte prev;
        prev.num = current[j];
        for (int i = 0; i < sizeof(uint); i++)
        {
            storageData[k++] = prev.array[i];
        }
    }

    storageData[k++] = '\0';

    flash->saveFile(filename, storageData, fileLength);

    this->_saved = true;
}

void VersionStorage::load()
{
    load(currentVersion, priorVersion);
}

void VersionStorage::load(uint *current, uint *previous)
{
    char storageData[fileLength];

    flash->openFile(filename, storageData, fileLength);

    uint k = 0;

    for (int j = 0; j < 3; j++)
    {
        UIntByte curr;
        for (int i = 0; i < sizeof(uint); i++)
        {
            curr.array[i] = storageData[k++];
        }
        current[j] = curr.num;
    }
    for (int j = 0; j < 3; j++)
    {
        UIntByte prev;
        for (int i = 0; i < sizeof(uint); i++)
        {
            prev.array[i] = storageData[k++];
        }
        current[j] = prev.num;
    }
}

bool VersionStorage::exists()
{
    return flash->fileExists(filename);
}

void VersionStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

uint *VersionStorage::getCurrent()
{
    return currentVersion;
}

void VersionStorage::setCurrent(const uint *curr)
{
    *currentVersion = *curr;
}

uint *VersionStorage::getPrevious()
{
    return priorVersion;
}

void VersionStorage::setPrevious(const uint *prev)
{
    *priorVersion = *prev;
}

VersionStorage versionStorage;