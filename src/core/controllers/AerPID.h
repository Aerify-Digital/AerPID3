/* =============================================================================
 * Copyright (c) 2015-2025 AerifyDigital LLC.  All rights reserved.
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
#include "sigma.h"

// *************************************

// Toggle verbose debug output.
// Set to 1 to enabled, 0 for disabled.
#define VERBOSE_DEBUG 1
// Show PID function debug output.
#define OUTPUT_DEBUG_PID 0

// *************************************

// Max element temperature (celsius)
#define TEMPERATURE_MAX 510.0
// Temperature offset adjustment
#define TEMPERATURE_OFFSET -0.1

// *************************************

// PWM frequency in Hz for PWM output
#define FREQ_PWM_Hz 200 // default 200hz
// PWM resolution bits for PWM Duty Cycle
// Expected Duty Cycle range is `0` to `2^PWM_RESOLUTION`
#define PWM_RESOLUTION 11

// Output value range limit
#define PID_OUTPUT_LIMIT 2047

// 'PID_OUTPUT_LIMIT' based on 'PWM_RESOLUTION' bit
/* 8 bit  - 256   - 255
 * 9 bit  - 512   - 511
 * 10 bit - 1024  - 1023
 * 11 bit - 2048  - 2047
 * 12 bit - 4096  - 4095
 * 13 bit - 8192  - 8191
 * 14 bit - 16384 - 16383
 */

// *************************************

// PWM output scaling factor (hack)
#define PWM_SCALE_FACTOR 0.995

// Integral WindUp limit (fix for timing windup)
#define PID_WINDUP_LIMIT 1000
// Output Bias for PID function
#define PID_BIAS 0

// *************************************

// Sample Time for PID compute in milliseconds
#define PID_SAMPLE_TIME_MS 500
// Sleep Time for PID thread in milliseconds
#define PID_SLEEP_TIME_MS 25
// Tick time overshoot amount for PID compute
#define PID_TIME_OVERSHOOT 70

// *************************************

// Measure sample array sizes
#define MEASURES_SIZE 128
// Measure rolling average size
#define MEASURES_AVG_TOTAL 40 // must be smaller than MEASURES_SIZE
// Rolling average measure max size
#define MEAS_TICK_MAX_C ((1000 / PID_SLEEP_TIME_MS) * 3)

// Sensor bit resolution - higher value takes longer
#define MEASURE_BIT_PRECISION 11

#if MEASURE_BIT_PRECISION == 9
#define MEASURE_TIME_COST 94
#define MES_TEMP_SIZE 7
#endif
#if MEASURE_BIT_PRECISION == 10
#define MEASURE_TIME_COST 190
#define MES_TEMP_SIZE 5
#endif
#if MEASURE_BIT_PRECISION == 11
#define MEASURE_TIME_COST 375
#define MES_TEMP_SIZE 3
#endif
#if MEASURE_BIT_PRECISION == 12
#define MEASURE_TIME_COST 750
#define MES_TEMP_SIZE 2
#endif

#if MEASURE_BIT_PRECISION <= 8
#error "MEASURE_BIT_PRECISION must be greater than or equal to 9"
#endif
#if MEASURE_BIT_PRECISION >= 13
#error "MEASURE_BIT_PRECISION must be less than or equal to 12"
#endif

// *************************************
// *************************************
// Sensor Setup
// *************************************

// Sensor types...
#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3 // Chip used

// Scratchpad Location
#define SENSOR_CONFIGURATION_LOCATION 4
// Device resolution
#define TEMP_9_BIT 0x1F  //!<  9 bit resolution
#define TEMP_10_BIT 0x3F //!< 10 bit resolution
#define TEMP_11_BIT 0x5F //!< 11 bit resolution
#define TEMP_12_BIT 0x7F //!< 12 bit resolution

// OneWire commands
#define STARTCONVO 0x44      //!< Tells device to take a temperature reading and put it on the
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
#define CONFIGURATION 4   //!< DS18S20: store for CRC. DS18B20 & DS1822: configuration register
#define INTERNAL_BYTE 5   //!< Internal use & CRC
#define COUNT_REMAIN 6    //!< DS18S20: COUNT_REMAIN, DS18B20 & DS1822: store for CRC
#define COUNT_PER_C 7     //!< DS18S20: COUNT_PER_C. DS18B20 & DS1822: store for crc
#define SCRATCHPAD_CRC 8  //!< Scratchpad CRC

// Timeout for measurement fault reset
#define FAULT_TIMEOUT 2000

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

/**
 * AerPID Object Class
 * @brief - This class is used to control the AerPID Temperature Controller Module. 
 * @author squidicuzz
 * @details Functions include: 
 * - Reading of the thermocouple temperature.
 * - Adjustment and setting of the PID tuning values.
 * - Control of the PID function sample timing (bit resolution).
 * - Control of the SSR state (on/off).
 * - Control of the PWM duty cycle.
 * - Control of the PWM frequency.
 */
class AerPID
{
public:
    /**
     * @brief Construct a new AerPID object
     *
     * @param pin OneWire pin
     * @param ssrPin SSR Control pin
     * @param ssrChan SSR PWM Channel
     * @param index Element Index
     */
    AerPID(uint8_t owPin, uint8_t ssrPin, uint8_t ssrChan, uint8_t index = 0);
    // dispose
    ~AerPID() {};

    // *********************************
    // AerPID Initialization
    bool init();

    // *************************
    // Temperature variables  (in celsius)
    double MES_TEMP = 0.0;          // Measured temperature
    double AVG_TEMP = 0.0;          // Averga temperature
    double SET_TEMP = 260.0;        // Set temperature
    double SET_TEMP_OLD = 260.0;    // Set temperature (old)
    double SET_TEMP_LAST = 260.0;   // Set temperature (last)
    double SET_TEMP_BUMP = 37.7778; // Bump set temperature
    // Set temperature max value
    double SET_TEMP_MAX = TEMPERATURE_MAX;
    // Offset temperature
    double OFFSET_TEMP = TEMPERATURE_OFFSET;

    // *************************
    // Setting control variables

    bool pid_saved = true;
    bool pwm_saved = true;
    bool temp_saved = true;
    bool temp_changed = true;

    // Set PID enable/running toggle
    void setPidOn(bool on) { pidEnabled = on; }
    // Get PID enabled/running state
    bool isPidOn() { return pidEnabled; }

    // Usage detection auto off.
    bool USAGE_DETECT = true;
    bool AUTO_OFF_ENB = false;
    long AUTO_OFF_TIME = 900;

    // Temperature Adjustment Amount..
    double TEMP_ADJ_AMOUNT = 1.0;

    // *************************
    // PID variable values
    double kP; // Function P value
    double kI; // Function I value
    double kD; // Function D value

    // *************************
    // Do work tick
    void tick();

    // Measurements array
    double *getMeasures();
    // Measurements array long
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

    // Adjust PID tunings
    void setTunings(bool reset);

    void setAutoTuneActive(bool on) { _autoTuneActive = on; }
    bool isAutoTuneActive() { return _autoTuneActive; }

    // Interal var accessors
    int getPwmFreq();
    void setPwmFreq(int freq);

    double getPwmScaler();
    void setPwmScaler(double factor);

    int getPidTime();
    int getPidTick();
    void setPidTime(int time);

    int getPwmResolution();
    void setPWMResolution(int res);

    int getOutputLimit();
    void setOutputLimit(int limit);

    void setOutputBias(double bias);
    double getOutputBias();

    void setWindupLimit(double limit);
    double getWindupLimit();

    double getOutput();
    double getSigma();

    void setMeasMode(uint8_t mode) { _measMode = mode; }

    bool hasFaultError();
    bool hasFaultErrorAlerted();
    void setFaultErrorAlert(bool alerted);

private:
    // verbose debug output
    bool _verbose_d = VERBOSE_DEBUG == 1;
    // Loaded flag
    bool _loaded = false;

    // 1Wire pin and object
    uint8_t owPin;
    OneWire *oneWire;

    // SSR pin and channel
    uint8_t ssrPin;
    uint8_t ssrChan;
    bool _ledAttached = false;

    // Element number
    uint8_t index;

    /* =========== */
    // PID library Reference
    PID *aPID;

    // PID compute output
    double output = 0;
    double xOutput = 0;

    // State for PID enable/running toggle
    bool pidEnabled = false;

    // PID auto tuning enabled flag
    bool _autoTuneActive = false;

    // PWM power scaler value
    double _pwmScaleFactor = PWM_SCALE_FACTOR;
    // Timing variable used for ledcSetup
    int _pwmFrequency = FREQ_PWM_Hz * 1; // PWM Frequency as hertz
    // bit resolution for PWM output
    int _pwmResolution = PWM_RESOLUTION;

    // PWM cycle time (vTaskDelay)
    int _pidCycleTime = PID_SAMPLE_TIME_MS;
    int _pidTickMax = ((double)_pidCycleTime / PID_SLEEP_TIME_MS) * 0.67;
    // pid output range limit for input to pwm
    int _pidOutputLimit = PID_OUTPUT_LIMIT;

    // Bias power amount to apply to PID function
    double _pidBias = PID_BIAS;

    // kI value wind up limitor
    double windUpLimit = 500;

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

    /* ============================ */
    /* Timers */

    // Last time measurement was taken for short tracking
    long _measLastTime = 0;
    // How often to take measurement
    long _measLastTimeMax = MEASURE_TIME_COST * 2 + 50;
    // Last time measurement was taken for long tracking
    long _measLastTimeLong = 0;
    // Sensor measure time tracker
    long _measureTimeMs = 0;

    // Last PID compute timestamp
    long _lastPidTimeMs = 0;

    /* ============================ */

    // PID compute tick counter
    int16_t _tick = 0;

    /* ============================ */
    /* Function Ticks */

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

    double deltaScaleOutput(const double delta, const double output);

    void updatePWM(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t resolution_bits);
    void updateSampleTime(int pidTickMax);

    /* ============================ */

    // measurement mode
    uint8_t _measMode = 0;

    // Measure fault counters
    uint _faultsTotal = 0;
    uint _faultsRecent = 0;
    long _faultLastTime = 0;
    // Measure fault flags
    uint8_t _faultCodeLast = 0;
    bool _faultError = false;
    bool _faultErrorAlerted = false;

    // Sensor error fault state codes
    typedef enum
    {
        MAX3185X_OK = 0,
        MAX3185X_OPEN_CIRCUIT = 1,
        MAX3185X_SHORT_TO_GND = 2,
        MAX3185X_SHORT_TO_VCC = 3,
        MAX3185X_NO_REPLY = 4,
    } Max3185xState;

    // Measure result codes
    typedef enum
    {
        NACK = 0,
        ACK = 1,
        FAULT = 2,
        PENDING = 3,
    } MeasureResult;

    // Measure Temperature
    MeasureResult measureElementTemperature();
    MeasureResult measureElementTemperatureAsync();
    MeasureResult measureElementTemperatureBlocking();

    // Set sensor bit resolution for measurements
    bool setSensorResolution(uint8_t newResolution);
    void readScratchPad(uint8_t *deviceAddress, uint8_t *scratchPad);

    // Error code lookup for measure
    Max3185xState getMax31855ErrorCode(uint16_t egtPacket);
};

extern AerPID xAerPID1;
#if AERPID_COUNT == 2
extern AerPID xAerPID2;
#endif

#endif
