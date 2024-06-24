/* =============================================================================
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

#ifndef _AERPID_H
#define _AERPID_H

#include <OneWire.h>
#include <ArduPID.h>
// Define pointer ref to ArduPID
#define PID ArduPID

#include <PinDefs.h>
#include "storage/chartsStor.h"

// *************************************
// Toggle verbose debug output.
// Set to 1 to enabled, 0 for disabled.
#define VERBOSE_DEBUG 1

// *************************************

// Max element temperature (celsius)
#define TEMPERATURE_MAX 454.444
// Temperature offset adjustment
#define TEMPERATURE_OFFSET -5.0

// *************************************

// PWM frequency and resolution
#define FREQ_PWM_KHz 1
#define PWM_RESOLUTION 14

/*
8 bit - 256
9 bit - 512
10 bit - 1024
11 bit - 2048
12 bit - 4096
13 bit - 8192
14 bit - 16384
*/

// PWM output scaling factor
#define PWM_SCALE_FACTOR 0.551
// Output value range limit
#define PID_OUTPUT_LIMIT 16383

// Integral WindUp fix limit
#define PID_WINDUP_LIMIT 5
// Output Bias for PID function
#define PID_BIAS 13

// *************************************

// Sample Time for PID compute in milliseconds
#define PID_SAMPLE_TIME_MS 100
// Sleep Time for PID thread in milliseconds
#define PID_SLEEP_TIME_MS 50
// Tick time overshoot amount for PID compute
#define PID_TIME_OVERSHOOT 7

// Tick max for PID - sample time scaler
#define PID_TICK_MAX (PID_SAMPLE_TIME_MS / PID_SLEEP_TIME_MS)
// Measure Freqeuncy counter max
#define PID_MEAS_COUNTER 2
// Measure tick max - internal to measure()
#define MEAS_TICK_MAX 3

#define MEAS_TICK_MAX_C ((1000 / PID_SLEEP_TIME_MS) * 3)

// *************************************

// set sensor bit resolution
#define MEASURE_BIT_PRECISION 11

// Measure sample array sizes
#define MEASURES_SIZE 64
#define MEASURES_AVG_TOTAL 64 // must be smaller than MEASURES_SIZE
#define MES_TEMP_SIZE 3

// *************************************
// *************************************

// Flags size for variable cache saves
#define SAVE_VAR_SIZE 11

// LED debug pin
#ifdef PIN_LED_B
#define PIN_LED_ACT PIN_LED_B
#else
#define PIN_LED_ACT 12
#endif

// *************************************
// *************************************
// Sensor Setup
// *************************************

// Scratchpad Location
#define SENSOR_CONFIGURATION_LOCATION 4
// Device resolution
#define TEMP_9_BIT 0x1F  //!<  9 bit resolution
#define TEMP_10_BIT 0x3F //!< 10 bit resolution
#define TEMP_11_BIT 0x5F //!< 11 bit resolution
#define TEMP_12_BIT 0x7F //!< 12 bit resolution

// OneWire commands
#define STARTCONVO \
    0x44                     //!< Tells device to take a temperature reading and put it on the
                             //!< scratchpad
#define COPYSCRATCH 0x48     //!< Copy EEPROM
#define READSCRATCH 0xBE     //!< Read EEPROM
#define WRITESCRATCH 0x4E    //!< Write to EEPROM
#define RECALLSCRATCH 0xB8   //!< Reload from last known
#define READPOWERSUPPLY 0xB4 //!< Determine if device needs parasite power
#define ALARMSEARCH 0xEC     //!< Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB 0        //!< Temperature LSB byte location
#define TEMP_MSB 1        //!< Temperature MSB byte location
#define HIGH_ALARM_TEMP 2 //!< High alarm temp byte location
#define LOW_ALARM_TEMP 3  //!< Low alarm temp byte location
#define CONFIGURATION \
    4                   //!< DS18S20: store for CRC. DS18B20 & DS1822: configuration register
#define INTERNAL_BYTE 5 //!< Internal use & CRC
#define COUNT_REMAIN \
    6                    //!< DS18S20: COUNT_REMAIN, DS18B20 & DS1822: store for CRC
#define COUNT_PER_C 7    //!< DS18S20: COUNT_PER_C. DS18B20 & DS1822: store for crc
#define SCRATCHPAD_CRC 8 //!< Scratchpad CRC

// *************************************
// *************************************

// AerPID - PID Functions
class AerPID
{
public:
    // Constructor
    AerPID(uint8_t owPin, uint8_t ssrPin, uint8_t ssrChan, uint8_t index = 0);
    ~AerPID(){};

    // *********************************
    // AerPID Initialization
    bool init();

    // *************************
    // Update triggers..
    void doUpdate();
    bool hasUpdated();
    bool needsUpdate(uint8_t i);

    // Saving
    void doSave(uint8_t i);
    bool needSave();
    bool needSave(uint8_t i);

    // *************************
    // Measure temperature var  (in celsius)
    double MES_TEMP = 0.0;
    double AVG_TEMP = 0.0;
    // Set temperature vars
    double SET_TEMP = 260.0;
    double SET_TEMP_OLD = 260.0;
    double SET_TEMP_LAST = 260.0;
    double SET_TEMP_BUMP = 37.7778;
    double SET_TEMP_MAX = TEMPERATURE_MAX;
    // Offset temperature
    double OFFSET_TEMP = TEMPERATURE_OFFSET;

    // *************************
    // Setting control variables

    bool pid_saved = true;
    bool pwm_saved = true;
    bool temp_saved = true;
    bool temp_changed = true;

    // State for PID enable/running toggle
    bool PID_ON = false;

    // Usage detection auto off..
    bool USAGE_DETECT = true;
    bool AUTO_OFF_ENB = false;
    long AUTO_OFF_TIME = 900;

    // Temperature Adjustment Amount..
    double TEMP_ADJ_AMOUNT = 1.0;

    // *************************
    // PID variable values
    double kP;
    double kI;
    double kD;

    // PID auto tuning enabled flag
    boolean AUTO_TUNE_ACTIVE = false;

    // *************************

    // PWM scaler value
    double PWM_ScaleFactor = PWM_SCALE_FACTOR;
    // PWM cycle time (vTaskDelay)
    int PWM_CycleTime = PID_SAMPLE_TIME_MS;

    // *************************
    // Do work tick
    void tick();

    // get measures array
    double *getMeasures();
    double *getMeasuresLong();

    // Average mesasures
    double avgMeasures();
    // Max measured value
    double maxMeasures();
    // Max temperature
    double getTempMax();

    double minMeasuresLong();
    double maxMeasuresLong();
    double avgMeasuresLong();

    // Adjust PID tunings..
    void setTunings();

    // Interal var accessors
    int getPwmFreq()
    {
        return _freq_PWN;
    }
    void setPwmFreq(int freq)
    {
        _freq_PWN = freq;
    }

private:
    // verbose debug output
    bool _verbose_d = VERBOSE_DEBUG == 1;

    uint8_t ow_pin;
    OneWire *oneWire;

    uint8_t ssrPin;
    uint8_t ssrChan;

    uint8_t index;

    // PID library
    PID *aPID;

    // Loaded flag
    bool _loaded = false;
    // has updated
    bool _needsUpdate = false;
    // needs save
    bool _needsSave[SAVE_VAR_SIZE];

    // PID compute output
    double output = 0;

    // Timing vars for ledcSetup
    int _freq_PWN = FREQ_PWM_KHz * 1000;
    int _resolution_bits = PWM_RESOLUTION;

    // Array of most recent measurements
    double aMeasuresArr[MES_TEMP_SIZE];
    // Array of recent measurements
    double bMeasuresArr[MEASURES_SIZE];
    // Array of measurements
    double cMeasuresArr[MEASURES_SIZE];

    // Last temperature reading
    double last_temp = 0.0;

    // Running time values
    uint16_t run_time = 0;
    long running_time = 0;
    unsigned long timed_start = 0;
    bool timed_running = false;

    // Auto shutoff PID running activate
    bool auto_off_active = false;

    /* Counters */

    int16_t meas_counter = 0;
    int16_t meas_ticker = 8;
    int16_t meas_ticker_c = 0;
    uint16_t meas_ticker_max = MEAS_TICK_MAX;
    uint16_t sys_temp_counter = 0;

    int16_t _tick = 0;

    long _last_time_ms = 0;

    // Compute function
    bool compute();

    // tick worker
    void handleFeatureSetTick();

    // Internal Measures
    double avgMES_TEMP();
    void addToMES_TEMP(double toAdd);
    // External Measures
    void addToMeasuresB(double toAdd);
    void addToMeasuresC(double toAdd);

    // Measure Temperature
    bool measureElementTemperature();

    bool setSensorResolution(uint8_t newResolution);
    void readScratchPad(uint8_t *deviceAddress, uint8_t *scratchPad);
};

extern AerPID xAerPID1;
#if AERPID_COUNT == 2
extern AerPID xAerPID2;
#endif

#endif
