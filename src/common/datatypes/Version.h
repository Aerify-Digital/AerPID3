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

protected:
    char vDot = '.';

private:
    String version;
    uint major;
    uint minor;
    uint build;
};

#endif