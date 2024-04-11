#include "commStor.h"

void CommStor::init(Flash *flash)
{
  CommStor::flash = flash;

  loadBT();
  loadWiFi();
  loadUSB();
}

void CommStor::loadBT()
{
  char data[64];
  Serial.println("[CommStor]>Setup: Attempting to load BLE settings from Flash...");
  // File btfile = SPIFFS.open("/bluetooth.txt");
  if (!CommStor::flash->openBTFile((char *)data)) //(!btfile)
  {
    Serial.println("[CommStor]>Error: Bluetooth File not found!");
    // TODO: Setup Default BT values.
    CommStor::btEnabled = false;
    CommStor::btDiscoverable = false;
    CommStor::btMonitorOnly = false;
    CommStor::btLoaded = true;
    saveBT();
    return;
  }
  // String dAs = btfile.readString();
  // strcpy(data, dAs.c_str());

  // Serial.println((char *)data);

  if (data[4] == '1')
  {
    CommStor::btEnabled = true;
  }
  if (data[9] == '1')
  {
    CommStor::btDiscoverable = true;
  }
  if (data[14] == '1')
  {
    CommStor::btMonitorOnly = true;
  }

  // Serial.println((char*) data);

  CommStor::btLoaded = true;
}

void CommStor::saveBT()
{
  char data[64];

  data[0] = 'B';
  data[1] = 'L';
  data[2] = 'E';
  data[3] = '=';
  if (CommStor::getBTEn())
  {
    data[4] = '1';
  }
  else
  {
    data[4] = '0';
  }

  data[5] = 'D';
  data[6] = 'I';
  data[7] = 'S';
  data[8] = '=';
  if (CommStor::getBTDis())
  {
    data[9] = '1';
  }
  else
  {
    data[9] = '0';
  }

  data[10] = 'M';
  data[11] = 'O';
  data[12] = 'N';
  data[13] = '=';
  if (CommStor::getBTMon())
  {
    data[14] = '1';
  }
  else
  {
    data[14] = '0';
  }

  data[15] = '\0';

  // Serial.println((char *)data);
  CommStor::flash->saveBTFile(data);
  /*File btFile = SPIFFS.open("/bluetooth.txt", "w");
  if(btFile.print((char*) data)){
    Serial.println("File written succesfully: bluetooth.txt");
  }
  else{
    Serial.println("File-Write Failed: bluetooth.txt");
  }
  btFile.close();*/
  btSaved = true;
}

void CommStor::loadWiFi()
{
  Serial.println("[CommStor]>Setup: Attempting to load WiFi settings from Flash...");
  char data[134];
  // File wfFile = SPIFFS.open("/wifi.txt");

  // Open file from Flash and load, or generate defaults and save file.
  if (!CommStor::flash->openWifiFile((char *)data)) //(!wfFile)
  {
    Serial.println("[CommStor]>Error: Wifi File not found!");
    CommStor::wifiEnabled = true;
    CommStor::wifiLoaded = true;
    CommStor::hasSSID = false;
    CommStor::setSSID("NULL");
    CommStor::setPSK("NULL");
    saveWiFi();
    return;
  }
  // String dAs = wfFile.readString();
  // strcpy(data, dAs.c_str());

  // Serial.println((char *)data);

  if (data[2] == '1')
  {
    CommStor::wifiEnabled = true;
  }
  else
  {
    CommStor::wifiEnabled = false;
  }
  if (data[4] == '1')
  {
    CommStor::hasSSID = true;
  }
  else
  {

    CommStor::hasSSID = false;
  }

  int j = 5;
  for (int i = 0; i < 64; i++)
  {
    if (data[j] == 0)
    {
      break;
    }
    if (data[j] != 0x7c)
    {
      CommStor::wifiSSID[i] = data[j];
    }
    j++;
  }
  j = 69;

  for (int i = 0; i < 64; i++)
  {
    if (data[j] == 0)
    {
      break;
    }
    if (data[j] != 0x7c)
    {
      CommStor::wifiPass[i] = data[j];
    }
    j++;
  }

  // Serial.println((char*) data);

  CommStor::wifiLoaded = true;
}

void CommStor::saveWiFi()
{
  char data[134];

  data[0] = 'E';
  data[1] = 'N';
  if (CommStor::getWifiEn())
  {
    data[2] = '1';
  }
  else
  {
    data[2] = '0';
  }
  data[3] = 'S';
  if (CommStor::getSSIDSet())
  {
    data[4] = '1';
  }
  else
  {
    data[4] = '0';
  }
  int j = 5;
  for (int i = 0; i < 64; i++)
  {
    if (i < strlen(CommStor::getSSID()))
    {
      data[j++] = CommStor::getSSID()[i];
    }
    else
    {
      data[j++] = 0x7c;
    }
  }
  j = 69;
  for (int i = 0; i < 63; i++)
  {
    if (i < strlen(CommStor::getPSK()))
    {
      data[j++] = CommStor::getPSK()[i];
    }
    else
    {
      data[j++] = 0x7c;
    }
  }

  data[132] = '\0';
  data[133] = '\0';

  // Serial.println((char *)data);
  /*File wfFile = SPIFFS.open("/wifi.txt", "w");
  if(wfFile.print((char*) data)){
    Serial.println("File written succesfully: wifi.txt");
  }
  else{
    Serial.println("File-Write Failed: wifi.txt");
  }
  wfFile.close();*/
  CommStor::flash->saveWifiFile(data);
  wifiSaved = true;
}

bool CommStor::isWifiSaved()
{
  return wifiSaved;
}

bool CommStor::isUsbSaved()
{
  return usbSaved;
}

bool CommStor::isBleSaved()
{
  return btSaved;
}

void CommStor::loadUSB()
{
  Serial.println("[CommStor]>Setup: Attempting to load USB settings from Flash...");
  char data[64];
  // File usbFile = SPIFFS.open("/usb.txt");

  // Open file from Flash and load; or generate defaults and save file.
  if (!CommStor::flash->openUsbFile((char *)data)) //(!usbFile)
  {
    Serial.println("[CommStor]>Error: USB File not found!");
    // TODO: Setup USB Defaults.
    CommStor::usbEnabled = true;
    CommStor::usbDiscoverable = true;
    CommStor::usbMonitorOnly = false;
    CommStor::usbLoaded = true;
    saveUSB();
    return;
  }
  // String dAs = usbFile.readString();
  // strcpy(data, dAs.c_str());

  // Serial.println((char *)data);

  if (data[4] == '1')
  {
    CommStor::usbEnabled = true;
  }
  else
  {
    CommStor::usbEnabled = false;
  }
  if (data[9] == '1')
  {
    CommStor::usbDiscoverable = true;
  }
  if (data[14] == '1')
  {
    CommStor::usbMonitorOnly = true;
  }

  // Serial.println((char*) data);

  CommStor::usbLoaded = true;
}

void CommStor::saveUSB()
{
  char data[64];

  data[0] = 'U';
  data[1] = 'S';
  data[2] = 'B';
  data[3] = '=';
  if (CommStor::getUSBEn())
  {
    data[4] = '1';
  }
  else
  {
    data[4] = '0';
  }

  data[5] = 'D';
  data[6] = 'I';
  data[7] = 'S';
  data[8] = '=';
  if (CommStor::getUSBDis())
  {
    data[9] = '1';
  }
  else
  {
    data[9] = '0';
  }

  data[10] = 'M';
  data[11] = 'O';
  data[12] = 'N';
  data[13] = '=';
  if (CommStor::getUSBMon())
  {
    data[14] = '1';
  }
  else
  {
    data[14] = '0';
  }

  data[15] = '\0';

  // Serial.println((char *)data);
  /*File usbFile = SPIFFS.open("/usb.txt", "w");
  if(usbFile.print((char*) data)){
    Serial.println("File written succesfully: usb.txt");
  }
  else{
    Serial.println("File-Write Failed: usb.txt");
  }
  usbFile.close();*/

  CommStor::flash->saveUsbFile(data);
  usbSaved = true;
}

bool CommStor::setupDefaults()
{
  return false;
}

void CommStor::setBTEn(bool state)
{
  CommStor::btEnabled = state;
  btSaved = false;
}

bool CommStor::getBTEn()
{
  return CommStor::btEnabled;
}

void CommStor::setBTDis(bool state)
{
  CommStor::btDiscoverable = state;
  btSaved = false;
}

bool CommStor::getBTDis()
{
  return CommStor::btDiscoverable;
}

void CommStor::setBTMon(bool state)
{
  CommStor::btMonitorOnly = state;
  btSaved = false;
}

bool CommStor::getBTMon()
{
  return CommStor::btMonitorOnly;
}

void CommStor::setUSBEn(bool state)
{
  CommStor::usbEnabled = state;
  usbSaved = false;
}

bool CommStor::getUSBEn()
{
  return CommStor::usbEnabled;
}

void CommStor::setUSBDis(bool state)
{
  CommStor::usbDiscoverable = state;
  usbSaved = false;
}

bool CommStor::getUSBDis()
{
  return CommStor::usbDiscoverable;
}

void CommStor::setUSBMon(bool state)
{
  CommStor::usbMonitorOnly = state;
  usbSaved = false;
}

bool CommStor::getUSBMon()
{
  return CommStor::usbMonitorOnly;
}

bool CommStor::getWifiEn()
{
  return CommStor::wifiEnabled;
}
void CommStor::setWifiEn(bool state)
{
  CommStor::wifiEnabled = state;
  wifiSaved = false;
}

bool CommStor::getSSIDSet()
{
  return CommStor::hasSSID;
}
void CommStor::setSSIDSet(bool state)
{
  CommStor::hasSSID = state;
  wifiSaved = false;
}

char *CommStor::getSSID()
{
  return CommStor::wifiSSID;
}

void CommStor::setSSID(const char *ssid)
{
  for (int i = 0; i < 33; i++)
  {
    if (i >= strlen(ssid))
    {
      CommStor::wifiSSID[i] = '\0';
      break;
    }
    CommStor::wifiSSID[i] = ssid[i];
  }
  hasSetup = false;
  if (strlen(wifiSSID) > 0)
  {
    hasSetup = true;
  }
  wifiSaved = false;
}

void CommStor::setPSK(const char *pass)
{
  for (int i = 0; i < 64; i++)
  {
    if (i >= strlen(pass))
    {
      CommStor::wifiPass[i] = '\0';
      break;
    }
    CommStor::wifiPass[i] = pass[i];
  }
  hasPass = false;
  if (strlen(wifiPass) > 0)
  {
    hasPass = true;
  }
  wifiSaved = false;
}

char *CommStor::getPSK()
{
  return CommStor::wifiPass;
}

bool CommStor::hasPSK()
{
  return CommStor::hasPass;
}

bool CommStor::hasWifiSetup()
{
  return CommStor::hasSetup;
}

CommStor commstor;