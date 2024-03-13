#ifndef TrueRandom_h
#define TrueRandom_h

#include "freertos/FreeRTOS.h"

class TrueRandomClass
{
  public:
    int rand();
    long random();
    long random(long howBig);
    long random(long howsmall, long how);
    char randomByte(void);
    void mac(uint8_t* macLocation);
    void uuid(uint8_t* uuidLocation);

  private:
    void memfill(char* location, int size);

};

extern TrueRandomClass TrueRandom;

#endif