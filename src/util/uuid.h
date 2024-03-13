#ifndef UUID_H
#define UUID_H

#include <TrueRandom.h>

class UUIDclass
{

public:
  char *generate();
  bool validate(char *uuidChars);

  void convertArrayToHex(char *hexCharArray, uint8_t *hexByteArray);

private:
  uint8_t uuidNumber[16]; // UUIDs in binary form are 16 bytes long
  char uuidValue[32];

  void printHex(uint8_t number, char *hexChars);
  void printUuid(uint8_t *uuidNumber);

  char convertCharToHex(char ch);
};

extern UUIDclass UUID;

#endif
