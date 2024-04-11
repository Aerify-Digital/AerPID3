#ifndef BUMPSTOR_H
#define BUMPSTOR_H

#include "flash.h"

class BumpStor
{
public:
  void init(Flash *flash);

  void load();
  void save();
  void destroy();

  bool setupDefaults();

  void setTemp(double temp);
  double getTemp();

  void setTime(double temp);
  double getTime();

  void setTimeout(double temp);
  double getTimeout();

  void setEnabled(uint8_t elementIndex, bool isEnabled);
  bool getEnabled(uint8_t elementIndex);

private:
  Flash *flash;

  bool loaded = false;

  union DoubleByte
  {
    byte array[sizeof(double)];
    double num;
  };

  const uint _storLength = sizeof(double) +
                           sizeof(double) +
                           sizeof(double) +
                           1;

  double bump_Temp = 0;
  double bump_Time = 120;
  double timeout_Time = 10000;

  bool enabled1 = false;
  bool enabled2 = false;

  void saveBumpFile(char *storageData);
  void saveDeviceBumpFile(const char *filename, char *storageData);
  bool openBumpFile(char *storageData);
  bool openDeviceBumpFile(const char *filename, char *storageData);
};

extern BumpStor bumpStor;

#endif