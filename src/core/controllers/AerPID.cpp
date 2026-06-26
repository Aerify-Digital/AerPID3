#include "AerPID.h"

// Construct a new AerPID object
AerPID::AerPID(uint8_t pin, uint8_t ssrPin, uint8_t ssrChan, uint8_t index)
{
    Serial.print(F("> "));
    Serial.print(F("Creating new instance of "));
    Serial.print(F("AerPID..."));

    this->index = index;
    this->ssrPin = ssrPin;
    this->ssrChan = ssrChan;
    this->owPin = pin;

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

    // =======================
    // initialize 1wire bus
    oneWire = new OneWire(owPin);

    Serial.println(F(" ok!"));

    // =======================
    // Setup SSR Pin Control
    updatePWM(ssrPin, ssrChan, _pwmFrequency, _pwmResolution);

    // =======================
    // Setup PID timing mode
    updateSampleTime(_pidTickMax);

    // =======================
    // Set initial PID tunings and limits
    aPID->setCoefficients(kP, kI, kD);
    aPID->setOutputLimits(0, _pidOutputLimit);
    aPID->setWindUpLimits(-windUpLimit, windUpLimit);
    aPID->setBias(_pidBias);
    aPID->reset();

    // start the PID function
    aPID->start();

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
// Function Work Ticks
// -----------------------------------------------------------------------------

void AerPID::handleFeatureSetTick()
{
    if (pidEnabled && millis() - _lastPidTimeMs >= 1000)
    {
        running_time++;
        if (AUTO_OFF_ENB)
        {
            run_time++;
            if (run_time > AUTO_OFF_TIME)
            {
                run_time = 0;
                pidEnabled = false;
                Serial.print("PID ");
                Serial.print(F("["));
                Serial.print(index);
                Serial.print(F("]"));
                Serial.println(" Disabled due to timeout!");
            }
        }
        _lastPidTimeMs = millis();
    }
}

// -----------------------------------------------------------------------------

// Main tick
void AerPID::tick()
{
    // tick measure counter
    if (millis() - _measLastTime > _measLastTimeMax || _measLastTime == 0)
    {
        // Second, Perform element measurements...
        MeasureResult res = measureElementTemperature();
        if (res == MeasureResult::ACK)
        {
            MES_TEMP = avgMES_TEMP();
            addToMeasuresB(MES_TEMP);
            AVG_TEMP = avgMeasures();
        }
        else if (res == MeasureResult::PENDING)
        {
            _measLastTime = millis() - MEASURE_TIME_COST * 0.6;
        }
        else if (res == MeasureResult::FAULT)
        {
            //_measLastTime = millis() - MEASURE_TIME_COST;
        }
    }
    if (millis() - _measLastTimeLong >= 3000)
    {
        addToMeasuresC(MES_TEMP);
        _measLastTimeLong = millis();
    }
    // tick pid compute
    if (_tick-- <= 0)
    {
        //  Process PID computation
        if (compute())
        {
            // reset pid tick. cadence is owned by the elapsed-time gate in compute()
            // (sampleTime via getPidTick()); this just keeps the _tick-- guard satisfied.
            // (was `_pidTickMax * (5 / 1000)` = integer 0 — a no-op; made explicit.)
            _tick = 0;
        }
        // Floor the counter so it can never march into int16 underflow. When compute()
        // returns false (PID disabled, over-temp latch, or sample-time not yet elapsed)
        // the original code left _tick decrementing unbounded: after ~6 min it reached
        // INT16_MIN and wrapped to +32767, making `_tick-- <= 0` false and freezing
        // compute() for ~6 min. Cadence is owned by compute()'s own sampleTime gate, so
        // re-flooring to 0 is behavior-preserving (compute() is still evaluated every
        // tick while _tick <= 0).
        if (_tick < 0)
        {
            _tick = 0;
        }
    }

    // timer trigger var
    if (!timed_running)
    {
        timed_running = true;
        timed_start = millis();
        if (pidEnabled)
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
    // sample time check
    int sampleTime = (PID_SLEEP_TIME_MS * getPidTick()) - PID_TIME_OVERSHOOT;
    if (millis() - timed_start < sampleTime)
    {
        return false;
    }
    // check if PID functions are active...
    if (!pidEnabled) // output disabled
    {
        digitalWrite(PIN_LED_ACT, LOW);
        ledcWrite(ssrChan, 0); // output pin off
        timed_start = millis();
        timed_running = false;
        run_time = 0;
        output = 0;
        xOutput = 0;
        return false;
    }
    // disable if above set temp max
    if (MES_TEMP > SET_TEMP_MAX + (SET_TEMP_MAX * 0.01))
    {
        pidEnabled = false;    // toggle pid off
        ledcWrite(ssrChan, 0); // output pin off
        return false;
    }

    /*if (millis() - timed_start > sampleTime)
    {
        digitalWrite(PIN_LED_ACT, LOW);
        ledcWrite(ssrChan, 0); // output pin off
    }
    else if (millis() - timed_start < sampleTime)
    {
        Serial.print("PID not ready! ");
        Serial.print(millis() - timed_start);
        Serial.print(" < ");
        Serial.println(sampleTime);
        return false;
    }*/
    if (true)
    {
        // perform PID calculation
        aPID->compute();

        if (OUTPUT_DEBUG_PID)
        {
            aPID->debug(&Serial, "aPID", PRINT_INPUT | PRINT_OUTPUT | PRINT_SETPOINT | PRINT_BIAS | PRINT_P | PRINT_I | PRINT_D);
        }

        Sigma sigma = Sigma();

        double aSig = sigma.calcDeviation(aMeasuresArr, MES_TEMP_SIZE);
        double bSig = sigma.calcDeviation(bMeasuresArr, MEASURES_SIZE, 28);
        double bSigAll = sigma.calcDeviation(cMeasuresArr, MEASURES_SIZE);
        bool aboveTarget = MES_TEMP > SET_TEMP;
        double delta = (SET_TEMP - MES_TEMP);

        // scale output using temperature delta for better stability at set point
        // double _output = deltaScaleOutput(delta, output);
        double _output = max(0.0, output);

        // convert output double to uint32 for ledcWrite
        xOutput = static_cast<uint32_t>(_output);

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
            Serial.print(F("ERROR= "));
            Serial.print(delta);
            Serial.print(F(" :: "));
            Serial.print(F("OUT= "));
            Serial.print(output);
            Serial.print(F(" :: "));
            Serial.print(F("xPWM= "));
            Serial.print(_output);
            Serial.print(F(" :: "));
            Serial.print(F("xOUT= "));
            Serial.print(xOutput);

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

double AerPID::deltaScaleOutput(const double delta, const double output)
{
    // PWM Resolution is half of the output range
    const double resolution = 1.0;

    // Scale PWM output based on the scaling factor
    double _output = output * _pwmScaleFactor;

    if (delta < -8.8)
    {
        // When over temp, set output power to 0
        return 0;
    }
    else if (delta <= -2.7)
    {
        return _output * resolution * 0.7;
    }
    else if (delta <= 0)
    {
        return _output * resolution * 0.8;
    }
    else if (delta <= 1.6)
    {
        return _output * resolution * 0.9;
    }
    else if (delta <= 3.3)
    {
        return _output * resolution * 0.95;
    }
    else // Assuming max value is controlled by the PID settings or bounds
    {
        return _output * resolution * 1.0; // Full output if delta exceeds predefined limits
    }
}

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
    if (bias < 0)
    {
        return;
    }
    this->_pidBias = bias;
    aPID->setBias(bias);
}

double AerPID::getOutputBias()
{
    return _pidBias;
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

int AerPID::getPwmFreq()
{
    return _pwmFrequency;
}

void AerPID::setPwmFreq(int freq)
{
    _pwmFrequency = freq;
    this->updatePWM(ssrPin, ssrChan, _pwmFrequency, _pwmResolution);
}

double AerPID::getPwmScaler()
{
    return _pwmScaleFactor;
}

void AerPID::setPwmScaler(double factor)
{
    _pwmScaleFactor = factor;
}

int AerPID::getPidTime()
{
    return _pidCycleTime;
}

int AerPID::getPidTick()
{
    return _pidTickMax;
}

void AerPID::setPidTime(int time)
{
    if (time < 50)
    {
        return;
    }
    _pidCycleTime = time;
    _pidTickMax = static_cast<int>(((double)time / PID_SLEEP_TIME_MS) * 1);
    this->updateSampleTime(_pidTickMax);
}

int AerPID::getPwmResolution()
{
    return _pwmResolution;
}

void AerPID::setPWMResolution(int res)
{
    if (res < 8)
    {
        return;
    }
    else if (res > 16)
    {
        return;
    }
    _pwmResolution = res;
    this->updatePWM(ssrPin, ssrChan, _pwmFrequency, _pwmResolution);
}

int AerPID::getOutputLimit()
{
    return _pidOutputLimit;
}

void AerPID::setOutputLimit(int limit)
{
    if (limit < 1)
    {
        return;
    }
    _pidOutputLimit = limit;
    aPID->setOutputLimits(0, _pidOutputLimit);
}

void AerPID::updatePWM(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t resolution_bits)
{
    if (_ledAttached)
    {
        ledcDetachPin(pin);
        _ledAttached = false;
        Serial.print(F(">> SETUP"));
        Serial.print(F("["));
        Serial.print(index);
        Serial.print(F("]"));
        Serial.print(F(" "));
        Serial.print(F("Detached PWM control of Pin "));
        Serial.print(F("'"));
        Serial.print(pin);
        Serial.println(F("!"));
    }
    double freqSet = ledcSetup(channel, freq, resolution_bits);
    ledcAttachPin(pin, channel);
    _ledAttached = true;
    if (_verbose_d)
    {
        Serial.print(F(">> SETUP"));
        Serial.print(F("["));
        Serial.print(index);
        Serial.print(F("]"));
        Serial.print(F(" "));
        Serial.print(F("Attached PWM control to Pin "));
        Serial.print(F("'"));
        Serial.print(pin);
        Serial.print(F("'"));
        Serial.print(F(" on Channel "));
        Serial.print(F("'"));
        Serial.print(channel);
        Serial.print(F("'"));
        Serial.print(F(" at frequency of: "));
        Serial.print(freqSet, 0);
        Serial.print(F(" hz"));
        Serial.print(F("  with: "));
        Serial.print(resolution_bits);
        Serial.print(F(" resolution bits"));
        Serial.println(F("!"));
    }
}

void AerPID::updateSampleTime(int pidTickMax)
{
    int sampleTime = PID_TIME_OVERSHOOT + ((0 + PID_SLEEP_TIME_MS) * pidTickMax);
    aPID->setSampleTime(sampleTime);
    aPID->reset();
    aPID->start();
    if (_verbose_d)
    {
        Serial.print(F(">> SETUP"));
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
    // async & sync modes
    if (_measMode > 0)
    {
        // async
        if (_measMode == 1)
        {
            return measureElementTemperatureAsync();
        }
        // sync
        if (_measMode == 2)
        {
            if (pidEnabled)
            {
                ledcWrite(ssrChan, 0); // output pin off
                delayMicroseconds(60); // some time to settle...
            }
            MeasureResult result = measureElementTemperatureBlocking();
            if (pidEnabled && output > 0.09)
            {
                double delta = (SET_TEMP - MES_TEMP);
                double _output = deltaScaleOutput(delta, output);
                _output = max(0.0, _output);
                uint32_t xOutput = static_cast<uint32_t>(_output);
                ledcWrite(ssrChan, xOutput);
            }
            return result;
        }
        return MeasureResult::NACK;
    }

    // auto mode
    // defaults to async, swaps to sync if too many fault.

    bool useAsync = !_faultError;
    bool measSuccess = false;
    bool recentFault = false;
    bool historFault = false;

    if (millis() - _faultLastTime < 7000)
    {
        recentFault = true;
    }
    if (millis() - _faultLastTime < 30000)
    {
        historFault = true;
    }

    if (!recentFault && _faultsRecent > 0)
    {
        _faultsRecent--;
    }
    if (!historFault && _faultsTotal > _faultsRecent)
    {
        _faultsTotal--;
    }

    if (_faultsTotal >= 50)
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
        delay(250);
    }

    // Firstly, disable the power to the element if in safe mode.
    if ((!useAsync || _faultError) && pidEnabled)
    {
        ledcWrite(ssrChan, 0); // output pin off
        delayMicroseconds(80); // some time to settle...
    }
    // Second, perform measurement...
    MeasureResult result = useAsync ? measureElementTemperatureAsync() : measureElementTemperatureBlocking();

    // Third, re-enable output power to the element.
    if ((!useAsync || _faultError) && pidEnabled && output > 0.09)
    {
        double delta = (SET_TEMP - MES_TEMP);
        double _output = deltaScaleOutput(delta, output);
        _output = max(0.0, _output);
        uint32_t xOutput = static_cast<uint32_t>(_output);
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

    bool isMeasureReset = _measureTimeMs == 0;
    bool doMeasure = millis() - _measureTimeMs >= MEASURE_TIME_COST + 2;

    if (isMeasureReset || doMeasure)
    {
        oneWire->reset_search();
        if (!oneWire->search(addr))
        {
            oneWire->reset_search();
            Serial.println(F("(async) Measure Reset!"));
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
            Serial.println(F("(async) CRC is not valid!"));
            return MeasureResult::NACK;
        }
        // Serial.println();

        // the first ROM byte indicates which chip
        switch (addr[0])
        {
        case 0x3B:
            // Serial.println("  Chip = MAX31850");
            temptype = TYPE_MAX31850;
            break;
        default:
            // Serial.println("Device is not a DS18x20 family device.");
            return MeasureResult::NACK;
        }
    }

    if (isMeasureReset)
    {
        oneWire->reset();
        oneWire->select(addr);
        oneWire->write(STARTCONVO); // start conversion, use ds.write(0x44,1) with parasite power on at the end
        // Serial.println("(async) Start conversion...");
        _measureTimeMs = millis();
        return MeasureResult::PENDING;
    }
    else if (!doMeasure)
    {
        Serial.println(F("(async) Measure Pending!"));
        return MeasureResult::PENDING;
    }

    // Serial.println("(async) Measure Perform!!!");
    _measureTimeMs = 0;

    present = oneWire->reset();
    if (!present)
    {
        Serial.println(F("(async) Measure Abort!!"));
        return MeasureResult::NACK;
    }
    oneWire->select(addr);
    oneWire->write(READSCRATCH); // Read Scratchpad

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

    int16_t raw = (data[1] << 8) | data[0];
    // Serial.println(raw, HEX);
    if (raw & 0x01)
    {
        if (pidEnabled)
        {
            _faultCodeLast = getMax31855ErrorCode(raw);
            Serial.print(F("ERROR: "));
            Serial.print(_faultCodeLast, HEX);
            Serial.println(F(" **FAULT!**"));
        }
        return MeasureResult::FAULT;
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
    if ((celsius > meas + (bSig * 3) || celsius < meas - (bSig * 3)) && abs(celsius - AVG_TEMP) > 3 && AVG_TEMP > 0)
    {
        Serial.print(F("(async) Fault! MEASURE SIGMA= "));
        Serial.print(AVG_TEMP + bSig * 3);
        Serial.print(F(" < "));
        Serial.print(celsius);
        Serial.print(F(" > "));
        Serial.print(AVG_TEMP - bSig * 3);
        Serial.print(F(" :: "));
        Serial.print(F("(async) Last Measure Time= "));
        Serial.print(millis() - _measLastTime);
        Serial.print(F(" ms"));

        if (millis() - _measLastTime > FAULT_TIMEOUT || _faultsRecent >= 3)
        {
            if (_faultsRecent > 0)
            {
                _faultsRecent--; // guard: _faultsRecent is unsigned; timeout branch can reach here at 0
            }
            addToMES_TEMP(celsius);
            _measLastTime = millis();
            Serial.println(F("(async) Measure Fault Recovery! Forcing measure..."));
            return MeasureResult::ACK;
        }
        else
        {
            Serial.println(F(" "));
        }

        _faultsRecent++;
        _faultLastTime = millis();
        return MeasureResult::FAULT;
    }

    addToMES_TEMP(celsius);
    _measLastTime = millis();
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
        Serial.println(F("Measure Reset!"));
        return MeasureResult::NACK;
    }

    if (OneWire::crc8(addr, 7) != addr[7])
    { // Check if the CRC is even valid
        Serial.println(F("CRC is not valid!"));
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

    oneWire->reset();           // reset the bus
    oneWire->select(addr);      // select the chip
    oneWire->write(STARTCONVO); // start conversion
    delay(MEASURE_TIME_COST);   // delay for conversion to complete

    present = oneWire->reset(); // presense check
    if (!present)
    { // check if chip is even present!
        Serial.println(F("Measure Abort!!"));
        return MeasureResult::NACK;
    }
    oneWire->select(addr);       // select the chip
    oneWire->write(READSCRATCH); // Read Scratchpad

    for (uint8_t i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = oneWire->read();
    }

    int16_t raw = (data[1] << 8) | data[0];
    // Serial.println(raw, HEX);
    if (raw & 0x01)
    {
        if (pidEnabled)
        {
            _faultCodeLast = getMax31855ErrorCode(raw);
            Serial.print(F("ERROR: "));
            Serial.print(_faultCodeLast, HEX);
            Serial.println(F(" **FAULT!**"));
        }
        return MeasureResult::FAULT;
    }

    celsius = ((float)raw / 16.0) + OFFSET_TEMP;
    addToMES_TEMP(celsius);
    _measLastTime = millis();
    return MeasureResult::ACK;
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
    return sigma.calcDeviation(bMeasuresArr, MEASURES_SIZE, 84) * 3;
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
