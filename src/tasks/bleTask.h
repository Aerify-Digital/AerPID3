#ifndef BLETASK_H
#define BLETASK_H

#include <SerialFlash.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"

#include <PinDefs.h>
#include "common/datatypes/mutex.h"
#include "common/datatypes/PropsTFT.h"
#include "common/enum/menu_definition.h"

#include "core/controllers/AerManager.h"

#include "BleCallbacks/CoilToggle.h"
#include "BleCallbacks/setTempCallbacks.h"

#define LOG_TAG "BLE"

// =======================
// Bluetooth LE Objects
// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// BLE Service UUIDS
#define SERVICE_UUID "4fafc201-1fb5-869e-8fcc-c5c9c331864b"
#define BLE_PASSKEY 420710
// BLE Characteristic UUIDS
// Temp UUIDS
#define CHARACTERISTIC_UUID "5aecfbde-71fa-459e-b560-94d5cc055b77"  // Coil Toggle
#define CHARACTERISTIC2_UUID "beb5438e-36e1-4688-b7f5-ea07361b26a8" // Averaged Temp
#define CHARACTERISTIC3_UUID "6966d7a7-e523-4c3d-9ace-e2827907d0fe" // Set_temp
#define QUICKADJU_CHAR_UUID "79a3b719-8647-4d84-b60e-af553985043c"  // up / down by read/write
#define QUICKAMT_CHAR_UUID "02c36960-5299-4acd-a56f-c3a247c5f1bb"   // Amount of quick adjust
#define FAV_TEMP1_UUID "9c51a50b-7c73-4bee-bbf5-ec6e8fac51cd"       // Read: shows set fav temp, Write sends it
#define FAV_TEMP2_UUID "d31ba0f0-f2cb-4527-ac77-b2d1db28683f"       //
#define FAV_TEMP3_UUID "eea584a4-3daf-4544-adb8-f119ded3606e"       //
#define FAV_TEMP4_UUID "f9418828-771a-43c2-9496-0ae37191a312"       //
// LED UUIDS
#define LedEnable_UUID "7644abee-c831-47bb-9bdf-1a239cc666db"  //
#define LedReset_UUID "4f07820d-e8f9-4413-aa47-c4691713f1ae"   //
#define LedStatus_UUID "eb0d7807-40e7-4817-8be6-dbc5a7f6b207"  //
#define LedAmbient_UUID "bc541bf1-df59-4cbf-96eb-7601ddad59a8" //
#define LedPresets_UUID "28521d7b-6540-4621-aeb7-d71c4f468a1e" //
// Led Preset UUIDS
#define LedRainbowWave_UUID "60a118d9-f9c5-4365-98ff-a08d13909dc6"  //
#define LedRainbow_UUID "92fe204a-4ede-4cec-91ed-67015728132d"      //
#define LedRainbowPulse_UUID "9ef0b08e-174e-4b6d-adb5-c403fb9d05bd" //
#define ColorShift_UUID "a35a64b3-8c4b-47cd-ac2c-8e5c96e43f8a"      //
#define ColorPulse_UUID "6e9c993e-b4e9-4831-8b17-c848a30fc47f"      //
// Led Ambient UUIDS
#define LedAmbientBlink_UUID "fc505047-6455-45d9-8797-2c3a37fd49f6" //
#define LedAmbientPulse_UUID "a987f6ee-681f-49c6-b774-735e513cbd90" //
#define LedRGBValue_UUID "e59d8494-9572-4303-bb38-a768967c54ab"     //
#define LedBrightness_UUID "19b97d7e-7d09-411a-a538-bb92c82ab83e"   //
// PID UUIDS
#define P_UUID "88d4f23a-a9a8-4755-a50e-b9146abef987" //
#define I_UUID "9dfa4f20-eddf-46af-abe6-ea34f63abe4c" //
#define D_UUID "8409fad9-25f0-44ea-81bb-b513b2ad9a1b" //
// Bump UUIDS
#define BumpTemp_UUID "818a9dc8-acf3-49c4-961f-595694893538"   //
#define BumpLength_UUID "79aebb8d-1999-4bf7-a23e-477321387f17" //
// Safety UUIDS
#define AutoOff_UUID "7773076b-df9e-415b-85eb-a46df8d17a61"       //
#define AutoOffLength_UUID "e5b25289-2d75-4708-b33a-7cfccb70576e" //
// Comms UUIDS
#define WifiToggle_UUID "92a3d88b-3dbb-492a-a3a9-254e6791ea1d"  //
#define WifiReset_UUID "bf39b025-5725-4584-863d-ac43313f054f"   //
#define WifiSSID_UUID "2e3e01b7-0ae3-4157-9ec4-c33ecc753b66"    //
#define WifiPass_UUID "8e669865-a858-4375-920f-3c63ed50937e"    //
#define BleEnabled_UUID "7e291ccd-220e-4da6-a19f-595a0631a48f"  //
#define BleMonitor_UUID "6df3562c-027b-43e3-a81e-b00828bd614b"  //
#define UsbEnabled_UUID "f8eabb98-a250-45cf-bc54-468c3236637e"  //
#define UsbDiscover_UUID "d238b387-c24d-4f39-bbd3-3f95985dcb0c" //
#define UsbMonitor_UUID "2f4c315f-f2c4-4bd6-9d36-7bb4aea20ff7"  //
// Fav Temp Settings UUIDS
#define Fav1Name_UUID "d71899f1-08e9-49e1-95b2-4f60c000294a" //
#define Fav1Temp_UUID "c4d2a60a-fdf3-4e09-b0ed-842dc84d9209" //
#define Fav2Name_UUID "d7284ffe-08b2-47ab-8861-860cf3b351f7" //
#define Fav2Temp_UUID "8530ff2e-1707-41d5-af93-a599792690a2" //
#define Fav3Name_UUID "ff4035e1-9352-4f78-bd2d-1ced822c21a3" //
#define Fav3Temp_UUID "cb37d569-20c0-45c5-bca4-faa869a81b7c" //
#define Fav4Name_UUID "bd71f843-3e2f-485d-ae6b-7f241bedfc62" //
#define Fav4Temp_UUID "2a7df77e-5df4-4c66-b03e-41ed031c9146" //
// Device Settings UUIDS
#define Hostname_UUID "cf46fba7-90f0-4702-b9fa-8a20607565fc"             //
#define SystemVersion_UUID "fe2c6985-9e84-431d-8c9c-79a8fd42e380"        //
#define SystemUptime_UUID "c89b18ab-1909-4497-b172-35db5616ef4a"         //
#define SystemTotalUptime_UUID "9ed63576-1e51-4adc-a2ab-97687b7896ff"    //
#define DeviceReboot_UUID "fafc20ff-66da-47e3-87da-dcb7091ee169"         //
#define Device_Factory_Reset_UUID "21b0b1d5-5f8f-40d1-a00c-445b63bf2d47" //

// START BLE VARS
typedef struct btMessage
{
    char tag[32];
    char body[128];
    double temp = 0;
    int adj = 1;
    bool state = false;
} btUpdate;
static const int btQueueLength = 10;
// BT Task-Queue
static QueueHandle_t btQueue = xQueueCreate(btQueueLength, sizeof(btUpdate)); // BLE queue;

//////////////////////////////////
// General BLE Vars
// TODO: Remove as this gets faded to other file.
bool deviceConnected = false;
bool oldDeviceConnected = false;
// Bluetooth Characteristics
BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;
BLECharacteristic *pCharacteristic2 = NULL;
BLECharacteristic *pChar_setTemp = NULL;
BLECharacteristic *pChar_quickAdj = NULL;
BLECharacteristic *pChar_quickAdjAmt = NULL;
BLECharacteristic *pChar_blank = NULL;
// Bluetooth
bool BLU_CONNECTING = false;
bool BLU_CONNECTED = false;
uint16_t blu_connect = 0;
uint16_t ble_tick = 0;
// Bluetooth Characteristic Update Bools
bool BleUP_State = false;
bool BleUP_SetTemp = false;  // should update bluetooth client?
bool BleUP_SetTemp2 = false; // should send and update to web clients?
bool BleUP_BumpTemp = false;
bool BleUP_AdjAmt = false;
bool BleUP_Fav1 = false;
bool BleUP_Fav2 = false;
bool BleUP_Fav3 = false;
bool BleUP_Fav4 = false;
// Bluetooth Characteristic Toggle Bools
bool Ble_Tog_Coil = false;
// END BLE Vars
////////////////////////////////////

void bleUpdateTask(void *pvParameters);

AerManager *_aerManager;

// BLE Callbacks START
class MyCallbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0)
        {
            Serial.println("*********");
            Serial.print("New value: ");
            for (int i = 0; i < value.length(); i++)
                Serial.print(value[i]);

            Serial.println();
            Serial.println("*********");
        }
    }

    void onRead(BLECharacteristic *pCharacteristic)
    {
        Serial.println("********************************************");
        Serial.println("           Coil Toggle Requested");
        Serial.println("********************************************");
        // togglecoilref();
        Ble_Tog_Coil = true;
    }
};

class SetTemp_Callbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        AerPID *aerStorage = _aerManager->getAerPID(0);
        std::string value = pCharacteristic->getValue();

        if (value.length() > 0)
        {
            String st = "";
            Serial.println("*********");
            Serial.print("New Set Temp value: ");
            for (int i = 0; i < value.length(); i++)
            {
                Serial.print(value[i]);
                st += value[i];
            }
            Serial.println();
            Serial.println("*********");
            double t = ::atof(value.c_str());
            Serial.println(t);
            if (aerStorage->SET_TEMP_MAX < 100)
            {
                aerStorage->SET_TEMP_MAX = 850;
                // eepromStor.save_set_temp_max(SET_TEMP_MAX);
                tempStor.setMaxTemp(aerStorage->SET_TEMP_MAX);
            }
            if (t < aerStorage->SET_TEMP_MAX && t > 100)
            {
                aerStorage->SET_TEMP = t;
                BleUP_SetTemp2 = true;
                _aerManager->updateTempStor(0, true);
            }
        }
    }
};

class QuickAdj_Callbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        AerPID *aerStorage = _aerManager->getAerPID(0);
        Serial.println("*********");
        Serial.println("Quick up called");
        Serial.println("*********");
        double t = aerStorage->SET_TEMP + aerStorage->TEMP_ADJ_AMOUNT;
        if (aerStorage->SET_TEMP_MAX < 100)
        {
            aerStorage->SET_TEMP_MAX = 850;
            // eepromStor.save_set_temp_max(SET_TEMP_MAX);
            tempStor.setMaxTemp(aerStorage->SET_TEMP_MAX);
        }
        if (t < aerStorage->SET_TEMP_MAX && t > 100)
        {
            aerStorage->SET_TEMP = t;
            BleUP_SetTemp2 = true;
            _aerManager->updateTempStor(0, true);
        }
    }
    void onRead(BLECharacteristic *pCharacteristic)
    {
        AerPID *aerStorage = _aerManager->getAerPID(0);
        Serial.println("*********");
        Serial.println("Quick down called");
        Serial.println("*********");
        double t = aerStorage->SET_TEMP - aerStorage->TEMP_ADJ_AMOUNT;
        if (aerStorage->SET_TEMP_MAX < 100)
        {
            aerStorage->SET_TEMP_MAX = 850;
            // eepromStor.save_set_temp_max(SET_TEMP_MAX);
            tempStor.setMaxTemp(aerStorage->SET_TEMP_MAX);
        }
        if (t < aerStorage->SET_TEMP_MAX && t > 100)
        {
            aerStorage->SET_TEMP = t;
            BleUP_SetTemp2 = true;
            _aerManager->updateTempStor(0, true);
        }
    }
};

class QuickAdjAmt_Callbacks : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        AerPID *aerStorage = _aerManager->getAerPID(0);
        std::string value = pCharacteristic->getValue();
        double t = ::atof(value.c_str());
        aerStorage->TEMP_ADJ_AMOUNT = t;
        BleUP_AdjAmt = true;
    }
    void onRead(BLECharacteristic *pCharacteristic)
    {
        // TODO: ?? Not sure I will need this one since it will have a notify.
    }
};

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        Serial.println("BlueTooth Connection recieved.");
        deviceConnected = true;
    };

    void onDisconnect(BLEServer *pServer)
    {
        Serial.println("*********");
        deviceConnected = false;
    }
};

class MySecurity : public BLESecurityCallbacks
{

    bool onConfirmPIN(uint32_t pin)
    {
        ESP_LOGI(LOG_TAG, "The passkey YES/NO number:%d", pin);
        vTaskDelay(5000);
        return true;
    }

    uint32_t onPassKeyRequest()
    {
        ESP_LOGI(LOG_TAG, "PassKeyRequest");
        return BLE_PASSKEY;
    }

    void onPassKeyNotify(uint32_t pass_key)
    {
        ESP_LOGI(LOG_TAG, "On passkey Notify number:%d", pass_key);
    }

    bool onSecurityRequest()
    {
        ESP_LOGI(LOG_TAG, "On Security Request");
        return true;
    }

    void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl)
    {
        ESP_LOGI(LOG_TAG, "Starting BLE work!");
        if (cmpl.success)
        {
            uint16_t length;
            esp_ble_gap_get_whitelist_size(&length);
            ESP_LOGD(LOG_TAG, "size: %d", length);
        }
    }
};
// BLE Callbacks END

// ===========================================================================================
// ===========================================================================================

// ===============================================================
// ===============================================================

void bleUpdateTask(void *pvParameters)
{
    AerManager *taskParams = (AerManager *)pvParameters; // task parameters
    // AerGUI *_aerGUI = taskParams->getGUI();
    _aerManager = taskParams;
    AerPID *aerStorage = taskParams->getAerPID(0);

    //
    Serial.println("Starting BLE Update Task.");
    btUpdate rcv_msg;

    BLEDevice::init("AerTiny");
    BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
    BLEDevice::setSecurityCallbacks(new MySecurity());

    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    BLECharacteristic *pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE);

    pCharacteristic->setCallbacks(new MyCallbacks());
    pCharacteristic->setValue("Read to Toggle Coil");
    pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    pCharacteristic2 = pService->createCharacteristic(
        CHARACTERISTIC2_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);
    pCharacteristic2->addDescriptor(new BLE2902());
    String temp = String(aerStorage->AVG_TEMP);
    pCharacteristic2->setValue(temp.c_str());
    pCharacteristic2->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    pChar_setTemp = pService->createCharacteristic(
        CHARACTERISTIC3_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);
    pChar_setTemp->addDescriptor(new BLE2902());
    pChar_setTemp->setValue(String(aerStorage->SET_TEMP).c_str());
    pChar_setTemp->setCallbacks(new SetTemp_Callbacks());
    pChar_setTemp->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    pChar_quickAdj = pService->createCharacteristic(
        QUICKADJU_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);
    pChar_quickAdj->addDescriptor(new BLE2902());
    pChar_quickAdj->setValue("Write Up / Read Down");
    pChar_quickAdj->setCallbacks(new QuickAdj_Callbacks());
    pChar_quickAdj->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    pChar_quickAdjAmt = pService->createCharacteristic(
        QUICKAMT_CHAR_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY |
            BLECharacteristic::PROPERTY_INDICATE);
    pChar_quickAdjAmt->addDescriptor(new BLE2902());
    pChar_quickAdjAmt->setValue(String(aerStorage->TEMP_ADJ_AMOUNT).c_str());
    pChar_quickAdjAmt->setCallbacks(new QuickAdjAmt_Callbacks());
    pChar_quickAdjAmt->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);

    pService->start();

    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    BLESecurity *pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_ONLY);
    pSecurity->setCapability(ESP_IO_CAP_KBDISP); //(ESP_IO_CAP_OUT);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    pSecurity->setStaticPIN(BLE_PASSKEY);

    Serial.println("[AerPID] > Bluetooth LE Setup Complete.");

    while (1)
    {
        // ======================================
        // BLE Updates
        if (BleUP_AdjAmt)
        { // Update BLE Tem_Adj_amount
            BleUP_AdjAmt = false;
            if (deviceConnected)
            {
                pChar_quickAdjAmt->setValue(String(aerStorage->TEMP_ADJ_AMOUNT).c_str());
                pChar_quickAdjAmt->notify();
            }
            // TODO: Update website as well!
            // ws.textAll("setadj:" + String(TEMP_ADJ_AMOUNT));
            vTaskDelay(3 / portTICK_PERIOD_MS); // delay(3);
        }
        if (BleUP_SetTemp)
        {
            // Serial.println("Did it notify?");
            if (deviceConnected)
            {
                pChar_setTemp->setValue(String(aerStorage->SET_TEMP).c_str());
                pChar_setTemp->notify();
            }
            BleUP_SetTemp = false;
            vTaskDelay(3 / portTICK_PERIOD_MS); // delay(3);
        }
        if (BleUP_SetTemp2)
        {
            BleUP_SetTemp2 = false;
            // sendSetTemp(); // FIXME:
        }
        if (deviceConnected && ble_tick > 5)
        {
            String temp = String(aerStorage->SET_TEMP);
            pCharacteristic2->setValue(temp.c_str());
            pCharacteristic2->notify();
            ble_tick = 0;
            vTaskDelay(3 / portTICK_PERIOD_MS); // delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
        }
        // disconnecting
        if (!deviceConnected && oldDeviceConnected)
        {
            vTaskDelay(500 / portTICK_PERIOD_MS); // delay(500);                  // give the bluetooth stack the chance to get things ready
            pServer->startAdvertising();          // restart advertising
            Serial.println("Start BT advertising again.");
            oldDeviceConnected = deviceConnected;
        }
        // connecting
        if (deviceConnected && !oldDeviceConnected)
        {
            // do stuff here on connecting
            oldDeviceConnected = deviceConnected;
        }
        ble_tick++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

// -----------------------------------------------------------------------------
// BlueTooth Update Tasks
// -----------------------------------------------------------------------------

/*void bleSecurity(){
  esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
  esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
  uint8_t key_size = 16;
  uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
  uint32_t passkey = BLE_PASSKEY;
  uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;

  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t));

  esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t));
  esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t));
}*/

#endif