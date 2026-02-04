#include "webAuthStor.h"

void WebAuthStorage::init(Flash *flash)
{
    WebAuthStorage::flash = flash;

    if (!webAuthExists())
    {
        saveWebAuth(false, "user", "AerPID3", "ff8d30a8c133627ec3e8cb75e91b61950aa2d8b1b4de499aeb9e8a7a1e20562c");
        Serial.println("> Saved Default Web Auth!");
    }
    loadWebAuth();
}

// ==========================================================================
// ==========================================================================
// Web Auth File

void WebAuthStorage::saveWebAuth(const bool enbabled, const char *user, const char *pass, const char *hash)
{
    char storageData[fileLength];

    uint k = 0;

    storageData[k++] = enbabled ? 1 : 0;

    storageData[k++] = '0';
    storageData[k++] = 'x';

    for (int j = 0; j < 32; j++)
    {
        storageData[k++] = user[j];
    }
    for (int j = 0; j < 32; j++)
    {
        storageData[k++] = pass[j];
    }
    for (int j = 0; j < 64; j++)
    {
        storageData[k++] = hash[j];
    }

    storageData[k++] = '\0';

    flash->saveFile(filename, storageData, fileLength);

    this->_saved = true;
}

void WebAuthStorage::loadWebAuth()
{
    loadWebAuth(enabled, user, pass, hash);
}

void WebAuthStorage::loadWebAuth(bool &enabled, char *user, char *pass, char *hash)
{
    char storageData[fileLength];

    flash->openFile(filename, storageData, fileLength);

    uint k = 0;

    enabled = storageData[k++];
    k += 2;
    for (int j = 0; j < 32; j++)
    {
        user[j] = storageData[k++];
    }
    for (int j = 0; j < 32; j++)
    {
        pass[j] = storageData[k++];
    }
    for (int j = 0; j < 64; j++)
    {
        hash[j] = storageData[k++];
    }
}

void WebAuthStorage::saveWebAuth()
{
    saveWebAuth(enabled, user, pass, hash);
}

bool WebAuthStorage::webAuthExists()
{
    return flash->fileExists(filename);
}

void WebAuthStorage::destroy()
{
    if (flash->fileExists(filename))
    {
        flash->deleteFile(filename);
    }
}

void WebAuthStorage::setEnabled(const bool val)
{
    enabled = val;
    this->_saved = false;
}

void WebAuthStorage::setUser(const char *user)
{
    for (int j = 0; j < 32; j++)
    {
        this->user[j] = user[j];
    }
    this->_saved = false;
}

void WebAuthStorage::setPass(const char *pass)
{
    for (int j = 0; j < 32; j++)
    {
        this->pass[j] = pass[j];
    }
    this->_saved = false;
}

void WebAuthStorage::setHash(const char *hash)
{
    for (int j = 0; j < 64; j++)
    {
        this->hash[j] = hash[j];
    }
    this->_saved = false;
}

void WebAuthStorage::hashPass()
{
    Sha256 sha256 = Sha256();
    sha256.init();
    sha256.write(webAuthStorage.getPass());
    const char *uhash = (const char *)sha256.result();
    sha256.reset();
    setHash(uhash);
}

WebAuthStorage webAuthStorage;