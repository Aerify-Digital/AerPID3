#include "networkStor.h"

void NetworkingStorage::init(Flash *flash)
{
    NetworkingStorage::flash = flash;

    load();
}

void NetworkingStorage::saveNetFile(char *storageData)
{
    const int leng = 256;
    const char *filename = "network.dat";
    flash->saveFile(filename, storageData, leng);
}

bool NetworkingStorage::openNetFile(char *storageData)
{
    const int leng = 256;
    const char *filename = "network.dat";
    return flash->openFile(filename, storageData, leng);
}

void NetworkingStorage::load()
{
    char data[256];
    Serial.println("[NetworkStor]>Setup: Loading data from Flash.");
    if (!openNetFile((char *)data))
    {
        Serial.println("[NetworkStor]>Error: File not found!");
        return;
    }

    hostnameSetup = data[66] == 1;

    uint j = 0;
    char host[65];
    for (int i = 0; i < 256; i++)
    {
        if (data[i] == 0xff || data[i] == '\0')
        {
            break;
        }
        if (j < 64)
        {
            host[j] = data[i];
            this->hostname[j] = data[i];
            j++;
        }
    }
    host[j] = '\0';
    this->hostname[j] = '\0';
}

void NetworkingStorage::save()
{
    char data[256];

    uint j = 0;
    for (int i = 0; i < 256; i++)
    {
        if (j < strlen(this->hostname))
        {
            data[i] = this->hostname[j++];
        }
        else
        {
            data[i] = 0xff;
        }
    }
    data[66] = hostnameSetup;

    saveNetFile(data);
    _saved = true;
}

void NetworkingStorage::destroy()
{
    const char *filename = "network.dat";
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

void NetworkingStorage::setHostname(const char *h)
{
    for (int i = 0; i < 64; i++)
    {
        if (i < strlen(h))
        {
            this->hostname[i] = h[i];
        }
        else
        {
            this->hostname[i] = '\0';
        }
    }
    this->hostnameSetup = strlen(h) > 1;
    this->_saved = false;
}

char *NetworkingStorage::getHostname()
{
    return this->hostname;
}

bool NetworkingStorage::isHostnameSetup()
{
    return this->hostnameSetup;
}

NetworkingStorage networkingStorage;
