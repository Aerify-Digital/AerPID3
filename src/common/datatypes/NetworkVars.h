#ifndef _APP_NETWORK_VARS_H_
#define _APP_NETWORK_VARS_H_

#include <stdint.h>
#include <IPAddress.h>
#include <string.h>
#include <vector>

class NetworkVars
{
public:
    NetworkVars()
    {
        ip = IPAddress(0, 0, 0, 0);
        hostname = (char *)"AERPID";
        ssid = (char *)"AERPID";
    }

    IPAddress ip;
    char *hostname;
    char *ssid;
    uint8_t mac[6];

    bool doscan = false;
    bool scanned = false;
    bool scanning = false;

    std::vector<String> scanResults = {};

    void setIP(IPAddress ip)
    {
        addr[0] = ip[0];
        addr[1] = ip[1];
        addr[2] = ip[2];
        addr[3] = ip[3];
    }
    uint8_t *getIP()
    {
        return addr;
    }

    void setMAC(uint8_t *addr)
    {
        this->mac[0] = addr[0];
        this->mac[1] = addr[1];
        this->mac[2] = addr[2];
        this->mac[3] = addr[3];
        this->mac[4] = addr[4];
        this->mac[5] = addr[5];
    }

protected:
    char vDot = '.';

private:
    uint8_t addr[4];
};

extern NetworkVars networkVars;

#endif