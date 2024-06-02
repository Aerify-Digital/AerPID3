#include "wifiStor.h"

using namespace CRC;

void WiFiStorage::init(Flash *flash)
{
    WiFiStorage::flash = flash;

    load();
    if (!_loaded)
    {
        innitDefaults();
        save();
        load();
    }
}

void WiFiStorage::saveNetFile(char *storageData)
{
    flash->saveFile(filename, storageData, fileLength);
}

bool WiFiStorage::openNetFile(char *storageData)
{
    return flash->openFile(filename, storageData, fileLength);
}

void WiFiStorage::innitDefaults()
{
    // disabled default
    this->usingStaticIP = 0;
    // local ip
    this->ip[0] = 192;
    this->ip[1] = 168;
    this->ip[2] = 0;
    this->ip[3] = 242;
    // gateway
    this->gateway[0] = 192;
    this->gateway[1] = 168;
    this->gateway[2] = 0;
    this->gateway[3] = 1;
    // netmask
    this->netmask[0] = 255;
    this->netmask[1] = 255;
    this->netmask[2] = 255;
    this->netmask[3] = 0;
    // dns 1
    this->dns1[0] = 8;
    this->dns1[1] = 8;
    this->dns1[2] = 8;
    this->dns1[3] = 8;
    // dns 2
    this->dns2[0] = 192;
    this->dns2[1] = 168;
    this->dns2[2] = 0;
    this->dns2[3] = 1;
}

void WiFiStorage::load()
{
    char data[fileLength];
    Serial.println("[WifiStor]>Setup: Loading data from Flash.");
    if (!openNetFile((char *)data))
    {
        Serial.println("[WifiStor]>Error: File not found!");
        return;
    }

    // crc check
    uint8_t r = data[22];

    Serial.print("CRC: ");
    Serial.println(r);
    bool valid = crc_check((uint8_t *)data, r, 21);

    if (!valid)
    {
        Serial.println("[WifiStor]>Error: CRC Invalid on load!");
        // return;
    }

    // enable
    this->usingStaticIP = data[0] != 0 || data[0] == 1;

    // local ip
    this->ip[0] = data[1];
    this->ip[1] = data[2];
    this->ip[2] = data[3];
    this->ip[3] = data[4];
    // gateway
    this->gateway[0] = data[5];
    this->gateway[1] = data[6];
    this->gateway[2] = data[7];
    this->gateway[3] = data[8];
    // netmask
    this->netmask[0] = data[9];
    this->netmask[1] = data[10];
    this->netmask[2] = data[11];
    this->netmask[3] = data[12];
    // dns 1
    this->dns1[0] = data[13];
    this->dns1[1] = data[14];
    this->dns1[2] = data[15];
    this->dns1[3] = data[16];
    // dns 2
    this->dns2[0] = data[17];
    this->dns2[1] = data[18];
    this->dns2[2] = data[19];
    this->dns2[3] = data[20];

    _loaded = true;
}

void WiFiStorage::save()
{
    char data[fileLength];

    // enable
    data[0] = this->usingStaticIP ? 1 : 0;

    // local ip
    data[1] = this->ip[0];
    data[2] = this->ip[1];
    data[3] = this->ip[2];
    data[4] = this->ip[3];
    // gateway
    data[5] = this->gateway[0];
    data[6] = this->gateway[1];
    data[7] = this->gateway[2];
    data[8] = this->gateway[3];
    // netmask
    data[9] = this->netmask[0];
    data[10] = this->netmask[1];
    data[11] = this->netmask[2];
    data[12] = this->netmask[3];
    // dns 1
    data[13] = this->dns1[0];
    data[14] = this->dns1[1];
    data[15] = this->dns1[2];
    data[16] = this->dns1[3];
    // dns 2
    data[17] = this->dns2[0];
    data[18] = this->dns2[1];
    data[19] = this->dns2[2];
    data[20] = this->dns2[3];

    // crc data
    uint8_t r = crc((const uint8_t *)data, 21);

    data[21] = 0x00;
    data[22] = r;
    data[23] = 0x00;
    data[24] = 0x00;

    saveNetFile(data);
    _saved = true;
}

void WiFiStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

WiFiStorage wiFiStorage;
