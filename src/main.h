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

#include <setup.h>
#include <PinDefs.h>

#include <Wire.h>
#include "FastLED.h"
#include "SPIFFS.h"
#include "extEEPROM.h"

#include "util/uuid.h"
#include "util/splitter.h"
#include "core/controllers/AerPID.h"
#include "core/controllers/AerManager.h"
#include "common/datatypes/NetworkVars.h"
#include "common/datatypes/PropsMenu.h"

// #include "screen/st7789.h" // SPI Screen
// #include "screen/i2c/oled.h"   // i2c Screen
// #include "screen/i2c/display.h"

#include "storage/eepromStor.h"
#include "storage/flash.h"
#include "storage/auth.h"
#include "storage/crypt.h"
#include "storage/config.h"
#include "storage/pidStor.h"
#include "storage/pwmStor.h"
#include "storage/favsStor.h"
#include "storage/commStor.h"
#include "storage/tempStor.h"
#include "storage/bumpStor.h"
#include "storage/lights.h"
#include "storage/networkStor.h"
#include "storage/uiSettingsStor.h"
#include "storage/chartsStor.h"

#include "screen/demo/TFT_demo.h"
#include "core/lighting/FancyLED.h"

#include "serial/serial_com.h"
#include "tasks/handles/TaskHandles.h"

#include "tasks/monitor/perfmon.h"
#include "tasks/workerTask.h"
#include "tasks/pidTask.h"
#include "tasks/elementTask.h"
#include "tasks/serialTask.h"
#include "tasks/tftTask.h"
#include "tasks/encTask.h"
#include "tasks/linkTask.h"
#include "tasks/ledTask.h"
#include "tasks/webServerTask.h"
#include "tasks/saveTask.h"

// #include "tasks/bleTask.h"

// BLE Imports
/*#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"*/
// =======================
// Bluetooth LE Objects
// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
// #include "tasks/bleTask.h"

// ===============================================================
// ===============================================================

// AerPID Class Object
AerPID xAerPID1 = AerPID(PIN_ONE_WIRE_A, PIN_SSR_EN_A, PIN_SSR_CHAN_1, 0);
#if AERPID_COUNT == 2
AerPID xAerPID2 = AerPID(PIN_ONE_WIRE_B, PIN_SSR_EN_B, PIN_SSR_CHAN_2, 1);
#endif

// ===============================================================
// ===============================================================
// Thread memory stack allocations

// Max task count
static const uint taskCount = 13; // count of tasks

// Task stack sizes.  Controls how much heap is allocated per thread.
// careful changing these sizes...
static uint32_t taskStackSize[taskCount] = {};
void _initThreadStackSizes()
{
    // heap space is of 32bit words in RAM...
    // each 32bit allocation is 4 bytes.
    // (word_size) * (word_count * multiplier) = heap_space

    taskStackSize[0] = 32 * 32 * 1;  // BleTask (not used)
    taskStackSize[1] = 32 * 32 * 7;  // Encoder_Task
    taskStackSize[2] = 32 * 32 * 2;  // Worker Task - RTC_Task & Local_Temp_Task
    taskStackSize[3] = 32 * 32 * 2;  // Link Task for element
    taskStackSize[4] = 32 * 32 * 5;  // TFT_Demo
    taskStackSize[5] = 32 * 32 * 3;  // PID_Task
    taskStackSize[6] = 32 * 32 * 6;  // Serial_Task
    taskStackSize[7] = 32 * 32 * 4;  // TFT_Task
    taskStackSize[8] = 32 * 32 * 2;  // LED_Task
    taskStackSize[9] = 32 * 32 * 1;  // perfmon_task
    taskStackSize[10] = 32 * 32 * 4; // webServer (wifi) Task
    taskStackSize[11] = 32 * 32 * 3; // Storage Variable Saving Task
    taskStackSize[12] = 32 * 32 * 2; // Element Feature Task (bump)
}

// ===============================================================
// main function definitions

void startSerialTask();
void startTftDemoTask();

void tickSystemCounters();
void tickMainLoop();

void sysTime();
