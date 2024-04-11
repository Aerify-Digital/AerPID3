#ifndef CRYPT_H
#define CRYPT_H

#include "common/base64/base64.h"
#include "common/aes/AES.h"
#include <string.h>
#include "auth.h"
#include "TrueRandom.h"

class Crypto
{
public:
  Crypto()
  {
    string_to_array(defaultKey, 16, key);
  };

  void init(Flash *flash);

  byte encrypt(char msg[], char *cipherTxt);
  byte encrypt(char msg[], char *cipherTxt, byte *key);
  byte decrypt(char cipherTxt[], char *msg);
  byte decrypt(char cipherTxt[], char *msg, byte *key);

  byte encrypt_long(char msg[], char *cipherTxt);
  byte encrypt_long(char msg[], char *cipherTxt, byte *key);
  byte decrypt_long(char cipherTxt[], char *msg);
  byte decrypt_long(char cipherTxt[], char *msg, byte *key);

  void wait(unsigned long milliseconds);

  void setKey(byte *key);
  void loadKey();
  byte *getKey();
  void saveKey();

  void array_to_string(const byte array[], unsigned int len, char buffer[]);
  void string_to_array(const char buffer[], unsigned int len, byte array[]);

private:
  Flash *flash;

  bool loadCrypto(char *pass);
  void saveCrypto(char *pass);

  AES aes;

  int cipherLength = 128;
  char cipher[128];
  int cipherLength_l = 192;
  char cipher_l[192];

  // Key
  byte key[16];

  const char *defaultKey = "0000d34db33ffff";

  unsigned long long int my_iv = 36753562;
};

// Cryptoo AES
extern Crypto crypto;

#endif
