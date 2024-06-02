/* =============================================================================
 * Main Application Software for 'AerPID-Lite' or the 'AerTiny' device module.
 * Copyright (c) 2015-2024 AerifyDigital LLC.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#include "main.h"

// =======================
// =======================

// =======================
// EEPROM Storage
extEEPROM myEEPROM(kbits_64, 1, 32, EEPROM_ADDRESS);

// Serial communications handler
SerialCom serialComm;

// Manager storage object
AerManager aerManager;

// Network variable storage
NetworkVars netVars;

// Static IP wifi storage
WiFiStorage wifiStorage;

// Control Interface: TFT Screen and Encoder with buttons
AerGUI aerGUI;

// =========================
// HTTP Web Server Object
WebServer webServer(AerSetup::verbose_d, &aerManager);

// =======================
// Misc Variables..
// =======================

// Display Options
boolean SHOW_SPLASH = true; // Show splash screen on loading
uint16_t SCROLL_SPEED = 6;  // Scrolling Speed Adjust

uint16_t running_time = 0;
double RUN_TIME_MAX = 100;

String uptime = "";
uint32_t uptimeTick = 0;

// *******************************************************************************************
// *******************************************************************************************
AppVersion app_version = AppVersion(VER_MAJOR, VER_MINOR, VER_BUILD);
AppVersion web_version = AppVersion(VER_WEB_MAJOR, VER_WEB_MINOR, VER_WEB_BUILD);
String version = app_version.get();
String webui_version = web_version.get();

/**
 * @brief Prints fre mem to the serial console.
 *
 */
void printFreeMem()
{
  if (Serial)
  {
    Serial.print("[init] ");
    Serial.print("heap free: ");
    Serial.print(ESP.getFreeHeap(), 10);
    Serial.println(" bytes");
    delay(10);
  }
}

// ===========================================================================================
// ===========================================================================================
// SETUP
// ===========================================================================================
void setup()
{
  // CPU Freq: 240 mhz
  setCpuFrequencyMhz(240);

  pinMode(PIN_LED_C, OUTPUT);    // BLUE
  digitalWrite(PIN_LED_C, HIGH); // ENABLE
  pinMode(PIN_LED_B, OUTPUT);    // RED or ORANGE
  digitalWrite(PIN_LED_B, HIGH); // ENABLE

  pinMode(PIN_EEPROM_EN, OUTPUT);
  digitalWrite(PIN_EEPROM_EN, HIGH);

  delay(50);

  // ----------------------------
  // Begin Serial handler (and debug).
  serialComm = SerialCom();
  serialComm.init();
  delay(50);

  Serial.println(F(" "));
  Serial.println(F("System Starting ..."));
  Serial.print(F(">> Version "));
  Serial.println(app_version.get());
  printFreeMem();

  // ----------------------------
  // Begin i2c Master
  Wire.begin();
  delay(50);

  if (AerSetup::verbose_d)
  {
    delay(200);
    Serial.println(F("> I2C scanner. Scanning ..."));
    byte count = 0;
    for (byte i = 5; i < 120; i++)
    {
      Wire.beginTransmission(i);
      if (Wire.endTransmission() == 0)
      {
        Serial.print(F(">> Found address: "));
        Serial.print(i, DEC);
        Serial.print(F(" (0x"));
        Serial.print(i, HEX);
        Serial.print(F(")"));
        if (i == 7)
          Serial.print(F(" [ATTINY88-x1]"));
        if (i == 8)
          Serial.print(F(" [ATTINY88-x0]"));
        if (i == 9)
          Serial.print(F(" [ATTINY88-x2]"));
        if (i == 45)
          Serial.print(F(" [ATTINY88-EP]"));
        if (i == 60)
          Serial.print(F(" [SCREEN]"));
        if (i == 63)
          Serial.print(F(" [SCREEN]"));
        if (i == 72)
          Serial.print(F(" [LM75B-x1]"));
        if (i == 73)
          Serial.print(F(" [LM75B-x2]"));
        if (i == 80)
          Serial.print(F(" [EEPROM-x0]"));
        if (i == 81)
          Serial.print(F(" [RTC]"));
        if (i == 84)
          Serial.print(F(" [EEPROM-x1]"));
        Serial.println(F(" "));
        count++;
        delay(1); // maybe unneeded?
      } // end of good response
    } // end of for loop
    Serial.println(F("> Done."));
    Serial.print(F(">> Found "));
    Serial.print(count, DEC);
    Serial.println(F(" device(s)."));
  }

  delay(10);

  byte i2cStat = myEEPROM.begin(myEEPROM.twiClock400kHz);
  if (i2cStat != 0 && i2cStat != 2)
  {
    Serial.print(F("I2C Problem! ["));
    Serial.print(i2cStat);
    Serial.println(F("]"));
    Serial.println(F(" "));
    delay(3000);
  }
  else
  {
    Serial.print(F("I2C EEPROM Enabled! ["));
    Serial.print(i2cStat);
    Serial.print(F("]  Size: "));
    Serial.print(myEEPROM.length());
    Serial.print(F(" bits"));
    Serial.print(F("  "));
    Serial.print(myEEPROM.length() / 1024 / 8);
    Serial.print(F(" kB"));
    Serial.println(F(" "));
    delay(50);
  }

  // init storage object
  netVars = NetworkVars();
#if AERPID_COUNT == 2
  aerManager = AerManager(&xAerPID1, &xAerPID2, &netVars);
#else
  aerManager = AerManager(&xAerPID1, &netVars);
#endif

  // init the screen and gui
  aerGUI = AerGUI(true);
  aerManager.setGUI(&aerGUI);

  // setup version
  aerManager.setVersionApp(&app_version);
  aerManager.setVersionWeb(&web_version);
  // setup screen and show app version
  aerGUI.getST7789()->setupISP();
  aerGUI.getST7789()->showVersion(app_version.get());
  delay(250);

  printFreeMem();

  // Init the EEPROM storage
  eepromStor.init(&myEEPROM, PIN_EEPROM_EN);

  if (SHOW_SPLASH)
  {
    // Load the logo splash image
    aerGUI.getST7789()->showSplashInit();
    delay(620);
  }

  printFreeMem();

  // ----------------------------
  aerGUI.getST7789()->showLoadingStorMessage();
  // Initialize the Flash storage
  flash.init(PIN_FLSH_CS);
  Serial.println(F("[BOOT] Initializing Config Storage (config) "));
  config.init(&flash, &eepromStor);
  char buf[16];
  version.toCharArray(buf, 16);
  config.setup(buf);
  Serial.println(F("[BOOT] Initializing Crypto Storage (crypto)  "));
  // auth.init(&flash);
  crypto.init(&flash);
  Serial.println(F("[BOOT] Initializing PID Storage (pidStor)  "));
  pidStor.init(&flash);
  Serial.println(F("[BOOT] Initializing PWM Storage (pwmStor)  "));
  pwmStor.init(&flash);
  Serial.println(F("[BOOT] Initializing Temperate Storage (tempStor)  "));
  tempStor.init(&flash);
  Serial.println(F("[BOOT] Initializing Bump Storage (bumpStor)  "));
  bumpStor.init(&flash);
  Serial.println(F("[BOOT] Initializing Favorites Storage (favstor)  "));
  favstor.init(&flash);
  Serial.println(F("[BOOT] Initializing Communcations Storage (commstor)  "));
  commstor.init(&flash);
  Serial.println(F("[BOOT] Initializing Lighting Storage (lightstor)  "));
  lightstor.init(&flash);
  Serial.println(F("[BOOT] Initializing Settings Storage (settingsStorage)  "));
  settingsStorage.init(&flash);
  Serial.println(F("[BOOT] Initializing Network Storage (networkStorage)  "));
  networkingStorage.init(&flash);
  Serial.println(F("[BOOT] Initializing WiFi Storage (wifiStorage)  "));
  wifiStorage.init(&flash);
  Serial.println(F("[BOOT] Initializing Charting Storage (chartingStorage)  "));
  chartingStorage.init(&flash);
  Serial.println(F("[BOOT] Initialized Storage Objects!"));
  Serial.println(F(" ... "));
  delay(50);

  // setup pointer link to managers...
  aerManager.setupStorageObjects(&favstor, &commstor, &lightstor, &bumpStor, &tempStor, &settingsStorage, &networkingStorage);
  Serial.println(F(" ... "));

  printFreeMem();

  // ----------------------------
  aerGUI.getST7789()->showLoadingAppMessage();
  delay(500);

  // Show loading screen now
  aerGUI.getST7789()->showSplashPost();

  Serial.println(F(" "));
  digitalWrite(PIN_LED_C, LOW);
  digitalWrite(PIN_LED_B, LOW);
  delay(200);
  Serial.println(F("[init] Completed Setup."));
  Serial.println(F(" "));

  printFreeMem();
  delay(100);

  // =============================

  // Setup mutex locks
  i2c1_mutex = xSemaphoreCreateMutex();
  spi1_mutex = xSemaphoreCreateMutex();
  sys1_mutex = xSemaphoreCreateMutex();

  _initThreadStackSizes();
  uint32_t stackTotal = 0;
  for (int t = 0; t < taskCount; t++)
  {
    stackTotal += taskStackSize[t] * 4;
  }
  Serial.print("[init] projected task stack size max: ");
  Serial.print(stackTotal, 10);
  Serial.println(" bytes\n");
  delay(20);

  printFreeMem();
  Serial.println(F("[init] Starting task threads...\n\n"));
  delay(100);

  // =============================
  // Setup Tasks

  // performance monitor task
  perfmon_start(&aerManager, taskStackSize[9]);

  if (commstor.getBTEn())
  { // Bluetooth Task
    // FIXME: brokey.
    // NOT USED!!!
    // xTaskCreatePinnedToCore(bleUpdateTask, "BleTask", taskStackSize[0], (void *)&aerManager, 8, &btTask, 0);
  }

  // Encoder knob task handler
  xTaskCreatePinnedToCore(enc_task, "Encoder_Task", taskStackSize[1], (void *)&aerManager, 1, &encTask, 1);

  // Link task for element monitor
  xTaskCreatePinnedToCore(link_task, "Link_Task", taskStackSize[3], (void *)&aerManager, 5, &linkTask, 0);

  // Worker task for generics. Runs RTC and Onboard Temperature sensors.
  xTaskCreatePinnedToCore(worker_task, "Worker_Task", taskStackSize[2], (void *)&aerManager, 6, &wrkTask, 1);

  // PID task
  // xTaskCreatePinnedToCore(pid_task, "PID_Task", taskStackSize[5], NULL, 2, &pidTask, 1);
  xTaskCreatePinnedToCore(pid_task_1, "PID_Task_1", taskStackSize[5], NULL, 2, &pidTask1, 1);
#if AERPID_COUNT == 2
  xTaskCreatePinnedToCore(pid_task_2, "PID_Task_2", taskStackSize[5], NULL, 2, &pidTask2, 1);
#endif

  xTaskCreatePinnedToCore(element_task, "Element_Task", taskStackSize[12], (void *)&aerManager, 8, &elementTask, 0);

  if (commstor.getUSBEn())
  {
    SerialTaskParams serialTaskParams;
    serialTaskParams.am = &aerManager;
    serialTaskParams.sc = &serialComm;
    // Serial communications handler
    xTaskCreatePinnedToCore(serial_task, "Serial_Task", taskStackSize[6], (void *)&serialTaskParams, 5, &serialTask, 1);
  }

  // Screen UI task handler
  xTaskCreatePinnedToCore(tft_task, "TFT_Task", taskStackSize[7], (void *)&aerManager, 4, &tftTask, 1);
  // Fancy LED task.
  xTaskCreatePinnedToCore(led_task, "LED_Task", taskStackSize[8], (void *)&aerManager, 7, &ledTask, 1);

  if (commstor.getWifiEn())
  {
    // web server task
    xTaskCreatePinnedToCore(taskWebWorker, "WebServer_Task", taskStackSize[10], NULL, 5, &webTask, 1);
  }

  SaveTaskParams saveTaskParams;
  saveTaskParams.am = &aerManager;
  saveTaskParams.flash = &flash;
  saveTaskParams.eeprom = &eepromStor;
  // storage saves task
  xTaskCreatePinnedToCore(save_task, "Save_Task", taskStackSize[11], (void *)&saveTaskParams, 9, &saveTask, 1);

  delay(100);
  printFreeMem();
}

// ===========================================================================================
// Thread start managers...

void startSerialTask()
{
  if (serialTask == nullptr)
  {
    SerialTaskParams serialTaskParams;
    serialTaskParams.am = &aerManager;
    serialTaskParams.sc = &serialComm;
    xTaskCreatePinnedToCore(serial_task, "Serial_Task", taskStackSize[6], (void *)&serialTaskParams, 5, &serialTask, 1);
  }
}

void stopTftDemoTask()
{
  if (demoTask != NULL)
  {
    Serial.println("Aborting tft Demo Task...");
    vTaskDelete(demoTask);
    dispose_starfieldData();
    demoTask = NULL;
  }
}

void startTftDemoTask()
{
  eTaskState xTaskState = eTaskState::eDeleted;
  if (demoTask != NULL)
  {
    xTaskState = eTaskGetState(&demoTask);
  }
  if (xTaskState == eTaskState::eDeleted)
  {
    Serial.println("Starting tft Demo Task...");
    xTaskCreatePinnedToCore(tft_demo, "TFT_Demo", taskStackSize[4], (void *)&aerManager, 10, &demoTask, 1);
  }
}

// ===========================================================================================
// ===========================================================================================
// LOOP
// ===========================================================================================

/**
 * @brief The main loop.
 * Try to avoid putting work here...
 *
 */
void loop(void)
{
  tickMainLoop();
  tickSystemCounters();

  // =======================================
  // Status led twinkle...
  // =======================================
  digitalWrite(PIN_LED_C, LOW);
  vTaskDelay(1);
  if (xSemaphoreTake(sys1_mutex, 1) == pdTRUE)
  {
    digitalWrite(PIN_LED_C, HIGH);
    xSemaphoreGive(sys1_mutex);
  }
  vTaskDelay(1);
}

// ===========================================================================================
// ===========================================================================================

/**
 * @brief main tick...
 * Try to avoid putting much work here?
 *
 */
void tickMainLoop()
{
  // =========================================================================================
  // TODO: Move this? I might not have too.
  // TODO: Lets move this to the BLE task??
  // =========================================================================================
  /*if (Ble_Tog_Coil)
  {
    Ble_Tog_Coil = false;
    toggleCoil();
  }*/

  /*if (Serial && serialTask == nullptr)
  {
    startSerialTask();
  }*/
  if (aerManager.isScreenSaverActive())
  {
    startTftDemoTask();
  }
  else if (!aerManager.isScreenSaverActive())
  {
    stopTftDemoTask();
  }
}

/**
 * @brief Ticks system counters, uptime.
 *
 */
void tickSystemCounters()
{
  if (uptimeTick > 0)
  {
    uptimeTick--;
  }
  if (uptimeTick == 0)
  {
    uptimeTick = 500;
    sysTime();
  }
}

void sysTime()
{
  long millisecs = millis();
  int systemUpTimeMn = int((millisecs / (1000 * 60)) % 60);
  int systemUpTimeHr = int((millisecs / (1000 * 60 * 60)) % 24);
  int systemUpTimeDy = int((millisecs / (1000 * 60 * 60 * 24)) % 365);
  uptime = String(systemUpTimeDy) + " Days " + String(systemUpTimeHr) + " Hrs " + String(systemUpTimeMn) + " Mins";
  // Update Web-Clients
}
