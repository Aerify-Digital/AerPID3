#ifndef _NETWORK_STOR_H
#define _NETWORK_STOR_H

#include "flash.h"

class NetworkingStorage
{
public:
    NetworkingStorage(){};

    void init(Flash *flash);

    bool needSave() { return !_saved; }

    void load();
    void save();
    void destroy();

    void setHostname(const char *hostname);
    char *getHostname();
    bool isHostnameSetup();

private:
    Flash *flash;

    bool _saved = true;

    void saveNetFile(char *storageData);
    bool openNetFile(char *storageData);

    char hostname[64];
    bool hostnameSetup = false;
};

extern NetworkingStorage networkingStorage;

#endif