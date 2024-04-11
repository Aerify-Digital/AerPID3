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
#include <PID_v1.h>

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
#define FREQ_PWM_KHz 40
#define PWM_RESOLUTION 10

// PWM output scaling factor
#define PWM_SCALE_FACTOR 0.105
// Output value range limit
#define PID_OUTPUT_LIMIT 255

// *************************************

// Sample Time for PID compute in milliseconds
#define PID_SAMPLE_TIME_MS 150
// Sleep Time for PID thread in milliseconds
#define PID_SLEEP_TIME_MS 50
// Tick time overshoot amount for PID compute
#define PID_TIME_OVERSHOOT 7

// Tick max for PID - sample time scaler
#define PID_TICK_MAX (PID_SAMPLE_TIME_MS / PID_SLEEP_TIME_MS)
// Measure Freqeuncy counter max
#define PID_MEAS_COUNTER 5
// Measure tick max - internal to measure()
#define MEAS_TICK_MAX 7

#define MEAS_TICK_MAX_C ((1000 / PID_SLEEP_TIME_MS) * 3)

// *************************************

// Measure sample array sizes
#define MEASURES_SIZE 64
#define MEASURES_AVG_TOTAL 64 // must be smaller than MEASURES_SIZE
#define MES_TEMP_SIZE 2

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
};

extern AerPID xAerPID1;
#if AERPID_COUNT == 2
extern AerPID xAerPID2;
#endif

#endif
