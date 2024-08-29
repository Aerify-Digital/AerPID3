#include "AerManager.h"

// Sets the app version
void AerManager::setVersionApp(AppVersion *v)
{
    AerManager::version = v;
}

// Sets the web app version
void AerManager::setVersionWeb(AppVersion *v)
{
    AerManager::versionWeb = v;
}

// Sets the app version
void AerManager::setVersion(uint major, uint minor, uint build)
{
    AerManager::setVersionApp(new AppVersion(major, minor, build));
}

AppVersion *AerManager::getVersion()
{
    return version;
}

AppVersion *AerManager::getVersionWeb()
{
    return versionWeb;
}

// Gets the AerPID storage Object
AerPID *AerManager::getAerPID(uint8_t ei) // getAerStorage()
{
    return AerManager::aerPID[ei];
}

// Gets the Network Variable storage Object
NetworkVars *AerManager::getNetVars()
{
    return AerManager::networkVars;
}

// ===========================================================

void AerManager::setCpu0(uint8_t v) { cpu0_usage = v; }
uint8_t AerManager::getCpu0() { return cpu0_usage; }
void AerManager::setCpu1(uint8_t v) { cpu1_usage = v; }
uint8_t AerManager::getCpu1() { return cpu1_usage; }

/**
 * @brief add load data to storage array for cpu 0
 *
 * @param v
 */
void AerManager::addCpu0(uint8_t v)
{
    shiftArrDown(cpu0_usages);
    cpu0_usages[0] = v;
}

/**
 * @brief cpu 0 load data
 *
 * @return uint8_t* pointer to array of data
 */
uint8_t *AerManager::getCpu0s() { return cpu0_usages; }

/**
 * @brief add load data to storage array for cpu 1
 *
 * @param v
 */
void AerManager::addCpu1(uint8_t v)
{
    shiftArrDown(cpu1_usages);
    cpu1_usages[0] = v;
}

/**
 * @brief cpu 1 load data
 *
 * @return uint8_t* pointer to array of data
 */
uint8_t *AerManager::getCpu1s() { return cpu1_usages; }

// gets the bar counts for display
uint AerManager::getCpuBarCount() { return cpu_usages_size; }

/**
 * @brief get cpu 0 load average
 *
 * @return uint8_t average load
 */
uint8_t AerManager::getCpu0Avg()
{
    uint32_t total = 0;
    for (int i = 0; i < cpu_usages_size; i++)
    {
        total += cpu0_usages[i];
    }
    return (total / cpu_usages_size) & 0xff;
}

/**
 * @brief get cpu 1 load average
 *
 * @return uint8_t average load
 */
uint8_t AerManager::getCpu1Avg()
{
    uint32_t total = 0;
    for (int i = 0; i < cpu_usages_size; i++)
    {
        total += cpu1_usages[i];
    }
    return (total / cpu_usages_size) & 0xff;
}

/**
 * @brief get cpu 0 load average by size
 *
 * @param s count of samples to average
 * @return uint8_t average load
 */
uint8_t AerManager::getCpu0Avg(uint8_t s)
{
    if (s > cpu_usages_size)
    {
        s = cpu_usages_size;
    }
    uint32_t total = 0;
    for (int i = 0; i < s; i++)
    {
        total += cpu0_usages[i];
    }
    return (total / s) & 0xff;
}

/**
 * @brief get cpu 1 load average by size
 *
 * @param s count of samples to average
 * @return uint8_t average load
 */
uint8_t AerManager::getCpu1Avg(uint8_t s)
{
    if (s > cpu_usages_size)
    {
        s = cpu_usages_size;
    }
    uint32_t total = 0;
    for (int i = 0; i < s; i++)
    {
        total += cpu1_usages[i];
    }
    return (total / s) & 0xff;
}

// ===========================================================

void AerManager::setGUI(AerGUI *gui)
{
    this->gui = gui;
}
AerGUI *AerManager::getGUI()
{
    return this->gui;
}

void AerManager::setFancyLED(FancyLED *xled)
{
    this->xled = xled;
}
FancyLED *AerManager::getFancyLED()
{
    return this->xled;
}

// ===========================================================

void AerManager::setupStorageObjects(FavsStor *f, CommStor *c, LightsStor *l, BumpStor *b, TempStor *s, SettingsStorage *ss, NetworkingStorage *n)
{
    // set pointers to storage objects
    this->favsObj = f;
    this->commsObj = c;
    this->lightsObj = l;
    this->bumpObj = b;
    this->tempObj = s;
    this->settingsObj = ss;
    this->netObj = n;
}

FavsStor *AerManager::getFavs()
{
    return this->favsObj;
}
CommStor *AerManager::getComms()
{
    return this->commsObj;
}
LightsStor *AerManager::getLights()
{
    return this->lightsObj;
}
BumpStor *AerManager::getBump()
{
    return this->bumpObj;
}
TempStor *AerManager::getTemp()
{
    return this->tempObj;
}
NetworkingStorage *AerManager::getNet()
{
    return this->netObj;
}

// ===========================================================

void AerManager::setDateTime(uint8_t hour,
                             uint8_t minute,
                             uint8_t second,
                             uint8_t year,
                             uint8_t month,
                             uint8_t day)
{
    this->dateTime.hour = hour;
    this->dateTime.minute = minute;
    this->dateTime.second = second;
    this->dateTime.year = year;
    this->dateTime.month = month;
    this->dateTime.day = day;
}

DateTimeRTC AerManager::getDateTime()
{
    return dateTime;
}

// =================================================

void AerManager::setLocalTemps(float tempA, float tempB)
{
    localTemp.tempA = tempA;
    localTemp.tempB = tempB;
    localTemp.tempC = 0;
}

#if AERPID_COUNT == 2
void AerManager::setLocalTemps(float tempA, float tempB, float tempC)
{
    localTemp.tempA = tempA;
    localTemp.tempB = tempB;
    localTemp.tempC = tempC;
}
#endif

// SSR 1 Temp
float AerManager::getLocalTempA()
{
    return localTemp.tempA;
}

// CPU Temp
float AerManager::getLocalTempB()
{
    return localTemp.tempB;
}

#if AERPID_COUNT == 2
// SSR 2 Temp
float AerManager::getLocalTempC()
{
    return localTemp.tempC;
}
#endif

// ===========================================================
// Favs

void AerManager::addWebFName(int f)
{
    switch (f)
    {
    case 0:
        this->webfnames[0] = 1;
        break;
    case 1:
        this->webfnames[1] = 1;
        break;
    case 2:
        this->webfnames[2] = 1;
        break;
    case 3:
        this->webfnames[3] = 1;
        break;
    default:
        break;
    }
}

int AerManager::getWebFnameVal(int f)
{
    return this->webfnames[f];
}

void AerManager::clearWebFNames()
{
    std::fill(this->webfnames, this->webfnames + 4, 0);
}

void AerManager::addWebFTemp(int f)
{
    switch (f)
    {
    case 0:
        this->webftemps[0] = 1;
        break;
    case 1:
        this->webftemps[1] = 1;
        break;
    case 2:
        this->webftemps[2] = 1;
        break;
    case 3:
        this->webftemps[3] = 1;
        break;
    default:
        break;
    }
}

int AerManager::getWebFtempVal(int f)
{
    return this->webftemps[f];
}

void AerManager::clearWebFTemps()
{
    std::fill(this->webftemps, this->webftemps + 4, 0);
}

bool AerManager::lastSensorFault(uint8_t ei)
{
    return lastFault[ei];
}

void AerManager::lastSensorFault(uint8_t ei, bool faulted)
{
    lastFault[ei] = faulted;
}

// ===========================================================

void AerManager::setOpEP(_elm_prot_op_code_t *op)
{
    this->ep_op_code = *op;
}

// Element protection circuit operation state
_elm_prot_op_code_t AerManager::getOpEP()
{
    return this->ep_op_code;
}

// ===========================================================
// Protected Methods and Functions
// =============================

// Initializes the gui class object
void AerManager::init()
{
    this->intialized = true;

    for (int i = 0; i < cpu_usages_size; i++)
    {
        cpu0_usages[i] = 0;
        cpu1_usages[i] = 0;
    }
}

// ===========================================================
// Private Methods and Functions
// =============================

// Shifts elements in array down by one index, leaving index 0 empty.
void AerManager::shiftArrDown(uint8_t *arr)
{
    // int32_t new_arr[cpu_usages_size];
    uint8_t *new_arr = (uint8_t *)malloc(sizeof(uint8_t) * cpu_usages_size);
    uint siz = cpu_usages_size - 1;
    uint j = 0;
    new_arr[j++] = 0;
    for (uint i = 0; i < siz; i++)
    {
        new_arr[j++] = arr[i];
    }
    for (j = 0; j < cpu_usages_size; j++)
    {
        arr[j] = new_arr[j];
    }
    free(new_arr);
}