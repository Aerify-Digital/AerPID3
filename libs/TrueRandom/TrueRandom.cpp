#include "TrueRandom.h"

char TrueRandomClass::randomByte(void) {
  char result[1];
  esp_fill_random(result, 1);
  return result[0];
}

int TrueRandomClass::rand() {
  int result;
  uint8_t i;
  result = 0;
  for (i=2; i--;) result += result + TrueRandomClass::randomByte();
  return result;
}

long TrueRandomClass::random() {
  long result;
  uint8_t i;
  result = 0;
  for (i=4; i--;) result += result + TrueRandomClass::randomByte();
  return result;
}

long TrueRandomClass::random(long howBig) {
  long randomValue;
  long maxRandomValue = UINT32_MAX;

  if (!howBig) {
	  return 0;
  } else if (howBig > maxRandomValue) {
	  howBig = maxRandomValue;
  }

  randomValue = random(howBig);
  return randomValue;
}

long TrueRandomClass::random(long howSmall, long howBig) {
  if (howSmall >= howBig) return howSmall;
  long diff = howBig - howSmall;
  return TrueRandomClass::random(diff) + howSmall;
}

void TrueRandomClass::memfill(char* location, int size) {
  for (;size--;) *location++ = randomByte();
}

void TrueRandomClass::mac(uint8_t* macLocation) {
  memfill((char*)macLocation,6);
}

void TrueRandomClass::uuid(uint8_t* uuidLocation) {
  // Generate a Version 4 UUID according to RFC4122
  memfill((char*)uuidLocation,16);
  // Although the UUID contains 128 bits, only 122 of those are random.
  // The other 6 bits are fixed, to indicate a version number.
  uuidLocation[6] = 0x40 | (0x0F & uuidLocation[6]);
  uuidLocation[8] = 0x80 | (0x3F & uuidLocation[8]);
  // Special squid vars
  uuidLocation[3] = 0xDE;
  uuidLocation[4] = 0x00;
  uuidLocation[5] = 0x01;
}

TrueRandomClass TrueRandom;
