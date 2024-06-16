#ifndef FLASH_H
#define FLASH_H

#include <SerialFlash.h>
#include <SPI.h>

#define FLASH_DEBUG

class Flash
{
public:
  void init(uint8_t cs_pin);

  void eraseAll();

  void deleteKeys();
  void deleteStorage();

  void deleteFile(const char *filename);
  bool fileExists(const char *filename);

  void saveFile(const char *filename, char *dat, int len);
  bool openFile(const char *filename, char *dat, int len);

  void saveAuthFile(char *passphrase);
  bool openAuthFile(char *passphrase);

  void saveCryptoFile(char *passphrase);
  bool openCryptoFile(char *passphrase);

  void saveConfigFile(char *storageData);
  bool openConfigFile(char *storageData);

  void saveSetupFile(char *storageData);
  bool openSetupFile(char *storageData);

  void saveFavoritesFile(char *storageData);
  bool openFavoritesFile(char *storageData);

  void saveBTFile(char *storageData);
  bool openBTFile(char *storageData);

  void saveWifiFile(char *storageData);
  bool openWifiFile(char *storageData);

  void saveUsbFile(char *storageData);
  bool openUsbFile(char *storageData);

  void saveLedFile(char *storageData);
  bool openLedFile(char *storageData);

  void save_favorites(double fava, double favb, double favc, double favd);
  void load_favorites(double &fava, double &favb, double &favc, double &favd);

  void save_setup(bool usage_det, bool auto_off, long off_time);
  void load_setup(bool &usage_det, bool &auto_off, long &off_time);

  void save_settings_lights(bool LIGHT_STAT_EN, bool LIGHT_AMBI_EN, uint8_t LIGHT_STYLE_CYCLE, String LIGHT_COLOR_EN, String LIGHT_COLOR_DIS);
  void load_settings_lights(bool &LIGHT_STAT_EN, bool &LIGHT_AMBI_EN, uint8_t &LIGHT_STYLE_CYCLE, String &LIGHT_COLOR_EN, String &LIGHT_COLOR_DIS);
  void save_settings_lights_cus(String LIGHT_COLOR_DIS);
  void load_settings_lights_cus(String &LIGHT_COLOR_DIS);

private:
  uint8_t cs_pin;

  bool checkFlash();
  const char *id2chip(const unsigned char *id);

  void saveAuthKeyFile(const char *filename, char *passphrase);
  bool openAuthKeyFile(const char *filename, char *passphrase);

  void saveDeviceConfigFile(const char *filename, char *storageData);
  bool openDeviceConfigFile(const char *filename, char *storageData);

  void saveDeviceSetupFile(const char *filename, char *storageData);
  bool openDeviceSetupFile(const char *filename, char *storageData);

  void saveDeviceFavoritesFile(const char *filename, char *storageData);
  bool openDeviceFavoritesFile(const char *filename, char *storageData);

  void saveDeviceBTFile(const char *filename, char *storageData);
  bool openDeviceBTFile(const char *filename, char *storageData);

  void saveDeviceUsbFile(const char *filename, char *storageData);
  bool openDeviceUsbFile(const char *filename, char *storageData);

  void saveDeviceLedFile(const char *filename, char *storageData);
  bool openDeviceLedFile(const char *filename, char *storageData);

  void saveDeviceWifiFile(const char *filename, char *storageData);
  bool openDeviceWifiFile(const char *filename, char *storageData);
};

// Flash Memory
extern Flash flash;

#endif
