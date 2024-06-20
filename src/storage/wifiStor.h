#ifndef _NETWORK_WIFI_STOR_H
#define _NETWORK_WIFI_STOR_H

#include "flash.h"
#include "util/crc.h"

class WiFiStorage
{
public:
    WiFiStorage(){};

    void init(Flash *flash);
    void innitDefaults();

    bool needSave() { return !_saved; }

    void load();
    void save();
    void destroy();

    void setUsingStaticIP(bool usingStatic)
    {
        this->usingStaticIP = usingStatic;
        this->_saved = false;
    }
    bool isUsingStaticIP() { return this->usingStaticIP; };

    IPAddress *getLocalIP() { return new IPAddress(this->ip[0], this->ip[1], this->ip[2], this->ip[3]); };
    void setLocalIP(const IPAddress ip)
    {
        this->ip[0] = ip[0];
        this->ip[1] = ip[1];
        this->ip[2] = ip[2];
        this->ip[3] = ip[3];
        _saved = false;
    };

    IPAddress *getGatewayIP() { return new IPAddress(this->gateway[0], this->gateway[1], this->gateway[2], this->gateway[3]); };
    void setGatewayIP(const IPAddress ip)
    {
        this->gateway[0] = ip[0];
        this->gateway[1] = ip[1];
        this->gateway[2] = ip[2];
        this->gateway[3] = ip[3];
        _saved = false;
    };

    IPAddress *getNetMask() { return new IPAddress(this->netmask[0], this->netmask[1], this->netmask[2], this->netmask[3]); };
    void setNetMask(const IPAddress ip)
    {
        this->netmask[0] = ip[0];
        this->netmask[1] = ip[1];
        this->netmask[2] = ip[2];
        this->netmask[3] = ip[3];
        _saved = false;
    };

    IPAddress *getDNS1() { return new IPAddress(this->dns1[0], this->dns1[1], this->dns1[2], this->dns1[3]); };
    void setDNS1(const IPAddress ip)
    {
        this->dns1[0] = ip[0];
        this->dns1[1] = ip[1];
        this->dns1[2] = ip[2];
        this->dns1[3] = ip[3];
        _saved = false;
    };

    IPAddress *getDNS2() { return new IPAddress(this->dns2[0], this->dns2[1], this->dns2[2], this->dns2[3]); };
    void setDNS2(const IPAddress ip)
    {
        this->dns2[0] = ip[0];
        this->dns2[1] = ip[1];
        this->dns2[2] = ip[2];
        this->dns2[3] = ip[3];
        _saved = false;
    };

private:
    Flash *flash;

    const int fileLength = 25;
    const char *filename = "net_static.dat";

    bool _saved = true;
    bool _loaded = false;

    void saveNetFile(char *storageData);
    bool openNetFile(char *storageData);

    bool usingStaticIP = false;
    uint8_t ip[4];
    uint8_t gateway[4];
    uint8_t netmask[4];
    uint8_t dns1[4];
    uint8_t dns2[4];
};

extern WiFiStorage wifiStorage;

#endif