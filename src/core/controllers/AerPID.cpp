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
AerPID::AerPID(uint8_t pin, uint8_t ssrPin, uint8_t ssrChan, uint8_t index)
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
        // Second, Perform element measurements...
        MeasureResult res = measureElementTemperature();
        if (res == MeasureResult::ACK)
        {
            meas_counter = PID_MEAS_COUNTER;
            MES_TEMP = avgMES_TEMP();
            addToMeasuresB(MES_TEMP);
            AVG_TEMP = avgMeasures();
        }
        else if (res == MeasureResult::FAULT)
        {
            meas_counter = PID_MEAS_COUNTER;
        }
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
            _tick = 2;
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
        Sigma sigma = Sigma();

        double aSig = sigma.calcDeviation(aMeasuresArr, MES_TEMP_SIZE);
        double bSig = sigma.calcDeviation(bMeasuresArr, MEASURES_SIZE, 28);
        double bSigAll = sigma.calcDeviation(cMeasuresArr, MEASURES_SIZE);
        bool aboveTarget = MES_TEMP > SET_TEMP;
        double delta = (SET_TEMP - MES_TEMP);

        double _output = output;

        // _output *= PWM_ScaleFactor;

        // convert output double to uint32
        uint32_t xOut = static_cast<uint32_t>(_output);
        xOutput = xOut;

        bool _on = false;
        if (_output > 0.09)
        {
            // output at scaled % value
            ledcWrite(ssrChan, xOutput);
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
            Serial.print(xOut);

            Serial.print(F(" :: "));
            Serial.print(F("T SIGMA= "));
            Serial.print(aSig);

            Serial.print(F(" :: "));
            Serial.print(F("TA SIGMA= "));
            Serial.println(bSig);
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

void AerPID::setTunings(bool reset)
{
    if (reset)
    {
        aPID->reset();
    }
    aPID->setCoefficients(kP, kI, kD);
}

void AerPID::setOutputBias(double bias)
{
    this->bias = bias;
    aPID->setBias(bias);
}

double AerPID::getOutputBias()
{
    return bias;
}

void AerPID::setWindupLimit(double limit)
{
    this->windUpLimit = limit;
    aPID->setWindUpLimits(-limit, limit);
}

double AerPID::getWindupLimit()
{
    return windUpLimit;
}

// =============================================================
// =============================================================
// =============================================================
// Thermocouple Measure
// =============================================================

/**
 * @brief (Safe) Element Measure Function.
 * This function will handle faults with the instrument sensor
 * and switch to Blocking mode if too many faults are detected.
 *
 * @return true on success.
 * @return false on error, fail, or busy.
 */
AerPID::MeasureResult AerPID::measureElementTemperature()
{
    if (_measMode > 0)
    {
        if (_measMode == 1)
        {
            return measureElementTemperatureAsync();
        }
        if (_measMode == 2)
        {
            if (PID_ON)
            {
                ledcWrite(ssrChan, 0); // output pin off
                delayMicroseconds(60); // some time to settle...
            }
            MeasureResult result = measureElementTemperatureBlocking();
            if (PID_ON && output > 0.09)
            {
                uint32_t xOutput = static_cast<uint32_t>(output);
                ledcWrite(ssrChan, xOutput);
            }
            return result;
        }
        return MeasureResult::NACK;
    }

    bool useAsync = !_faultError;
    bool measSuccess = false;
    bool recentFault = false;

    if (millis() - _faultLastTime < 10000)
    {
        recentFault = true;
    }

    if (!recentFault && _faultsRecent > 0)
    {
        _faultsRecent--;
    }

    if (_faultsTotal >= 20)
    {
        useAsync = false;
        if (!_faultError)
        {
            _faultError = true;
            Serial.println(F("********************************"));
            Serial.println(F("*********** WARNING! ***********"));
            Serial.println(F("********************************"));
            Serial.println(F(">> Sensor Fault Error Detected!!"));
            Serial.print(F(">>> FAULT CODE: "));
            Serial.print(_faultCodeLast);
            Serial.print(F(" on AerPID #"));
            Serial.println(index);
            Serial.println(F(">> Sensor Entering Safe Mode! <<"));
            Serial.println(F("********************************"));
            Serial.println(F(">>> If this problem persists:"));
            Serial.println(F(" there may be noise on the line,"));
            Serial.println(F(" there is too much interference,"));
            Serial.println(F(" instrument signal is malformed,"));
            Serial.println(F(" the cable length is too long,"));
            Serial.println(F(" the element sensor is faulty,"));
            Serial.println(F(" or there is nothing connected."));
            Serial.println(F("********************************"));
            Serial.println(F(" "));
        }
    }
    else if (_faultsRecent > 3)
    {
        useAsync = false;
    }

    if (_faultError && recentFault && _faultsRecent > 5)
    {
        delay(1000);
    }

    // Firstly, disable the power to the element if in safe mode.
    if ((!useAsync || _faultError) && PID_ON)
    {
        ledcWrite(ssrChan, 0); // output pin off
        delayMicroseconds(80); // some time to settle...
    }
    // Second, perform measurement...
    MeasureResult result = useAsync ? measureElementTemperatureAsync() : measureElementTemperatureBlocking();

    // Third, re-enable output power to the element.
    if ((!useAsync || _faultError) && PID_ON && output > 0.09)
    {
        uint32_t xOutput = static_cast<uint32_t>(output);
        ledcWrite(ssrChan, xOutput);
    }

    return result;
}

/**
 * @brief Non-Blocking Temperature Measurement Function
 *
 * @return true on success.
 * @return false on error, fail, or busy.
 */
AerPID::MeasureResult AerPID::measureElementTemperatureAsync()
{
    uint8_t present = 0;
    uint8_t temptype;
    uint8_t data[9];
    byte addr[8];
    float celsius;

    if (meas_ticker == 0 || meas_ticker >= meas_ticker_max - 1)
    {
        oneWire->reset_search();
        if (!oneWire->search(addr))
        {
            oneWire->reset_search();
            Serial.println("(async) Measure Reset!");
            return MeasureResult::NACK;
        }

        /*Serial.print("ROM =");
        for (uint8_t i = 0; i < 8; i++)
        {
            Serial.write(' ');
            Serial.print(addr[i], HEX);
        }*/

        if (OneWire::crc8(addr, 7) != addr[7])
        {
            Serial.println("(async) CRC is not valid!");
            return MeasureResult::NACK;
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
            return MeasureResult::NACK;
        }
    }
    if (meas_ticker == meas_ticker_max - 1)
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
    if (meas_ticker == 0)
    {
        meas_ticker = meas_ticker_max;
    }
    else if (meas_ticker > 0)
    {
        meas_ticker--;
        return MeasureResult::NACK;
    }

    present = oneWire->reset();
    if (!present)
    {
        Serial.println("(async) Measure Abort!!");
        return MeasureResult::NACK;
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
            if (PID_ON)
            {
                _faultCodeLast = getMax31855ErrorCode(raw);
                Serial.print(F("(async) ERROR: "));
                Serial.print(_faultCodeLast, HEX);
                Serial.println(F(" **MEASURE FAULT!**"));
                if (_faultsTotal < 0xFFFFFFFF)
                {
                    _faultsTotal++;
                    _faultsRecent++;
                    _faultLastTime = millis();
                }
            }
            return MeasureResult::FAULT;
        }
    }
    else
    {
        byte cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
        {
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        }
        else if (cfg == 0x20)
        {
            raw = raw & ~3; // 10 bit res, 187.5 ms
        }
        else if (cfg == 0x40)
        {
            raw = raw & ~1; // 11 bit res, 375 ms
                            //// default is 12 bit resolution, 750 ms conversion time
        }
    }

    celsius = ((float)raw / 16.0) + OFFSET_TEMP;

    Sigma sigma = Sigma();
    double bSig = sigma.calcDeviation(bMeasuresArr, MEASURES_AVG_TOTAL);
    /*Serial.print(F("SIGMA= "));
    Serial.print(AVG_TEMP + bSig * 3);
    Serial.print(" < ");
    Serial.print(celsius);
    Serial.print(" > ");
    Serial.println(AVG_TEMP - bSig * 3);*/
    double meas = (celsius + AVG_TEMP) / 2;
    if ((celsius > meas + (bSig * 1) || celsius < meas - (bSig * 1)) && abs(celsius - AVG_TEMP) > 3 && AVG_TEMP > 0)
    {
        Serial.print(F("SIGMA= "));
        Serial.print(AVG_TEMP + bSig * 1);
        Serial.print(" < ");
        Serial.print(celsius);
        Serial.print(" > ");
        Serial.println(AVG_TEMP - bSig * 1);

        _faultsRecent++;
        _faultLastTime = millis();
        return AerPID::FAULT;
    }

    addToMES_TEMP(celsius);
    return MeasureResult::ACK;
}

/**
 * @brief Minified 'Blocking' Temperature Measurement Function
 *
 * @return true on success.
 * @return false on error, fail, or busy.
 */
AerPID::MeasureResult AerPID::measureElementTemperatureBlocking()
{
    uint8_t present = 0;
    uint8_t temptype;
    uint8_t data[9];
    byte addr[8];
    float celsius;

    oneWire->reset_search();
    if (!oneWire->search(addr))
    { // Reset if no response on search
        oneWire->reset_search();
        Serial.println("Measure Reset!");
        return MeasureResult::NACK;
    }

    if (OneWire::crc8(addr, 7) != addr[7])
    { // Check if the CRC is even valid
        Serial.println("CRC is not valid!");
        return MeasureResult::NACK;
    }

    switch (addr[0]) // first ROM byte indicates chip type
    {
    case 0x3B:
        // Serial.println("  Chip = MAX31850");
        temptype = TYPE_MAX31850;
        break;
    default:
        // Serial.println("Device is not a MAX3185x family device.");
        return MeasureResult::NACK;
    }

    oneWire->reset();      // reset the bus
    oneWire->select(addr); // select the chip
    oneWire->write(0x44);  // start conversion

#if MEASURE_BIT_PRECISION == 9
    delay(94);
#endif
#if MEASURE_BIT_PRECISION == 10
    delay(190);
#endif
#if MEASURE_BIT_PRECISION == 11
    delay(375);
#endif
#if MEASURE_BIT_PRECISION == 12
    delay(750);
#endif

    present = oneWire->reset(); // presense check
    if (!present)
    { // check if chip is even present!
        Serial.println("Measure Abort!!");
        return MeasureResult::NACK;
    }
    oneWire->select(addr); // select the chip
    oneWire->write(0xBE);  // Read Scratchpad

    for (uint8_t i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = oneWire->read();
    }

    int16_t raw = (data[1] << 8) | data[0];
    // Serial.println(raw, HEX);
    if (raw & 0x01)
    {
        if (PID_ON)
        {
            _faultCodeLast = getMax31855ErrorCode(raw);
            Serial.print(F("ERROR: "));
            Serial.print(_faultCodeLast, HEX);
            Serial.println(F(" **FAULT!**"));
        }
        return MeasureResult::FAULT;
    }

    celsius = (float)raw / 16.0;
    addToMES_TEMP(celsius + OFFSET_TEMP);
    return MeasureResult::ACK;
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

AerPID::Max3185xState AerPID::getMax31855ErrorCode(uint16_t egtPacket)
{
#define MAX31855_RESERVED_BITS 0x20008
#define MAX33855_FAULT_BIT BIT(16)
#define MAX33855_OPEN_BIT BIT(0)
#define MAX33855_GND_BIT BIT(1)
#define MAX33855_VCC_BIT BIT(2)

    if (((egtPacket & MAX31855_RESERVED_BITS) != 0) ||
        (egtPacket == 0x0) ||
        (egtPacket == 0xffffffff))
    {
        return MAX3185X_NO_REPLY;
    }
    else if ((egtPacket & MAX33855_OPEN_BIT) != 0)
    {
        return MAX3185X_OPEN_CIRCUIT;
    }
    else if ((egtPacket & MAX33855_GND_BIT) != 0)
    {
        return MAX3185X_SHORT_TO_GND;
    }
    else if ((egtPacket & MAX33855_VCC_BIT) != 0)
    {
        return MAX3185X_SHORT_TO_VCC;
    }
    else
    {
        return MAX3185X_OK;
    }
}

double AerPID::getOutput()
{
    return this->xOutput;
}

double AerPID::getSigma()
{
    Sigma sigma = Sigma();
    return sigma.calcDeviation(cMeasuresArr, MEASURES_SIZE, 28);
}

bool AerPID::hasFaultError()
{
    return this->_faultError;
}

bool AerPID::hasFaultErrorAlerted()
{
    return this->_faultErrorAlerted;
}

void AerPID::setFaultErrorAlert(bool alerted)
{
    this->_faultErrorAlerted = alerted;
}
