#ifndef _APP_VERSION_STOR_H
#define _APP_VERSION_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>

#include "common/sha256/sha256.h"
#include "common/datatypes/Version.h"

class VersionStorage
{
public:
    void init(Flash *flash, AppVersion version);

    bool needSave() { return !_saved; }

    void load();
    void load(uint *current, uint *previous);
    void save();
    void save(const uint *current, const uint *previous);

    bool exists();
    void destroy();

    bool hasVersionChanged() { return _versionChanged; }

    uint *getCurrent();
    void setCurrent(const uint *verArr);
    uint *getPrevious();
    void setPrevious(const uint *verArr);

protected:
    union UIntByte
    {
        byte array[sizeof(uint)];
        uint num;
    };

private:
    Flash *flash;

    bool _saved = false;

    bool _versionChanged = false;

    uint currentVersion[3];
    uint priorVersion[3];

    // file byte length
    const int fileLength = (3 * sizeof(uint)) + (3 * sizeof(uint)) + 1;
    const char *filename = "version.dat";
};

extern VersionStorage versionStorage;

#endif