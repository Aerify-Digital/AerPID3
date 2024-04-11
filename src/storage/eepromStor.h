#ifndef EEPROM_STORAGE_H
#define EEPROM_STORAGE_H

#include <Wire.h>
#include "extEEPROM.h"
#include "common/enum/ThermalUnitsType.h"

class EepromStorage
{

public:
  void init(extEEPROM *eeprom, uint8_t PIN);

  char *rd(uint16_t o, uint8_t l);
  void rb(uint16_t o, int c);
  char rdb(uint16_t o);
  void wr(uint16_t o, const double d);
  void wrb(uint16_t o, uint8_t c);

  bool write(int offset, byte *buf, int length);
  bool write(int offset, byte buf);
  void writeEnable();
  void writeDisable();

  void saveUUIDflag(uint8_t f);
  uint8_t loadUUIDflag();
  void saveUUID(char *uuid);
  char *loadUUID();

  void saveKeyAES(char *key);
  char *loadKeyAES();

  void save_kP(double p);
  double load_kP();
  void save_kI(double p);
  double load_kI();
  void save_kD(double p);
  double load_kD();

  char *getData();
  void setData(char *data);

private:
  // EEPROM hook
  extEEPROM *ee;
  uint8_t EEP_PIN_EN;

  char data[128];
};

extern EepromStorage eepromStor;

#endif
