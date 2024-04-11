#ifndef COMMSSTOR_H
#define COMMSSTOR_H

#include "flash.h"
#include "SPIFFS.h"

class CommStor
{
public:
  void init(Flash *flash);

  void loadBT();
  void saveBT();

  void loadWiFi();
  void saveWiFi();

  void loadUSB();
  void saveUSB();

  bool setupDefaults();

  bool isBleSaved();
  void setBTEn(bool state);
  bool getBTEn();
  void setBTDis(bool state);
  bool getBTDis();
  void setBTMon(bool state);
  bool getBTMon();

  bool isUsbSaved();
  void setUSBEn(bool state);
  bool getUSBEn();
  void setUSBDis(bool state);
  bool getUSBDis();
  void setUSBMon(bool state);
  bool getUSBMon();

  bool isWifiSaved();
  bool getWifiEn();
  void setWifiEn(bool state);
  bool getSSIDSet();
  void setSSIDSet(bool state);
  char *getSSID();
  void setSSID(const char *ssid);
  char *getPSK();
  void setPSK(const char *pass);
  bool hasPSK();
  bool hasWifiSetup();

private:
  Flash *flash;

  bool usbLoaded = false;
  bool btLoaded = false;
  bool wifiLoaded = false;

  bool btSaved = true;
  bool btEnabled = false;
  bool btDiscoverable = false;
  bool btMonitorOnly = false;

  bool usbSaved = true;
  bool usbEnabled = true;
  bool usbDiscoverable = true;
  bool usbMonitorOnly = false;

  bool wifiSaved = true;
  bool wifiEnabled = true;
  bool hasSSID = false;
  bool hasPass = false;
  bool hasSetup = false;
  char wifiSSID[33];
  char wifiPass[64];
};

extern CommStor commstor;

#endif