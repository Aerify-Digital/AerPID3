#include "config.h"

void Config::init(Flash *flash, EepromStorage *eeprom)
{
  Config::flash = flash;
  Config::eeprom = eeprom;

  loadInfo();
  // load();
}

bool Config::setup(char *version)
{
  uint8_t isUUIDset = eeprom->loadUUIDflag();
  Config::internalVer = version;
  if (UUID.validate(Config::getUUID()) == false || isUUIDset == 0)
  {
    if (isUUIDset == 1)
    {
      char *euid = eeprom->loadUUID();
      Serial.println("UUID Validation Failed!  Reloading...");
      char name[] = "AerPID-Lite";
      Config::setName(name);
      Config::setUUID(euid);
      Config::setDate((char *)"2024-03-14 00:00:00");
      Config::setVers(version);
      Config::saveInfo();
      Serial.print("Device UUID: ");
      Serial.println(Config::getUUID());
      return true;
    }
    else
    {
      Serial.println("UUID Validation Failed!  Regenerating...");
      char name[] = "AerPID-Lite";
      Config::setName(name);
      char *uid = UUID.generate();
      eeprom->saveUUID(uid);
      eeprom->saveUUIDflag(1);
      Config::setUUID(uid);
      Config::setDate((char *)"2024-03-14 00:00:00");
      Config::setVers(version);
      Config::saveInfo();
      Serial.print("Device New UUID: ");
      Serial.println(Config::getUUID());
      return true;
    }
  }
  Serial.print("Device UUID: ");
  Serial.println(Config::getUUID());
  Serial.println("UUID Validation Passed!");
  return false;
}

// ==========================================================
// Config Values File
void Config::load()
{
  char data[128];

  Config::flash->openConfigFile((char *)data);

  if (data[4] == '1')
  {
    Config::useEncryption = true;
  }
  if (data[9] == '1')
  {
    Config::debugMode = true;
  }
  if (data[14] == '1')
  {
    Config::showLogout = true;
  }
  if (data[19] == '1')
  {
    Config::lockOnReboot = true;
  }
  if (data[24] == '1')
  {
    Config::autoLogout = true;
  }
  if (data[25] == 'M')
  {
    Config::interfaceMode = data[29];
  }

  // Serial.println((char*) data);

  Config::loaded = true;
}

void Config::save()
{
  char data[128];

  data[0] = 'E';
  data[1] = 'N';
  data[2] = 'C';
  data[3] = '=';
  if (Config::UseEncryption())
  {
    data[4] = '1';
  }
  else
  {
    data[4] = '2';
  }

  data[5] = 'D';
  data[6] = 'B';
  data[7] = 'G';
  data[8] = '=';
  if (Config::DebugMode())
  {
    data[9] = '1';
  }
  else
  {
    data[9] = '2';
  }

  data[10] = 'S';
  data[11] = 'H';
  data[12] = 'W';
  data[13] = '=';
  if (Config::ShowLogout())
  {
    data[14] = '1';
  }
  else
  {
    data[14] = '2';
  }

  data[15] = 'L';
  data[16] = 'C';
  data[17] = 'K';
  data[18] = '=';
  if (Config::LockOnReboot())
  {
    data[19] = '1';
  }
  else
  {
    data[19] = '2';
  }

  data[20] = 'A';
  data[21] = 'L';
  data[22] = 'G';
  data[23] = '=';
  if (Config::AutoLogout())
  {
    data[24] = '1';
  }
  else
  {
    data[24] = '2';
  }

  data[25] = 'M';
  data[26] = 'O';
  data[27] = 'D';
  data[28] = '=';
  data[29] = Config::InterfaceMode();

  data[30] = '\0';

  Config::flash->saveConfigFile(data);
}

bool Config::UseEncryption()
{
  return Config::useEncryption;
}

void Config::UseEncryption(bool value)
{
  Config::useEncryption = value;
}

bool Config::DebugMode()
{
  return Config::debugMode;
}

void Config::DebugMode(bool value)
{
  Config::debugMode = value;
}

bool Config::ShowLogout()
{
  return Config::showLogout;
}

void Config::ShowLogout(bool value)
{
  Config::showLogout = value;
}

bool Config::LockOnReboot()
{
  return Config::lockOnReboot;
}

void Config::LockOnReboot(bool value)
{
  Config::lockOnReboot = value;
}

bool Config::AutoLogout()
{
  return Config::autoLogout;
}

void Config::AutoLogout(bool value)
{
  Config::autoLogout = value;
}

int Config::InterfaceMode()
{
  return Config::interfaceMode;
}

void Config::InterfaceMode(int value)
{
  Config::interfaceMode = value;
}

// ==========================================================
// Device Setup Info File
void Config::loadInfo()
{
  char data[148];
  if (!Config::flash->openSetupFile((char *)data))
  {
    Serial.println("NO SETUP FOUND!!!");
    return;
  }

  int j = 0;
  for (int i = 0; i < 64; i++)
  {
    if (data[j] != 0x7c)
    {
      Config::name[i] = data[j];
    }
    j++;
  }
  j = 64;
  for (int i = 0; i < 36; i++)
  {
    if (data[j] != 0x7c)
    {
      Config::uuid[i] = data[j];
    }
    j++;
  }
  j = 100;
  for (int i = 0; i < 32; i++)
  {
    if (data[j] != 0x7c)
    {
      Config::date[i] = data[j];
    }
    j++;
  }
  j = 132;
  for (int i = 0; i < 12; i++)
  {
    if (data[j] != 0x7c)
    {
      Config::vers[i] = data[j];
    }
    j++;
  }
  Config::loaded = true;
}

void Config::saveInfo()
{
  char data[148];

  int j = 0;
  for (int i = 0; i < 64; i++)
  {
    if (i < strlen(Config::getName()))
    {
      data[j++] = Config::getName()[i];
    }
    else
    {
      data[j++] = 0x7c;
    }
  }
  j = 64;
  for (int i = 0; i < 36; i++)
  {
    if (i < strlen(Config::getUUID()))
    {
      data[j++] = Config::getUUID()[i];
      Serial.print(data[j]);
    }
    else
    {
      data[j++] = 0x7c;
    }
  }
  j = 100;
  for (int i = 0; i < 32; i++)
  {
    if (i < strlen(Config::getDate()))
    {
      data[j++] = Config::getDate()[i];
      Serial.print(data[j]);
    }
    else
    {
      data[j++] = 0x7c;
    }
  }
  j = 132;
  for (int i = 0; i < 12; i++)
  {
    if (i < strlen(Config::getVers()))
    {
      data[j++] = Config::getVers()[i];
    }
    else
    {
      data[j++] = 0x7c;
    }
  }
  data[144] = '\0';

  Config::flash->saveSetupFile(data);
}

void Config::setName(char name[])
{
  for (int i = 0; i < 64; i++)
  {
    Config::name[i] = name[i];
  }
}

void Config::setUUID(char uuid[])
{
  for (int i = 0; i < 36; i++)
  {
    Config::uuid[i] = uuid[i];
  }
}

void Config::setDate(char date[])
{
  for (int i = 0; i < 32; i++)
  {
    Config::date[i] = date[i];
  }
}

void Config::setVers(char vers[])
{
  for (int i = 0; i < 12; i++)
  {
    Config::vers[i] = vers[i];
  }
}

char *Config::getInternalVer()
{
  return Config::internalVer;
}

char *Config::getName()
{
  return Config::name;
}

char *Config::getUUID()
{
  return Config::uuid;
}

char *Config::getDate()
{
  return Config::date;
}

char *Config::getVers()
{
  return Config::vers;
}

// Config
Config config;
