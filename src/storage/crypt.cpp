#include "crypt.h"

void Crypto::init(Flash *flash)
{
  Crypto::flash = flash;
  loadKey();
  Serial.println("Initialized Crypto!");
}

bool Crypto::loadCrypto(char *pass)
{
  return Crypto::flash->openCryptoFile(pass);
}

void Crypto::saveCrypto(char *pass)
{
  Crypto::flash->saveCryptoFile(pass);
}

void Crypto::setKey(byte *ke)
{
  for (int i = 0; i < 16; i++)
  {
    Crypto::key[i] = ke[i];
  }
}

byte *Crypto::getKey()
{
  return Crypto::key;
}

void Crypto::loadKey()
{
  char pass[64];
  if (Crypto::loadCrypto(pass))
  {
    Crypto::setKey((byte *)pass);
    Serial.println("Crypto Key Loaded Saved..");
  }
  else
  {
    Crypto::setKey((byte *)defaultKey);
    Serial.println("Crypto Key Loaded Defaults");
  }

  if (String((char *)Crypto::key).equals("0000d34db33ffff"))
  {
    Serial.println("Crypto Key is Default -- Regenerating!!!");
    // regen key...
    TrueRandomClass trc = TrueRandomClass();
    for (int i = 0; i < 16; i++)
    {
      Crypto::key[i] = trc.randomByte();
    }
    saveKey();
  }
}

void Crypto::saveKey()
{
  char *pass = (char *)Crypto::getKey();
  Crypto::saveCrypto(pass);
}

// ===============================================================
byte Crypto::encrypt(char msg[], char *cipherText)
{
  return Crypto::encrypt(msg, cipherText, Crypto::key);
}

byte Crypto::encrypt(char msg[], char *cipherText, byte *key)
{
  Crypto::aes.iv_inc();
  byte iv[N_BLOCK];
  int bits = 128;

  Crypto::aes.set_IV(Crypto::my_iv);
  Crypto::aes.get_IV(iv);

  // int ivLen = strlen((char*)iv);
  int inlen = strlen((char *)msg);
  // int ciphLen = 128; // 128
  byte inPad[Crypto::cipherLength + 16];
  // byte cipher[ciphLen];
  int j = 0;

  for (int i = 0; i < Crypto::cipherLength; i++)
  {
    if (i >= Crypto::cipherLength - 16)
    {
      inPad[i] = 0x7C;
    }
    else if (i >= Crypto::cipherLength - inlen - 16 && j < inlen)
    {
      inPad[i] = (char)msg[j++];
    }
    else
    {
      inPad[i] = 0x7C;
    }
  }

  // Serial.print("> InPAD::");
  // Serial.println((char*) inPad);

  Crypto::aes.do_aes_encrypt(inPad, Crypto::cipherLength, (byte *)Crypto::cipher, key, bits, iv);

  // int ciphLen = Crypto::cipherLength;
  // int clen = strlen((char*)Crypto::cipher);
  // int len =  clen - ciphLen;
  // int pad = ciphLen - len;
  // int padLeng = len + pad;
  // int padLeng = clen + len;
  // char cipheriv[padLeng];

  /*if (clen % 16 != 0) {
    int k = 0;
    for (int i = 0; i < padLeng; i++) {
      if (i < clen && k < ciphLen) {
        cipheriv[i] = Crypto::cipher[k++];
      } else {
        cipheriv[i] = 0x00;
      }
    }
    base64_encode(cipherText, (char*) cipheriv, padLeng);
  } else {*/
  base64_encode(cipherText, Crypto::cipher, Crypto::cipherLength);
  //}

  // Serial.print("> Cipher::");
  // Serial.println(cipherText);

  return 0;
}

byte Crypto::decrypt(char cipherText[], char *msg)
{
  return Crypto::decrypt(cipherText, msg, Crypto::key);
}

byte Crypto::decrypt(char cipherText[], char *msg, byte *key)
{
  aes.iv_inc();
  byte iv[N_BLOCK];
  int bits = 128;

  aes.set_IV(Crypto::my_iv);
  aes.get_IV(iv);

  // int ciphLen = 128; // 128
  // byte cipher[ciphLen];
  // int padLen = 48 + ciphLen + N_BLOCK;
  int padLen = Crypto::cipherLength + 16;
  // int padLen = strlen((char*)cipherText);
  char inPad[padLen];

  base64_decode((char *)inPad, (char *)cipherText, strlen((char *)cipherText));

  /*int k = 0;
  for (int i = 0; i < padLen; i++) {
    if (i < Crypto::cipherLength) {
      Crypto::cipher[i] = inPad[i];
    } else {
      Crypto::cipher[i] = '\0';
      break;
    }
  }*/

  byte clearText[Crypto::cipherLength + 16];

  Crypto::aes.do_aes_decrypt((byte *)inPad, Crypto::cipherLength, clearText, key, bits, iv);

  int padding = 0;
  for (int i = 0; i < Crypto::cipherLength; i++)
  {
    if (clearText[i] == 0x7C)
    {
      padding++;
    }
    else
    {
      break;
    }
  }
  int j = 0;
  // int k = padding;
  for (int i = padding; i < Crypto::cipherLength; i++)
  {
    if (clearText[i] == 0x7C)
    {
      break;
    }
    if (j < Crypto::cipherLength)
    {
      msg[j++] = clearText[i];
    }
  }

  msg[j] = '\0';

  return 0;
}

// =======================================================
// =======================================================
// Encrypt LONG
byte Crypto::encrypt_long(char msg[], char *cipherText)
{
  return Crypto::encrypt_long(msg, cipherText, Crypto::key);
}

byte Crypto::encrypt_long(char msg[], char *cipherText, byte *key)
{
  Crypto::aes.iv_inc();
  byte iv[N_BLOCK];
  int bits = 128;

  Crypto::aes.set_IV(Crypto::my_iv);
  Crypto::aes.get_IV(iv);

  // int ivLen = strlen((char*)iv);
  int inlen = strlen((char *)msg);
  // int ciphLen = 128; // 128
  byte inPad[Crypto::cipherLength_l + 64];
  // byte cipher[ciphLen];

  int j = 0;
  for (int i = 0; i < inlen; i++)
  {
    if (i >= Crypto::cipherLength_l - 16)
    {
      inPad[i] = 0x7C;
    }
    else if (i >= Crypto::cipherLength_l - inlen - 16 && j < inlen)
    {
      inPad[i] = (char)msg[j++];
    }
    else
    {
      inPad[i] = 0x7C;
    }
  }

  // Serial.print("> InPAD::");
  // Serial.println((char*) inPad);

  Crypto::aes.do_aes_encrypt(inPad, Crypto::cipherLength_l, (byte *)Crypto::cipher_l, key, bits, iv);

  // int ciphLen = Crypto::cipherLength_l;
  // int clen = strlen((char*)Crypto::cipher_l);
  // int len =  clen - ciphLen;
  // int pad = ciphLen - len;
  // int padLeng = len + pad;
  // int padLeng = clen + len;
  // char cipheriv[padLeng];

  /*if (clen % 16 != 0) {
    int k = 0;
    for (int i = 0; i < padLeng; i++) {
      if (i < clen && k < ciphLen) {
        cipheriv[i] = Crypto::cipher_l[k++];
      } else {
        cipheriv[i] = 0x00;
      }
    }
    base64_encode(cipherText, (char*) cipheriv, padLeng);
  } else {*/
  base64_encode(cipherText, Crypto::cipher_l, Crypto::cipherLength_l);
  //}

  // Serial.print("> Cipher::");
  // Serial.println(cipherText);

  return 0;
}

byte Crypto::decrypt_long(char cipherText[], char *msg)
{
  return Crypto::decrypt_long(cipherText, msg, Crypto::key);
}

byte Crypto::decrypt_long(char cipherText[], char *msg, byte *key)
{
  aes.iv_inc();
  byte iv[N_BLOCK];
  int bits = 128;

  aes.set_IV(Crypto::my_iv);
  aes.get_IV(iv);

  // int ciphLen = 128; // 128
  // byte cipher[ciphLen];
  // int padLen = 48 + ciphLen + N_BLOCK;
  int padLen = Crypto::cipherLength_l + 16;
  // int padLen = strlen((char*)cipherText);
  char inPad[padLen];

  base64_decode((char *)inPad, (char *)cipherText, strlen((char *)cipherText));

  /*int k = 0;
  for (int i = 0; i < padLen; i++) {
    if (i < Crypto::cipherLength) {
      Crypto::cipher[i] = inPad[i];
    } else {
      Crypto::cipher[i] = '\0';
      break;
    }
  }*/

  byte clearText[padLen];

  Crypto::aes.do_aes_decrypt((byte *)inPad, Crypto::cipherLength_l, clearText, key, bits, iv);

  int padding = 0;
  for (int i = 0; i < Crypto::cipherLength_l; i++)
  {
    if (clearText[i] == 0x7C)
    {
      padding++;
    }
    else
    {
      break;
    }
  }
  int j = 0;
  // int k = padding;
  for (int i = padding; i < Crypto::cipherLength_l; i++)
  {
    if (clearText[i] == 0x7C)
    {
      break;
    }
    if (j < Crypto::cipherLength_l)
    {
      msg[j++] = clearText[i];
    }
  }

  msg[j] = '\0';

  return 0;
}

/* non-blocking wait function */
void Crypto::wait(unsigned long milliseconds)
{
  unsigned long timeout = millis() + milliseconds;
  while (millis() < timeout)
  {
    yield();
  }
}

void Crypto::array_to_string(const byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1 < 0xA ? '0' + nib1 : 'A' + nib1 - 0xA;
    buffer[i * 2 + 1] = nib2 < 0xA ? '0' + nib2 : 'A' + nib2 - 0xA;
  }
  buffer[len * 2] = '\0';
}

void Crypto::string_to_array(const char buffer[], unsigned int len, byte array[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    array[i] = (byte)buffer[i];
  }
}

// Crypto AES
Crypto crypto;
