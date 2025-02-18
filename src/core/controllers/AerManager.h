#ifndef _AER_MANAGER_H
#define _AER_MANAGER_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

#include "common/datatypes/NetworkVars.h"
#include "common/datatypes/DateTimeRTC.h"
#include "common/datatypes/LocalTemperature.h"
#include "common/datatypes/Version.h"
#include "common/enum/ThermalUnitsType.h"
#include "common/enum/ElementProtectOpCodes.h"

#include "screen/gui/gui.h"
#include "core/lighting/FancyLED.h"

#include "storage/favsStor.h"
#include "storage/commStor.h"
#include "storage/bumpStor.h"
#include "storage/lights.h"
#include "storage/tempStor.h"
#include "storage/uiSettingsStor.h"
#include "storage/networkStor.h"
#include "storage/measureStor.h"

#include "AerPID.h"

class FancyLED;

class SettingsStorage;

/// @brief Storage accesor class for cross thread data sharing and handling
class AerManager
{
public:
    AerManager() { this->intialized = false; }
    AerManager(AerPID *stor1, NetworkVars *netVars)
    {
        this->aerPID[0] = stor1;
        this->networkVars = netVars;
        init();
    }
#if AERPID_COUNT == 2
    AerManager(AerPID *stor1, AerPID *stor2, NetworkVars *netVars)
    {
        this->aerPID[0] = stor1;
        this->aerPID[1] = stor2;
        this->networkVars = netVars;
        init();
    }
#endif

    void setGUI(AerGUI *gui);
    AerGUI *getGUI();

    void setFancyLED(FancyLED *xled);
    FancyLED *getFancyLED();

    void setVersionApp(AppVersion *version);
    void setVersion(uint major, uint minor, uint build);
    void setVersionWeb(AppVersion *version);

    void setupStorageObjects(FavsStor *f, CommStor *c, LightsStor *l, BumpStor *b, TempStor *s, SettingsStorage *ss, NetworkingStorage *n);

    AppVersion *getVersion();
    AppVersion *getVersionWeb();

    /// @brief Gets the AerPID class Object
    /// @return AerPID
    AerPID *getAerPID(uint8_t elementIndex);
    void setMeasureMode(uint8_t mode, bool save);
    uint8_t getMeasureMode();

    /// @brief Gets the Network Storage Object
    /// @return NetworkVars
    NetworkVars *getNetVars();

    FavsStor *getFavs();
    CommStor *getComms();
    LightsStor *getLights();
    BumpStor *getBump();
    TempStor *getTemp();
    NetworkingStorage *getNet();

    SettingsStorage *getSettings() { return this->settingsObj; }

    // Date Time
    void setDateTime(uint8_t hour,
                     uint8_t minute,
                     uint8_t second,
                     uint8_t year,
                     uint8_t month,
                     uint8_t day);
    DateTimeRTC getDateTime();

    // Local Temps
    void setLocalTemps(float tempA, float tempB);
#if AERPID_COUNT == 2
    void setLocalTemps(float tempA, float tempB, float tempC);
#endif
    float getLocalTempA();
    float getLocalTempB();
#if AERPID_COUNT == 2
    float getLocalTempC();
#endif

    // element monitor op code
    void setOpEP(_elm_prot_op_code_t *op);
    _elm_prot_op_code_t getOpEP();

    // reading type (celsius toggle)
    void setReadingType(ThermalUnitsType u)
    {
        this->localTemp.thermalUnit = u;
        this->getSettings()->setThermalUnit(u, false);
    }
    ThermalUnitsType getReadingType() { return this->localTemp.thermalUnit; }

    bool isScreenSaverActive() { return this->saverActive; }
    void setScreenSaverActive(bool active) { this->saverActive = active; }

    bool isUpdateDateTime() { return this->updateDateTime; }
    void setUpdateDateTime(bool b) { this->updateDateTime = b; }

    bool updateLStor() { return this->updateLightStor; }
    void updateLStor(bool b) { this->updateLightStor = b; }

    bool updateFStor() { return this->updateFavStor; }
    void updateFStor(bool b) { this->updateFavStor = b; }

    bool updateBumpStor() { return this->updatebumpStor; }
    void updateBumpStor(bool b) { this->updatebumpStor = b; }

    bool updateBtStor() { return this->updatebtStor; }
    void updateBtStor(bool b) { this->updatebtStor = b; }

    bool updateWifiStor() { return this->updatewifiStor; }
    void updateWifiStor(bool b) { this->updatewifiStor = b; }

    bool updateUsbStor() { return this->updateusbStor; }
    void updateUsbStor(bool b) { this->updateusbStor = b; }

    bool updateTempStor(uint8_t ei) { return ei == 0 ? this->updatetempStor : this->updatetempStor2; }
    void updateTempStor(uint8_t ei, bool b) { ei == 0 ? this->updatetempStor = b : this->updatetempStor2 = b; }

    bool updateLightState() { return this->updatelightState; }
    void updateLightState(bool b) { this->updatelightState = b; }

    bool updateKnobState() { return this->updateknobState; }
    void updateKnobState(bool b) { this->updateknobState = b; }

    bool updateUnitsStor() { return this->updateunitsStor; }
    void updateUnitsStor(bool b) { this->updateunitsStor = b; }

    bool updateFlashStor() { return this->updateflashStor; }
    void updateFlashStor(bool b) { this->updateflashStor = b; }

    bool updatedByWeb() { return this->updatedbyWeb; }
    void updatedByWeb(bool b) { this->updatedbyWeb = b; }

    bool webUpdateLEDState() { return this->webupdateledstate; }
    void webUpdateLEDState(bool b) { this->webupdateledstate = b; }

    bool webUpdateLEDMode() { return this->webupdateledmode; }
    void webUpdateLEDMode(bool b) { this->webupdateledmode = b; }

    bool webUpdateLEDBright() { return this->webupdateledbright; }
    void webUpdateLEDBright(bool b) { this->webupdateledbright = b; }

    bool webUpdateLEDColor() { return this->webupdateledcolor; }
    void webUpdateLEDColor(bool b) { this->webupdateledcolor = b; }

    bool webUpdateLEDStatus() { return this->webupdateledstatus; }
    void webUpdateLEDStatus(bool b) { this->webupdateledstatus = b; }

    bool webUpdateFavs() { return this->webupdatefavs; }
    void webUpdateFavs(bool b) { this->webupdatefavs = b; }

    bool webUpdateBump() { return this->webupdatebump; }
    void webUpdateBump(bool b) { this->webupdatebump = b; }

    bool webUpdateBLE() { return this->webupdateble; }
    void webUpdateBLE(bool b) { this->webupdateble = b; }

    bool webUpdateWIFI() { return this->webupdatewifi; }
    void webUpdateWIFI(bool b) { this->webupdatewifi = b; }

    bool webUpdateUSB() { return this->webupdateusb; }
    void webUpdateUSB(bool b) { this->webupdateusb = b; }

    bool webUpdateTemp() { return this->webupdatetemp; }
    void webUpdateTemp(bool b) { this->webupdatetemp = b; }

    bool webUpdatePID() { return this->webupdatepid; }
    void webUpdatePID(bool b) { this->webupdatepid = b; }

    uint64_t getPressTick() { return this->btn_press_tick; }
    bool isPressTickReady() { return this->btn_press_tick <= 0; }
    void setPressTick(uint64_t pt) { this->btn_press_tick = pt; }
    void tickPressTick()
    {
        if (this->btn_press_tick > 0)
        {
            this->btn_press_tick -= 1;
        }
    }

    // control functions for task health monitor
    void setCpu0(uint8_t v);
    uint8_t getCpu0();
    void setCpu1(uint8_t v);
    uint8_t getCpu1();

    void addCpu0(uint8_t v);
    uint8_t *getCpu0s();
    void addCpu1(uint8_t v);
    uint8_t *getCpu1s();

    uint getCpuBarCount();

    uint8_t getCpu0Avg();
    uint8_t getCpu1Avg();
    uint8_t getCpu0Avg(uint8_t size);
    uint8_t getCpu1Avg(uint8_t size);

    bool getConnectWifi() { return this->connect_wifi; }
    void setConnectWifi(bool b) { this->connect_wifi = b; }

    bool getActiveWifi() { return this->active_wifi; }
    void setActiveWifi(bool b) { this->active_wifi = b; }

    bool getActiveBle() { return this->active_ble; }
    void setActiveBle(bool b) { this->active_ble = b; }

    bool getActiveUsb() { return this->active_usb; }
    void setActiveUsb(bool b) { this->active_usb = b; }

    bool getActiveWeb() { return this->active_web; }
    void setActiveWeb(bool b) { this->active_web = b; }

    void setEnabledDTR(bool e)
    {
        this->enabled_dtr = e;
        this->webUpdateDTR(true);
        this->hasUpdateDTR(true);
    }
    bool isEnabledDTR() { return this->enabled_dtr; }
    bool webUpdateDTR() { return this->webupdatedtr; }
    void webUpdateDTR(bool b) { this->webupdatedtr = b; }
    bool hasUpdateDTR() { return this->hasupdatedtr; }
    void hasUpdateDTR(bool b) { this->hasupdatedtr = b; }

    void addWebFName(int f);
    int getWebFnameVal(int f);
    void clearWebFNames();
    void addWebFTemp(int f);
    int getWebFtempVal(int f);
    void clearWebFTemps();

    bool lastSensorFault(uint8_t ei);
    void lastSensorFault(uint8_t ei, bool faulted);

protected:
    void init();

    friend class FancyLED;
    friend class SettingsStorage;

private:
    bool intialized;

    AerGUI *gui;

    AppVersion *version;
    AppVersion *versionWeb;

    AerPID *aerPID[AERPID_COUNT];
    uint8_t measMode;

    NetworkVars *networkVars;

    FavsStor *favsObj;
    CommStor *commsObj;
    LightsStor *lightsObj;
    BumpStor *bumpObj;
    TempStor *tempObj;
    NetworkingStorage *netObj;

    FancyLED *xled;

    DateTimeRTC dateTime;
    LocalTemperature localTemp;

    SettingsStorage *settingsObj;

    // Element protection circuit operation state (read-only)
    _elm_prot_op_code_t ep_op_code;

    bool connect_wifi = false;
    bool active_wifi = false;
    bool active_ble = false;
    bool active_usb = false;
    bool active_web = false;

    bool saverActive = false;
    bool updateDateTime = false;
    bool updateLightStor = false;
    bool webupdateledstate = false;
    bool webupdateledmode = false;
    bool webupdateledbright = false;
    bool webupdateledcolor = false;
    bool webupdateledstatus = false;
    bool updateFavStor = false;
    bool webupdatefavs = false;
    int webfnames[4] = {0};
    int webftemps[4] = {0};
    bool updatebumpStor = false;
    bool webupdatebump = false;
    bool updatebtStor = false;
    bool webupdateble = false;
    bool updatewifiStor = false;
    bool webupdatewifi = false;
    bool updateusbStor = false;
    bool webupdateusb = false;
    bool updatetempStor = false;
    bool updatetempStor2 = false;
    bool webupdatetemp = false;
    bool webupdatepid = false;
    bool webupdatedtr = false;
    bool hasupdatedtr = false;
    bool updatelightState = false;
    bool updateknobState = false;
    bool updateunitsStor = false;
    bool updateflashStor = false;
    bool updatedbyWeb = false;

    bool enabled_dtr = false;

    uint64_t btn_press_tick = 100;

    bool lastFault[2] = {false, false};

    // task health monitor vars
    int cpu0_usage = 0;
    int cpu1_usage = 0;

    static const uint cpu_usages_size = 15 * 2; // must be multiple of 30. or width of bar.
    uint8_t cpu0_usages[cpu_usages_size];
    uint8_t cpu1_usages[cpu_usages_size];

    void shiftArrDown(uint8_t *cpu0_usages);
};

extern AerManager aerManager;

#endif