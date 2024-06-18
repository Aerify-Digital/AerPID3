#include "AerPID.h"

// =======================
// One Wire
// OneWire oneWire(PIN_ONE_WIRE);

/**
 * @brief Construct a new AerPID object
 *
 * @param pin OneWire pin
 * @param ssrPin SSR Control pin
 * @param ssrChan SSR PWM Channel
 * @param index Element Index
 */
AerPID::AerPID(u_int8_t pin, uint8_t ssrPin, uint8_t ssrChan, uint8_t index)
{
    Serial.print(F("> "));
    Serial.print(F("Creating new instance of "));
    Serial.print(F("AerPID..."));

    this->index = index;
    this->ssrPin = ssrPin;
    this->ssrChan = ssrChan;
    this->ow_pin = pin;

    // =======================
    // PID Setup Instance
    aPID = new PID();
    aPID->begin(&MES_TEMP, &output, &SET_TEMP, kP, kI, kD);
    Serial.println(F(" ok!"));
};

bool AerPID::init()
{
    Serial.println(F("-------------------------------"));
    Serial.print(F(">> "));
    Serial.print(F("Initializing AerPID "));
    Serial.print(index);
    Serial.print(F(" "));
    Serial.print(F("..."));

    // Singelton Instance check.
    if (_loaded)
    {
        Serial.println();
        Serial.print(F("[WARNING]"));
        Serial.print(F(" Already Initialized "));
        Serial.println(F("AerPID..."));
        Serial.println(F("..."));
        return false;
    }

    // initialize 1wire bus
    oneWire = new OneWire(ow_pin);

    // set sensor bit resolution
    setSensorResolution(MEASURE_BIT_PRECISION);

    Serial.println(F(" ok!"));

    // =======================
    // Setup SSR Pin Control
    double freq_PWN = ledcSetup(ssrChan, _freq_PWN, _resolution_bits);
    ledcAttachPin(ssrPin, ssrChan);

    if (_verbose_d)
    {
        Serial.print(F(">> "));
        Serial.print(F("["));
        Serial.print(index);
        Serial.print(F("]"));
        Serial.print(F(" "));
        Serial.print(F("Attached PWM control to Pin "));
        Serial.print(F("'"));
        Serial.print(ssrPin);
        Serial.print(F("'"));
        Serial.print(F(" on Channel "));
        Serial.print(F("'"));
        Serial.print(ssrChan);
        Serial.print(F("'"));
        Serial.print(F(" at frequency of: "));
        Serial.print(freq_PWN, 0);
        Serial.print(F(" hz"));
        Serial.print(F("  with: "));
        Serial.print(_resolution_bits);
        Serial.print(F(" resolution bits"));
        Serial.println(F("!"));
    }

    // =======================
    // Setup PID timing mode
    int sampleTime = (PID_TIME_OVERSHOOT + PID_SLEEP_TIME_MS) * PID_TICK_MAX; // Default: 100
    aPID->setSampleTime(sampleTime);

    if (_verbose_d)
    {
        Serial.print(F(">> "));
        Serial.print(F("["));
        Serial.print(index);
        Serial.print(F("]"));
        Serial.print(F(" "));
        Serial.print(F("PID sampling time: "));
        Serial.print(sampleTime);
        Serial.print(F(" ms"));
        Serial.print(F(" "));
        Serial.print(F("and tolerance of "));
        Serial.print(PID_TIME_OVERSHOOT);
        Serial.print(F(" ms"));
        Serial.println(F("!"));
    }

    // =======================
    // Set initial PID tunings
    aPID->setCoefficients(kP, kI, kD);
    aPID->setOutputLimits(0, PID_OUTPUT_LIMIT);
    aPID->setBias(PID_BIAS);
    aPID->setWindUpLimits(-PID_WINDUP_LIMIT, PID_WINDUP_LIMIT);

    // start the PID function
    aPID->start();

    // Setup storage arrays..
    for (int i = 0; i < MES_TEMP_SIZE; i++)
    {
        aMeasuresArr[i] = 0;
    }
    for (int i = 0; i < MEASURES_SIZE; i++)
    {
        bMeasuresArr[i] = 0;
    }
    for (int i = 0; i < MEASURES_SIZE; i++)
    {
        cMeasuresArr[i] = 0;
    }
    for (int i = 0; i < SAVE_VAR_SIZE; i++)
    {
        _needsSave[i] = false;
    }

    // Set loaded okay!
    _loaded = true;

    if (_verbose_d)
    {
        Serial.print(F("Loaded "));
        Serial.print(F("["));
        Serial.print(index);
        Serial.print(F("]"));
        Serial.print(F(" "));
        Serial.print(F("kP= "));
        Serial.print(kP);
        Serial.print(F(" "));
        Serial.print(F("kI= "));
        Serial.print(kI);
        Serial.print(F(" "));
        Serial.print(F("kD= "));
        Serial.print(kD);
        Serial.print(F(" "));
        Serial.println(F("!"));
    }
    Serial.print(F("AerPID "));
    Serial.print(index);
    Serial.println(F(" Loaded!"));
    Serial.println(F("-------------------------------"));
    return _loaded;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Has updated!
bool AerPID::hasUpdated()
{
    return _needsUpdate;
}

// Needs update vars check
bool AerPID::needsUpdate(uint8_t i)
{
    if (i > SAVE_VAR_SIZE - 1)
    {
        return _needsUpdate;
    }
    return (_needsUpdate = true) && (_needsSave[i] = true);
}

// Do update for save
void AerPID::doUpdate()
{
    _needsUpdate = false;
}

// Needs save!
bool AerPID::needSave()
{
    bool ns = false;
    for (uint8_t i = 0; i < SAVE_VAR_SIZE; i++)
    {
        if (i <= 1)
        {
            continue;
        }
        if (this->needSave(i))
        {
            ns = true;
            break;
        }
    }
    return ns;
}

// check if need save by index
bool AerPID::needSave(uint8_t i)
{
    if (i > SAVE_VAR_SIZE - 1)
    {
        return false;
    }
    return _needsSave[i];
}

// do save by index
void AerPID::doSave(uint8_t i)
{
    if (i > SAVE_VAR_SIZE - 1)
    {
        return;
    }
    _needsSave[i] = false;
}

// -----------------------------------------------------------------------------

void AerPID::handleFeatureSetTick()
{
    if (PID_ON && millis() - _last_time_ms >= 1000)
    {
        running_time++;
        if (AUTO_OFF_ENB)
        {
            run_time++;
            if (run_time > AUTO_OFF_TIME)
            {
                run_time = 0;
                PID_ON = false;
                Serial.print("PID ");
                Serial.print(F("["));
                Serial.print(index);
                Serial.print(F("]"));
                Serial.println(" Disabled due to timeout!");
            }
        }
        _last_time_ms = millis();
    }
}

// -----------------------------------------------------------------------------

// Main tick
void AerPID::tick()
{
    // tick measure counter
    if (meas_counter-- <= 0)
    {
        // Perform element measurements...
        if (measureElementTemperature())
        {
            meas_counter = PID_MEAS_COUNTER;
            MES_TEMP = avgMES_TEMP();
            addToMeasuresB(MES_TEMP);
        }
        AVG_TEMP = avgMeasures();
    }
    if (meas_ticker_c-- <= 0)
    {
        meas_ticker_c = MEAS_TICK_MAX_C;
        addToMeasuresC(MES_TEMP);
    }
    // tick pid compute
    if (_tick-- <= 0)
    {
        //  Process PID computation
        if (compute())
        {
            // reset pid tick
            _tick = PID_TICK_MAX;
        }
        else
        {
            _tick = 3;
        }
    }

    // timer trigger var
    if (!timed_running)
    {
        timed_running = true;
        timed_start = millis();
        if (PID_ON)
        {
            // status indicator led
            digitalWrite(PIN_LED_ACT, HIGH);
        }
    }

    handleFeatureSetTick();

    vTaskDelay(PID_SLEEP_TIME_MS / portTICK_PERIOD_MS);
}

// --------------------------------------
// PID Compute Function
// --------------------------------------

/**
 * @brief This, as they say, is where the magic happens...
 * This computes the PID output and will set the PWM for the SSR Iutput pin, as well as status pin.
 * This function should be called every time a "void loop()" process executes.
 * The function will decide for itself whether a new pid output needs to be computed.
 *
 * @returns true when the output is computed, false when nothing has been done.
 */
bool AerPID::compute()
{
    // check if PID functions are active...
    if (!PID_ON) // output disabled
    {
        digitalWrite(PIN_LED_ACT, LOW);
        ledcWrite(ssrChan, 0); // output pin off
        timed_start = millis();
        run_time = 0;
        return false;
    }
    // disable if above set temp max
    if (MES_TEMP > SET_TEMP_MAX + (SET_TEMP_MAX * 0.01))
    {
        PID_ON = false;        // toggle pid off
        ledcWrite(ssrChan, 0); // output pin off
        return false;
    }
    // sample time check
    int sampleTime = PID_TIME_OVERSHOOT + (PID_TIME_OVERSHOOT + PID_SLEEP_TIME_MS - AERPID_COUNT) * PID_TICK_MAX;
    if (millis() - timed_start > sampleTime)
    {
        digitalWrite(PIN_LED_ACT, LOW);
        ledcWrite(ssrChan, 0); // output pin off
    }
    // perform PID calculation
    // if (aPID->Compute())
    aPID->compute();
    if (true)
    {
        double _output = output;
        if (output > 128)
        {
            // this helps... with overshoot reduction
            _output *= 0.667;
        }
        _output *= PWM_ScaleFactor;

        // convert output double to uint32
        uint32_t xOut = static_cast<uint32_t>(_output);

        bool _on = false;
        if (_output > 0.09)
        {
            // output at scaled % value
            ledcWrite(ssrChan, xOut += 1);
            _on = true;
        }
        else
        {
            ledcWrite(ssrChan, 0); // output off
            digitalWrite(PIN_LED_ACT, LOW);
        }
        unsigned long timed_length = millis() - timed_start;
        timed_running = false;
        if (_verbose_d)
        {
            Serial.print(F("[PID]"));
            Serial.print(F("["));
            Serial.print(index);
            Serial.print(F("]"));
            Serial.print(F(" TEMP= "));
            Serial.print(MES_TEMP);
            Serial.print(F(" :: "));
            Serial.print(_on);
            Serial.print(F(" :: "));
            Serial.print(F("TEMP_AVG= "));
            Serial.print(avgMeasures());
            Serial.print(F(" :: "));
            Serial.print(F("TIME= "));
            Serial.print(timed_length);
            Serial.print(F(" ms"));
            Serial.print(F(" :: "));
            Serial.print(F("PWM= "));
            Serial.print(output);
            Serial.print(F(" :: "));
            Serial.print(F("xPWM= "));
            Serial.print(_output);
            Serial.print(F(" :: "));
            Serial.print(F("xOUT= "));
            Serial.println(xOut);
        }
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------

double AerPID::avgMES_TEMP()
{
    double total = 0;
    int count = 0;
    for (int i = 0; i < MES_TEMP_SIZE; i++)
    {
        if (aMeasuresArr[i] > 0)
        {
            total += aMeasuresArr[i];
            count++;
        }
    }
    if (total > 0 && count > 0)
    {
        return total / count;
    }
    else
    {
        return 0;
    }
}

void AerPID::addToMeasuresB(double toAdd)
{
    double meas[MEASURES_SIZE];
    meas[0] = toAdd;
    for (int i = 1; i < MEASURES_SIZE; i++)
    {
        meas[i] = bMeasuresArr[i - 1];
    }
    for (int i = 0; i < MEASURES_SIZE; i++)
    {
        bMeasuresArr[i] = meas[i];
    }
}

void AerPID::addToMeasuresC(double toAdd)
{
    double meas[MEASURES_SIZE];
    meas[0] = toAdd;
    for (int i = 1; i < MEASURES_SIZE; i++)
    {
        meas[i] = cMeasuresArr[i - 1];
    }
    for (int i = 0; i < MEASURES_SIZE; i++)
    {
        cMeasuresArr[i] = meas[i];
    }
}

double AerPID::avgMeasures()
{
    double total = 0;
    int count = 0;
    for (int i = 0; i < MEASURES_AVG_TOTAL; i++)
    {
        if (bMeasuresArr[i] > 0)
        {
            total += bMeasuresArr[i];
            count++;
        }
    }
    if (total > 0 && count > 0)
    {
        return total / count;
    }
    else
    {
        return 0;
    }
}

double AerPID::avgMeasuresLong()
{
    double total = 0;
    int count = 0;
    for (int i = 0; i < MEASURES_AVG_TOTAL; i++)
    {
        if (cMeasuresArr[i] > 0)
        {
            total += cMeasuresArr[i];
            count++;
        }
    }
    if (total > 0 && count > 0)
    {
        return total / count;
    }
    else
    {
        return 0;
    }
}

double AerPID::maxMeasures()
{
    double max = 0;
    for (int i = 0; i < MEASURES_SIZE / 2; i++)
    {
        if (bMeasuresArr[i] > 1000)
        {
            continue;
        }
        if (bMeasuresArr[i] > max)
        {
            max = bMeasuresArr[i];
        }
    }
    return max;
}

double AerPID::minMeasuresLong()
{
    double min = maxMeasures();
    for (int i = 0; i < MEASURES_SIZE; i++)
    {
        if (cMeasuresArr[i] < 7)
        {
            continue;
        }
        if (cMeasuresArr[i] < min)
        {
            min = cMeasuresArr[i];
        }
    }
    return min;
}

double AerPID::maxMeasuresLong()
{
    double max = 0;
    for (int i = 0; i < MEASURES_SIZE; i++)
    {
        if (cMeasuresArr[i] > 1000)
        {
            continue;
        }
        if (cMeasuresArr[i] > max)
        {
            max = cMeasuresArr[i];
        }
    }
    return max;
}

void AerPID::addToMES_TEMP(double toAdd)
{
    double meas[MES_TEMP_SIZE];
    meas[0] = toAdd;
    for (int i = 1; i < MES_TEMP_SIZE; i++)
    {
        meas[i] = aMeasuresArr[i - 1];
    }
    for (int i = 0; i < MES_TEMP_SIZE; i++)
    {
        aMeasuresArr[i] = meas[i];
    }
}

double *AerPID::getMeasures()
{
    return bMeasuresArr;
}

double *AerPID::getMeasuresLong()
{
    return cMeasuresArr;
}

double AerPID::getTempMax()
{
    return SET_TEMP_MAX;
}

void AerPID::setTunings()
{
    aPID->setCoefficients(kP, kI, kD);
}

// =============================================================
// =============================================================
// =============================================================
// Thermocouple Measure
// =============================================================

#define TYPE_DS18S20 0
#define TYPE_DS18B20 1
#define TYPE_DS18S22 2
#define TYPE_MAX31850 3

boolean AerPID::measureElementTemperature()
{
    uint8_t present = 0;
    uint8_t temptype;
    uint8_t data[12];
    byte addr[8];
    float celsius;

    if (meas_ticker >= meas_ticker_max || meas_ticker == 0)
    {
        oneWire->reset_search();
        if (!oneWire->search(addr))
        {
            oneWire->reset_search();
            // Serial.println("Measure Reset!");
            // delay(5); // delay(250);
            return false;
        }

        /*Serial.print("ROM =");
        for (uint8_t i = 0; i < 8; i++)
        {
            Serial.write(' ');
            Serial.print(addr[i], HEX);
        }*/

        if (OneWire::crc8(addr, 7) != addr[7])
        {
            Serial.println("CRC is not valid!");
            return false;
        }
        // Serial.println();

        // the first ROM byte indicates which chip
        switch (addr[0])
        {
        case 0x10:
            // Serial.println("  Chip = DS18S20"); // or old DS1820
            temptype = TYPE_DS18S20;
            break;
        case 0x28:
            // Serial.println("  Chip = DS18B20");
            temptype = TYPE_DS18B20;
            break;
        case 0x22:
            // Serial.println("  Chip = DS1822");
            temptype = TYPE_DS18S22;
            break;
            // ADDED SUPPORT FOR MAX31850!
        case 0x3B:
            // Serial.println("  Chip = MAX31850");
            temptype = TYPE_MAX31850;
            break;
        default:
            // Serial.println("Device is not a DS18x20 family device.");
            return false;
        }
    }
    if (meas_ticker >= meas_ticker_max)
    {
        oneWire->reset();
        oneWire->select(addr);
        oneWire->write(0x44); // start conversion, use ds.write(0x44,1) with parasite power on at the end
        // Serial.println("Start conversion...");
    }

    if (meas_ticker > meas_ticker_max)
    {
        meas_ticker = meas_ticker_max;
    }
    else if (meas_ticker > 0)
    {
        meas_ticker--;
        return false;
    }
    else if (meas_ticker == 0)
    {
        meas_ticker = meas_ticker_max;
        // delay(200);
    }

    // delay(50); //delay(500);     // maybe 750ms is enough, maybe not
    //  we might do a ds.depower() here, but the reset will take care of it.

    present = oneWire->reset();
    if (!present)
    {
        Serial.println("Measure Abort!!");
        return false;
    }
    oneWire->select(addr);
    oneWire->write(0xBE); // Read Scratchpad

    // Serial.print("  Data = ");
    // Serial.print(present, HEX);
    // Serial.print(" ");
    for (uint8_t i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = oneWire->read();
        // Serial.print(data[i], HEX);
        // Serial.print(" ");
    }
    // Serial.print(" CRC=");
    // Serial.print(OneWire::crc8(data, 8), HEX);
    // Serial.println();

    // Serial.print("  Address = 0x");
    // Serial.println(data[4] & 0xF, HEX);

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (temptype == TYPE_DS18S20)
    {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else if (temptype == TYPE_MAX31850)
    {
        // Serial.println(raw, HEX);
        if (raw & 0x01)
        {
            // Serial.println("**FAULT!**");
            //  delay(300);
            return false;
        }
    }
    else
    {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
                            //// default is 12 bit resolution, 750 ms conversion time
    }

    celsius = (float)raw / 16.0;
    /*float fahrenheit = celsius * 1.8 + 32.0;
    Serial.print("  Temperature = ");
    Serial.print(celsius);
    Serial.print(" Celsius, ");
    Serial.print(fahrenheit);
    Serial.println(" Fahrenheit");*/

    addToMES_TEMP(celsius + OFFSET_TEMP);

    return true;
}

// set resolution of a device to 9, 10, 11, or 12 bits
// if new resolution is out of range, 9 bits is used.
bool AerPID::setSensorResolution(uint8_t newResolution)
{
    byte addr[8];
    uint8_t temptype;

    oneWire->reset_search();
    if (!oneWire->search(addr))
    {
        oneWire->reset_search();
        return false;
    }

    if (OneWire::crc8(addr, 7) != addr[7])
    {
        Serial.println("CRC is not valid!");
        return false;
    }

    // the first ROM byte indicates which chip
    switch (addr[0])
    {
    case 0x3B:
        Serial.print(" Chip = MAX31850 ...");
        temptype = TYPE_MAX31850;
        break;
    default:
        Serial.println("Device is not a DS18x20 family device.");
        return false;
    }

    if (temptype == TYPE_MAX31850)
    {
        uint8_t scratchPad[9];
        readScratchPad(addr, scratchPad);
        Serial.print(" Set Resolution: ");
        switch (newResolution)
        {
        case 12:
            scratchPad[SENSOR_CONFIGURATION_LOCATION] = TEMP_12_BIT;
            Serial.print(12);
            break;
        case 11:
            scratchPad[SENSOR_CONFIGURATION_LOCATION] = TEMP_11_BIT;
            Serial.print(11);
            break;
        case 10:
            scratchPad[SENSOR_CONFIGURATION_LOCATION] = TEMP_10_BIT;
            Serial.print(10);
            break;
        case 9:
        default:
            scratchPad[SENSOR_CONFIGURATION_LOCATION] = TEMP_9_BIT;
            Serial.print(9);
            break;
        }
        Serial.println(" bits");

        oneWire->reset();
        oneWire->select(addr);
        oneWire->write_bytes(scratchPad, 9);
        Serial.print("Wrote Scratchpad to Sensor...");

        oneWire->reset();
    }

    return false;
}

void AerPID::readScratchPad(uint8_t *deviceAddress, uint8_t *scratchPad)
{
    // send the command
    oneWire->reset();
    oneWire->select(deviceAddress);
    oneWire->write(READSCRATCH);

    // TODO => collect all comments &  use simple loop
    // byte 0: temperature LSB
    // byte 1: temperature MSB
    // byte 2: high alarm temp
    // byte 3: low alarm temp
    // byte 4: DS18S20: store for crc
    //         DS18B20 & DS1822: configuration register
    // byte 5: internal use & crc
    // byte 6: DS18S20: COUNT_REMAIN
    //         DS18B20 & DS1822: store for crc
    // byte 7: DS18S20: COUNT_PER_C
    //         DS18B20 & DS1822: store for crc
    // byte 8: SCRATCHPAD_CRC
    //
    // for(int i=0; i<9; i++)
    // {
    //   scratchPad[i] = _wire->read();
    // }

    // read the response

    // byte 0: temperature LSB
    scratchPad[TEMP_LSB] = oneWire->read();

    // byte 1: temperature MSB
    scratchPad[TEMP_MSB] = oneWire->read();

    // byte 2: high alarm temp
    scratchPad[HIGH_ALARM_TEMP] = oneWire->read();

    // byte 3: low alarm temp
    scratchPad[LOW_ALARM_TEMP] = oneWire->read();

    // byte 4:
    // DS18S20: store for crc
    // DS18B20 & DS1822: configuration register
    scratchPad[CONFIGURATION] = oneWire->read();

    // byte 5:
    // internal use & crc
    scratchPad[INTERNAL_BYTE] = oneWire->read();

    // byte 6:
    // DS18S20: COUNT_REMAIN
    // DS18B20 & DS1822: store for crc
    scratchPad[COUNT_REMAIN] = oneWire->read();

    // byte 7:
    // DS18S20: COUNT_PER_C
    // DS18B20 & DS1822: store for crc
    scratchPad[COUNT_PER_C] = oneWire->read();

    // byte 8:
    // SCTRACHPAD_CRC
    scratchPad[SCRATCHPAD_CRC] = oneWire->read();

    for (uint8_t i = 0; i < 8; i++)
    {
        // Serial.print("\n 0x"); Serial.print(scratchPad[i], HEX);
    }
    oneWire->reset();
}
