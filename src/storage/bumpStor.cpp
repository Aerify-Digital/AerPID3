#include "bumpStor.h"

void BumpStor::init(Flash *flash)
{
  BumpStor::flash = flash;

  load();
}

void BumpStor::setTemp(double temp)
{
  BumpStor::bump_Temp = temp;
}

double BumpStor::getTemp()
{
  return BumpStor::bump_Temp;
}

void BumpStor::setTime(double temp)
{
  BumpStor::bump_Time = temp;
}

double BumpStor::getTime()
{
  return BumpStor::bump_Time;
}

void BumpStor::setTimeout(double temp)
{
  BumpStor::timeout_Time = temp;
}

double BumpStor::getTimeout()
{
  return BumpStor::timeout_Time;
}

void BumpStor::setEnabled(uint8_t ei, bool isEnabled)
{
  switch (ei)
  {
  case 0:
    BumpStor::enabled1 = isEnabled;
    break;
  case 1:
    BumpStor::enabled2 = isEnabled;
    break;
  default:
    break;
  }
}

bool BumpStor::getEnabled(uint8_t ei)
{
  switch (ei)
  {
  case 0:
    return BumpStor::enabled1;
  case 1:
    return BumpStor::enabled2;
  default:
    return false;
  }
}

// ==========================================================================
// ==========================================================================
// Bump File Save/Load

void BumpStor::saveBumpFile(char *storageData)
{
  BumpStor::saveDeviceBumpFile("bump.dat", storageData);
}

void BumpStor::saveDeviceBumpFile(const char *filename, char *storageData)
{
  flash->saveFile(filename, storageData, _storLength);
}

bool BumpStor::openBumpFile(char *storageData)
{
  return BumpStor::openDeviceBumpFile("bump.dat", storageData);
}

bool BumpStor::openDeviceBumpFile(const char *filename, char *storageData)
{
  return flash->openFile(filename, storageData, _storLength);
}

void BumpStor::load()
{
  char data[_storLength];
  Serial.println("[BumpStor]>Setup: Loading Bump settings from Flash.");
  if (!BumpStor::openBumpFile((char *)data))
  {
    Serial.println("[BumpStor]>Error: Bump File not found!");
    setupDefaults();
    return;
  }

  DoubleByte bTemp;
  DoubleByte bTime;
  DoubleByte bTimeout;

  uint di = 0;
  for (int i = 0; i < sizeof(double); i++)
  {
    bTemp.array[i] = data[di++];
  }
  for (int i = 0; i < sizeof(double); i++)
  {
    bTime.array[i] = data[di++];
  }
  for (int i = 0; i < sizeof(double); i++)
  {
    bTimeout.array[i] = data[di++];
  }
  BumpStor::bump_Temp = bTemp.num;
  BumpStor::bump_Time = bTime.num;
  BumpStor::timeout_Time = bTimeout.num;

  BumpStor::loaded = data[di] == 0;
}

void BumpStor::save()
{
  char data[_storLength];

  DoubleByte bTemp;
  DoubleByte bTime;
  DoubleByte bTimeout;

  bTemp.num = bump_Temp;
  bTime.num = bump_Time;
  bTimeout.num = timeout_Time;

  uint di = 0;
  for (int i = 0; i < sizeof(double); i++)
  {
    data[di++] = bTemp.array[i];
  }
  for (int i = 0; i < sizeof(double); i++)
  {
    data[di++] = bTime.array[i];
  }
  for (int i = 0; i < sizeof(double); i++)
  {
    data[di++] = bTimeout.array[i];
  }
  data[di] = '\0';

  BumpStor::saveBumpFile(data);
}

void BumpStor::destroy()
{
  const char *filename = "bump.dat";
  if (flash->fileExists(filename))
  {
    flash->deleteFile(filename);
  }
}

bool BumpStor::setupDefaults()
{
  Serial.println("[BumpStor] > Applying Defaults!");
  BumpStor::bump_Temp = 90;
  BumpStor::bump_Time = 300;
  BumpStor::timeout_Time = 10000;
  BumpStor::enabled1 = false;
  BumpStor::enabled2 = false;
  save();
  BumpStor::loaded = true;
  return false;
}

// ======================================

BumpStor bumpStor;
