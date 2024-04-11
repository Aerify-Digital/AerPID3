#ifndef CONFIG_H
#define CONFIG_H

#include "flash.h"
#include "eepromStor.h"
#include "util/uuid.h"

class Config
{
public:
  void init(Flash *flash, EepromStorage *eeprom);

  void load();
  void save();

  bool setup(char *version);

  bool UseEncryption();
  void UseEncryption(bool value);

  bool DebugMode();
  void DebugMode(bool value);

  bool ShowLogout();
  void ShowLogout(bool value);

  bool LockOnReboot();
  void LockOnReboot(bool value);

  bool AutoLogout();
  void AutoLogout(bool value);

  int InterfaceMode();
  void InterfaceMode(int value);

  void setName(char name[]);
  void setUUID(char uuid[]);
  void setDate(char date[]);
  void setVers(char vers[]);

  char *getInternalVer();

  char *getName();
  char *getUUID();
  char *getDate();
  char *getVers();

  void loadInfo();
  void saveInfo();

private:
  bool loaded = false;
  Flash *flash;
  EepromStorage *eeprom;

  bool useEncryption = true;
  bool debugMode = true;
  bool showLogout = false;
  bool lockOnReboot = false;
  bool autoLogout = false;

  int interfaceMode = 255;

  char *internalVer;

  char name[65];
  char uuid[37];
  char date[33];
  char vers[13];
};

extern Config config;

#endif
