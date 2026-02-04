#ifndef _APP_VERSION_H_
#define _APP_VERSION_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "WString.h"

struct xVersion
{
    String version;
    uint major;
    uint minor;
    uint build;
};

class AppVersion
{
public:
    AppVersion(uint major, uint minor, uint build)
    {
        this->major = major;
        this->minor = minor;
        this->build = build;
        this->version = String(major) + vDot + String(minor) + vDot + String(build);
    }

    String get()
    {
        return version;
    }

    xVersion getVer()
    {
        xVersion v;
        v.version = this->version;
        v.major = this->major;
        v.minor = this->minor;
        v.build = this->build;
        return v;
    }

    uint *getVerArr()
    {
        uint *ver = new uint[3];
        ver[0] = this->major;
        ver[1] = this->minor;
        ver[2] = this->build;
        return ver;
    }

protected:
    char vDot = '.';

private:
    String version;
    uint major;
    uint minor;
    uint build;
};

#endif