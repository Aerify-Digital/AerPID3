#include "flash.h"

void Flash::init(uint8_t pin)
{
  Flash::cs_pin = pin;

  pinMode(Flash::cs_pin, OUTPUT);
  digitalWrite(Flash::cs_pin, HIGH);

  SerialFlash.begin(Flash::cs_pin);

  delay(5);

#ifdef FLASH_DEBUG
  Serial.println("> Check Flash..");
  if (!Flash::checkFlash())
  {
    Serial.println("Failed to load FLASH...");
    delay(1000);
  }
  else
  {
    Serial.println("Flash Okay!");
  }
#endif
  // Uncomment below for factory reset!
  // Serial.println(F("Flash Erease All!"));
  // SerialFlash.eraseAll();
  Serial.println(F("Flash Initialized!"));
}

void Flash::eraseAll()
{
  Serial.println(F("Flash Erease All!"));
  SerialFlash.eraseAll();
}

// ==========================================================================
// ==========================================================================
// General File

void Flash::saveFile(const char *filename, char *dat, int len)
{
  Serial.print("Saving Flash File: ");
  Serial.println(filename);

  if (SerialFlash.exists(filename))
  {
    SerialFlash.remove(filename);
    Serial.println("Deleted Old File!");
    delayMicroseconds(50);
  }

  if (!SerialFlash.exists(filename))
  {
    Serial.print("Creating File...  ");
    SerialFlash.createErasable(filename, len);
    // Serial.println("File Saved!");
    delayMicroseconds(20);
  }

  SerialFlashFile keyFile = SerialFlash.open(filename);

  // Serial.print("Opened File.. ");
  Serial.println(filename);

  /*char keys[len];
  int p = 0;
  for (int k = 0; k < len; k++)
  {
    keys[k] = dat[p];
    p++;
  }*/

  // Serial.println("Saving File.. ");
  // Serial.print("FILE:  ");
  // Serial.print(keys);
  // Serial.print(" :: ");
  // Serial.print(len);

  // keyFile.write(keys, len);
  keyFile.write(dat, len);
  keyFile.close();

  Serial.print(" ... ");
  Serial.println("File Saved!");
}

bool Flash::openFile(const char *filename, char *dat, int len)
{
  if (!SerialFlash.exists(filename))
  {
    // Serial.println("SerialFlash.exists() = false");
    dat[0] = '\0';
    return false;
  }

  // Serial.println(" ");
  Serial.print("Opening File.. ");
  Serial.println(filename);

  SerialFlashFile keyFile = SerialFlash.open(filename);
  delayMicroseconds(20);

  // Serial.print("Reading File..  ");

  char buf[len];
  keyFile.read(buf, len);

  // Serial.print("FILE:  ");
  // Serial.print(buf);
  // Serial.print(" :: ");
  // Serial.println(len);

  keyFile.close();

  int p = 0;
  for (int k = 0; k < len; k++)
  {
    dat[p++] = buf[k];
  }

  return true;
}

// ==========================================================================
// ==========================================================================
// Auth File

void Flash::saveAuthFile(char *passphrase)
{
  Flash::saveAuthKeyFile("0x07.dat", (char *)passphrase);
}

void Flash::saveAuthKeyFile(const char *filename, char *passphrase)
{
  int leng = 64;
  saveFile(filename, passphrase, leng);
}

bool Flash::openAuthFile(char *passphrase)
{
  return Flash::openAuthKeyFile("0x07.dat", passphrase);
}

bool Flash::openAuthKeyFile(const char *filename, char *passphrase)
{
  int leng = 64;
  return openFile(filename, passphrase, leng);
}

// ==========================================================================
// ==========================================================================
// Crypto Key File
void Flash::saveCryptoFile(char *passphrase)
{
  Flash::saveAuthKeyFile("0x03.dat", passphrase);
}

bool Flash::openCryptoFile(char *passphrase)
{
  return Flash::openAuthKeyFile("0x03.dat", passphrase);
}

// ==========================================================================
// ==========================================================================
// Config File

void Flash::saveConfigFile(char *storageData)
{
  Flash::saveDeviceConfigFile("config.dat", storageData);
}

void Flash::saveDeviceConfigFile(const char *filename, char *storageData)
{
  int leng = 128;
  saveFile(filename, storageData, leng);
}

bool Flash::openConfigFile(char *storageData)
{
  return Flash::openDeviceConfigFile("config.dat", storageData);
}

bool Flash::openDeviceConfigFile(const char *filename, char *storageData)
{
  int leng = 128;
  return openFile(filename, storageData, leng);
}

// ==========================================================================
// ==========================================================================
// Setup File

void Flash::saveSetupFile(char *storageData)
{
  Flash::saveDeviceSetupFile("info.dat", storageData);
}

void Flash::saveDeviceSetupFile(const char *filename, char *storageData)
{
  int leng = 144;
  saveFile(filename, storageData, leng);
}

bool Flash::openSetupFile(char *storageData)
{
  return Flash::openDeviceSetupFile("info.dat", storageData);
}

bool Flash::openDeviceSetupFile(const char *filename, char *storageData)
{
  int leng = 144;
  return openFile(filename, storageData, leng);
}

// ==========================================================================
// ==========================================================================
// Bluetooth File

void Flash::saveBTFile(char *storageData)
{
  Flash::saveDeviceBTFile("bluetooth.dat", storageData);
}

void Flash::saveDeviceBTFile(const char *filename, char *storageData)
{
  int leng = 64;
  saveFile(filename, storageData, leng);
}

bool Flash::openBTFile(char *storageData)
{
  return Flash::openDeviceBTFile("bluetooth.dat", storageData);
}

bool Flash::openDeviceBTFile(const char *filename, char *storageData)
{
  int leng = 64;
  return openFile(filename, storageData, leng);
}

// ==========================================================================
// ==========================================================================
// USB File

void Flash::saveUsbFile(char *storageData)
{
  Flash::saveDeviceUsbFile("usb.dat", storageData);
}

void Flash::saveDeviceUsbFile(const char *filename, char *storageData)
{
  int leng = 64;
  saveFile(filename, storageData, leng);
}

bool Flash::openUsbFile(char *storageData)
{
  return Flash::openDeviceUsbFile("usb.dat", storageData);
}

bool Flash::openDeviceUsbFile(const char *filename, char *storageData)
{
  int leng = 64;
  return openFile(filename, storageData, leng);
}

// ==========================================================================
// ==========================================================================
// LED File

void Flash::saveLedFile(char *storageData)
{
  Flash::saveDeviceLedFile("led.dat", storageData);
}

void Flash::saveDeviceLedFile(const char *filename, char *storageData)
{
  int leng = 128;
  saveFile(filename, storageData, leng);
}

bool Flash::openLedFile(char *storageData)
{
  return Flash::openDeviceLedFile("led.dat", storageData);
}

bool Flash::openDeviceLedFile(const char *filename, char *storageData)
{
  int leng = 128;
  return openFile(filename, storageData, leng);
}

// ==========================================================================
// ==========================================================================
// WIFI File

void Flash::saveWifiFile(char *storageData)
{
  Flash::saveDeviceWifiFile("wifi.dat", storageData);
}

void Flash::saveDeviceWifiFile(const char *filename, char *storageData)
{
  int leng = 133;
  saveFile(filename, storageData, leng);
}

bool Flash::openWifiFile(char *storageData)
{
  return Flash::openDeviceWifiFile("wifi.dat", storageData);
}

bool Flash::openDeviceWifiFile(const char *filename, char *storageData)
{
  int leng = 133;
  return openFile(filename, storageData, leng);
}

// ==========================================================================
// ==========================================================================
// Favorites File

void Flash::saveFavoritesFile(char *storageData)
{
  Flash::saveDeviceFavoritesFile("favorites.dat", storageData);
}

void Flash::saveDeviceFavoritesFile(const char *filename, char *storageData)
{
  int leng = 289;
  saveFile(filename, storageData, leng);
}

bool Flash::openFavoritesFile(char *storageData)
{
  return Flash::openDeviceFavoritesFile("favorites.dat", storageData);
}

bool Flash::openDeviceFavoritesFile(const char *filename, char *storageData)
{
  int leng = 289;
  return openFile(filename, storageData, leng);
}

void Flash::save_favorites(double fava, double favb, double favc, double favd)
{
  const char *filename = "favorites.dat";
  char storageData[37];
  int leng = 36;

  union DoubleByte
  {
    byte array[8];
    double Num;
  } d;

  // Serial.print("Saving.. ");
  d.Num = fava;
  // Serial.print(d.Num);
  int j = 0;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      storageData[j++] = d.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  // Serial.print(" ");
  d.Num = favb;
  // Serial.print(d.Num);
  j = 9;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      storageData[j++] = d.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  // Serial.print(" ");
  d.Num = favc;
  // Serial.print(d.Num);
  j = 18;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      storageData[j++] = d.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  d.Num = favd;
  // Serial.print(d.Num);
  j = 27;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      storageData[j++] = d.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  storageData[36] = '\0';

  // Serial.println("");

  // Serial.print("Saving Flash File: ");
  // Serial.println(filename);

  if (SerialFlash.exists(filename))
  {
    SerialFlash.remove(filename);
    // Serial.println("Deleted Old File!");
    delay(130);
  }

  if (!SerialFlash.exists(filename))
  {
    // Serial.print("Creating File...  ");
    SerialFlash.createErasable(filename, leng);
    // Serial.println("File Saved!");
  }

  SerialFlashFile keyFile = SerialFlash.open(filename);

  // Serial.print("Opened File.. ");
  // Serial.println(filename);

  char keys[leng];
  int p = 0;
  for (int k = 0; k < leng; k++)
  {
    if (p < leng - 1)
    {
      keys[k] = storageData[p++];
    }
    else if (p < leng)
    {
      keys[k] = 0x7c;
    }
    else
    {
      keys[k] = '\0';
    }
  }

  /*Serial.println("Saving File.. ");
  Serial.print("FILE:  ");
  for (int k = 0; k < leng; k++) {
    if (k > 0 && k % 9 == 0) {
      Serial.print(" ");
    }
    if (keys[k] <= 9) {
      Serial.print("0");
    }
    Serial.print(keys[k], HEX);
  }
  Serial.print(" :: ");
  Serial.print(leng);*/

  keyFile.write(keys, leng);
  keyFile.close();

  // Serial.print(" ... ");
  // Serial.println("File Saved!");
}

// load
void Flash::load_favorites(double &fava, double &favb, double &favc, double &favd)
{
  const char *filename = "favorites.dat";
  int leng = 36;

  union DoubleByte
  {
    byte array[8];
    double Num;
  } d;

  if (!SerialFlash.exists(filename))
  {
    return;
  }

  // Serial.println(" ");
  // Serial.print("Opening File.. ");
  // Serial.println(filename);

  SerialFlashFile keyFile = SerialFlash.open(filename);

  // Serial.print("Reading File..  ");

  char buf[leng];
  keyFile.read(buf, leng);

  /*Serial.print("FILE:  ");
  for (int k = 0; k < leng; k++) {
    if (k > 0 && k % 9 == 0) {
      Serial.print(" ");
    }
    if (buf[k] <= 9) {
      Serial.print("0");
    }
    Serial.print(buf[k], HEX);
  }
  Serial.print(" :: ");
  Serial.println(leng);*/

  keyFile.close();

  d.Num = 0;
  // Serial.print("Read Values: ");
  int j = 0;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      d.array[i] = buf[j++];
    }
  }
  fava = d.Num;
  // Serial.print(" ");
  // Serial.print(fava);
  j = 9;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      d.array[i] = buf[j++];
    }
  }
  favb = d.Num;
  // Serial.print(" ");
  // Serial.print(favb);
  j = 18;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      d.array[i] = buf[j++];
    }
  }
  favc = d.Num;
  // Serial.print(" ");
  // Serial.print(favc);
  j = 27;
  for (int i = 0; i < 9; i++)
  {
    if (i < 8)
    {
      d.array[i] = buf[j++];
    }
  }
  favd = d.Num;
  // Serial.print(" ");
  // Serial.print(favd);

  // Serial.println("");
}

// ==========================================================================
// ==========================================================================
// Settings File

void Flash::save_setup(bool usage_det, bool auto_off, long off_time)
{
  const char *filename = "setup.dat";
  char storageData[25];
  int leng = 25;

  union DoubleByte
  {
    byte array[sizeof(double)];
    uint16_t Num;
  } dAr;

  union WordByte
  {
    byte array[2];
    uint16_t Num;
  } wAr;

  union LongByte
  {
    byte array[4];
    uint32_t Num;
  } lAr;

  union BoolByte
  {
    byte array[1];
    bool Num;
  } bAr;

  // Serial.print("Saving.. ");
  bAr.Num = usage_det;
  // Serial.print(d.Num);
  int j = 0;
  for (int i = 0; i < 2; i++)
  {
    if (i < 1)
    {
      storageData[j++] = bAr.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  // Serial.print(" ");
  bAr.Num = auto_off;
  // Serial.print(d.Num);
  j = 2;
  for (int i = 0; i < 2; i++)
  {
    if (i < 1)
    {
      storageData[j++] = bAr.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  // Serial.print(" ");
  lAr.Num = off_time;
  // Serial.print(d.Num);
  j = 5;
  for (int i = 0; i < 5; i++)
  {
    if (i < 4)
    {
      storageData[j++] = lAr.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  // Serial.print(" ");
  /*dAr.Num = bump_amt;
  // Serial.print(d.Num);
  j = 10;
  for (int i = 0; i < 9; i++)
  {
    if (i < 2)
    {
      storageData[j++] = dAr.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }
  // Serial.print(" ");
  lAr.Num = bump_time;
  // Serial.print(d.Num);
  j = 20;
  for (int i = 0; i < 5; i++)
  {
    if (i < 4)
    {
      storageData[j++] = lAr.array[i];
    }
    else
    {
      storageData[j++] = 0x7c;
    }
  }*/
  storageData[24] = '\0';
  // Serial.println("");

  // Serial.print("Saving Flash File: ");
  // Serial.println(filename);

  if (SerialFlash.exists(filename))
  {
    SerialFlash.remove(filename);
    // Serial.println("Deleted Old File!");
    delay(130);
  }

  if (!SerialFlash.exists(filename))
  {
    // Serial.print("Creating File...  ");
    SerialFlash.createErasable(filename, leng);
    // Serial.println("File Saved!");
  }

  SerialFlashFile keyFile = SerialFlash.open(filename);

  Serial.print("Opened File.. ");
  Serial.println(filename);

  char keys[leng];
  int p = 0;
  for (int k = 0; k < leng; k++)
  {
    if (p < leng - 1)
    {
      keys[k] = storageData[p++];
    }
    else if (p < leng)
    {
      keys[k] = 0x7c;
    }
    else
    {
      keys[k] = '\0';
    }
  }

  /*Serial.println("Saving File.. ");
  Serial.print("FILE:  ");
  for (int k = 0; k < leng; k++) {
    if (k > 0 && k % 9 == 0) {
      Serial.print(" ");
    }
    if (keys[k] <= 9) {
      Serial.print("0");
    }
    Serial.print(keys[k], HEX);
  }
  Serial.print(" :: ");
  Serial.print(leng);*/

  keyFile.write(keys, leng);
  keyFile.close();

  Serial.print(" ... ");
  Serial.println("File Saved!");
}

// load
void Flash::load_setup(bool &usage_det, bool &auto_off, long &off_time)
{
  const char *filename = "setup.dat";
  int leng = 25;

  union DoubleByte
  {
    byte array[sizeof(double)];
    double Num;
  } dAr;

  union WordByte
  {
    byte array[2];
    uint16_t Num;
  } wAr;

  union LongByte
  {
    byte array[4];
    uint32_t Num;
  } lAr;

  union BoolByte
  {
    byte array[1];
    bool Num;
  } bAr;

  if (!SerialFlash.exists(filename))
  {
    return;
  }

  // Serial.println(" ");
  Serial.print("Opening File.. ");
  Serial.print(filename);

  SerialFlashFile keyFile = SerialFlash.open(filename);

  Serial.println(" ok");
  // Serial.print("Reading File..  ");

  char buf[leng];
  keyFile.read(buf, leng);

  /*Serial.print("FILE:  ");
  for (int k = 0; k < leng; k++) {
    if (k > 0 && k % 5 == 0) {
      Serial.print(" ");
    }
    if (buf[k] <= 9) {
      Serial.print("0");
    }
    Serial.print(buf[k], HEX);
  }
  Serial.print(" :: ");
  Serial.println(leng);*/

  keyFile.close();

  // Serial.print("Read Values: ");
  int j = 0;
  for (int i = 0; i < 2; i++)
  {
    if (i < 1)
    {
      bAr.array[i] = buf[j++];
    }
  }
  usage_det = bAr.Num;
  // Serial.print(" ");
  // Serial.print(usage_det);
  j = 2;
  for (int i = 0; i < 2; i++)
  {
    if (i < 1)
    {
      bAr.array[i] = buf[j++];
    }
  }
  auto_off = bAr.Num;
  // Serial.print(" ");
  // Serial.print(auto_off);
  j = 5;
  for (int i = 0; i < 5; i++)
  {
    if (i < 4)
    {
      lAr.array[i] = buf[j++];
    }
  }
  off_time = lAr.Num;
  // Serial.print(" ");
  // Serial.print(off_time);
  j = 10;
  for (int i = 0; i < 8; i++)
  {
    if (i < 4)
    {
      dAr.array[i] = buf[j++];
    }
  }
  // bump_amt = dAr.Num;
  //  Serial.print(" ");
  //  Serial.print(bump_amt);
  j = 15;
  for (int i = 0; i < 5; i++)
  {
    if (i < 4)
    {
      lAr.array[i] = buf[j++];
    }
  }
  // bump_time = lAr.Num;
  //  Serial.print(" ");
  //  Serial.print(bump_time);
  //  Serial.println("");
}

void Flash::save_settings_lights(bool LIGHT_STAT_EN, bool LIGHT_AMBI_EN, uint8_t LIGHT_STYLE_CYCLE, String LIGHT_COLOR_EN, String LIGHT_COLOR_DIS)
{
}

void Flash::load_settings_lights(bool &LIGHT_STAT_EN, bool &LIGHT_AMBI_EN, uint8_t &LIGHT_STYLE_CYCLE, String &LIGHT_COLOR_EN, String &LIGHT_COLOR_DIS)
{
}

// ================================================================================
// ================================================================================
// FLASH FUNCTIONS
// ===============================================================
void Flash::deleteKeys()
{
  SerialFlash.remove("0x07.dat");
  SerialFlash.remove("0x03.dat");
}

void Flash::deleteStorage()
{
  // deleteFile("");
}

void Flash::deleteFile(const char *filename)
{
  if (SerialFlash.exists(filename))
  {
    SerialFlash.remove(filename);
  }
}

bool Flash::fileExists(const char *filename)
{
  if (SerialFlash.exists(filename))
  {
    return true;
  }
  return false;
}

// ==========================================================================
// ==========================================================================
// Flash Check
// ==========================================================================
bool Flash::checkFlash()
{
  unsigned char buf[256];
  unsigned long chipsize, blocksize;

  // Read the chip identification
  Serial.println();
  Serial.println(F("Read Chip Identification:"));
  SerialFlash.readID(buf);
  Serial.print(F("  JEDEC ID:     "));
  Serial.print(buf[0], HEX);
  Serial.print(" ");
  Serial.print(buf[1], HEX);
  Serial.print(" ");
  Serial.println(buf[2], HEX);
  Serial.print(F("  Part Nummber: "));
  Serial.println(id2chip(buf));
  Serial.print(F("  Memory Size:  "));
  chipsize = SerialFlash.capacity(buf);
  Serial.print(chipsize);
  Serial.println(F(" bytes"));
  if (chipsize == 0)
    return false;
  Serial.print(F("  Block Size:   "));
  blocksize = SerialFlash.blockSize();
  Serial.print(blocksize);
  Serial.println(F(" bytes"));
  Serial.print(F("  Frequency:    "));
  Serial.print(SerialFlash.frequency() * 0.000001);
  Serial.println(F(" Mhz"));
  return true;
}

const char *Flash::id2chip(const unsigned char *id)
{
  if (id[0] == 0xEF)
  {
    // Winbond
    if (id[1] == 0x40)
    {
      if (id[2] == 0x14)
        return "W25Q80BV";
      if (id[2] == 0x15)
        return "W25Q16DV";
      if (id[2] == 0x17)
        return "W25Q64FV";
      if (id[2] == 0x18)
        return "W25Q128FV";
      if (id[2] == 0x19)
        return "W25Q256FV";
    }
    if (id[1] == 0xAA)
    {
      if (id[2] == 0x21)
        return "W25N01GV";
    }
  }
  if (id[0] == 0x01)
  {
    // Spansion
    if (id[1] == 0x02)
    {
      if (id[2] == 0x16)
        return "S25FL064A";
      if (id[2] == 0x19)
        return "S25FL256S";
      if (id[2] == 0x20)
        return "S25FL512S";
    }
    if (id[1] == 0x20)
    {
      if (id[2] == 0x18)
        return "S25FL127S";
    }
  }
  if (id[0] == 0xC2)
  {
    // Macronix
    if (id[1] == 0x20)
    {
      if (id[2] == 0x18)
        return "MX25L12805D";
    }
  }
  if (id[0] == 0x20)
  {
    // Micron
    if (id[1] == 0xBA)
    {
      if (id[2] == 0x20)
        return "N25Q512A";
      if (id[2] == 0x21)
        return "N25Q00AA";
    }
    if (id[1] == 0xBB)
    {
      if (id[2] == 0x22)
        return "MT25QL02GC";
    }
  }
  if (id[0] == 0xBF)
  {
    // SST
    if (id[1] == 0x25)
    {
      if (id[2] == 0x02)
        return "SST25WF010";
      if (id[2] == 0x03)
        return "SST25WF020";
      if (id[2] == 0x04)
        return "SST25WF040";
      if (id[2] == 0x41)
        return "SST25VF016B";
      if (id[2] == 0x4A)
        return "SST25VF032";
    }
    if (id[1] == 0x25)
    {
      if (id[2] == 0x01)
        return "SST26VF016";
      if (id[2] == 0x02)
        return "SST26VF032";
      if (id[2] == 0x43)
        return "SST26VF064";
    }
  }
  return "(unknown chip)";
}

// Setup Flash Memory
Flash flash;
