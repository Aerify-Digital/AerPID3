#ifndef _WEB_AUTH_STOR_H
#define _WEB_AUTH_STOR_H

#include "flash.h"

#include <stdio.h>
#include <stdint.h>

#include "common/sha256/sha256.h"

class WebAuthStorage
{
public:
    void init(Flash *flash);

    bool needSave() { return !_saved; }

    void loadWebAuth();
    void loadWebAuth(bool &enabled, char *user, char *pass, char *hash);
    void saveWebAuth();
    void saveWebAuth(const bool enabled, const char *user, const char *pass, const char *hash);
    bool webAuthExists();

    void destroy();

    bool isEnabled() { return enabled; }
    void setEnabled(const bool val);
    char *getUser() { return user; }
    void setUser(const char *user);
    char *getPass() { return pass; }
    void setPass(const char *pass);
    char *getHash() { return hash; }
    void setHash(const char *hash);

    void hashPass();

private:
    Flash *flash;

    bool _saved = false;

    bool enabled = false;
    char user[32];
    char pass[32];
    char hash[64];

    const int fileLength = 1 + 2 + 32 + 32 + 64 + 1; // file byte length
    const char *filename = "webauth1.dat";
};

extern WebAuthStorage webAuthStorage;

#endif