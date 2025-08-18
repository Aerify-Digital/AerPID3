#include "perfmon.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "sdkconfig.h"

#include "esp_log.h"

// check interval span lenth in milliseconds
static const uint checkIntervalLength = 250;

// calibration time span length in milliseconds
static const uint calTimeLengthMillis = 11000;

static const uint64_t MaxIdleCalls = 731000; // Max idle calls per second

// storage vars for call counters
static uint64_t idle0Calls = 0; // current idle call count per interval for cpu 0
static uint64_t idle1Calls = 0; // current idle call count per interval for cpu 1
static uint64_t runn0Calls = 0; // current run call count per interval for cpu 0
static uint64_t runn1Calls = 0; // current run call count per interval for cpu 1

/// @brief Counter for idles on CPU 0
/// @return false
static bool idle_task_0()
{
    idle0Calls += 1;
    return false;
}

/// @brief Counter for idles on CPU 1
/// return false
static bool idle_task_1()
{
    idle1Calls += 1;
    return false;
}

/// @brief Counter run callbacks on CPU 0
static void runn_task_0()
{
    runn0Calls += 1;
}

/// @brief Counter run callbaclks on CPU 1
static void runn_task_1()
{
    runn1Calls += 1;
}

/// @brief The thread runner for the performance monitor task
/// @param args AerManager object
static void perfmon_task(void *args)
{
    AerManager *_am = (AerManager *)args;

    // setup interval check rate time-span and max-idle-calls per tick check.
    const uint timeSpan = checkIntervalLength >= 100 ? checkIntervalLength : 100; // default base time-span for MaxIdleCalls
    const uint interval = timeSpan;
    uint64_t maxIdleCalls0 = MaxIdleCalls * ((float)interval / 1000) * 1.00;
    uint64_t maxIdleCalls1 = MaxIdleCalls * ((float)interval / 1000) * 1.00;

    const long startTime = millis(); // time we entered this thread...
    const uint calSize = (calTimeLengthMillis / interval) + 1;
    uint64_t *idleCallsArr0 = (uint64_t *)malloc(sizeof(uint64_t) * calSize);
    uint64_t *idleCallsArr1 = (uint64_t *)malloc(sizeof(uint64_t) * calSize);

    // seed calibration storage with max call value
    shiftArrDownThenAdd(idleCallsArr0, calSize, maxIdleCalls0);
    shiftArrDownThenAdd(idleCallsArr1, calSize, maxIdleCalls1);

    // set calibrating flag
    _am->setCpuMonitorCalibrating(true);

    while (_am->isCpuMonitorCalibrating())
    {
        // reset idle calls
        const uint64_t idle0 = idle0Calls; // store current idle0Calls
        const uint64_t idle1 = idle1Calls; // store current idle1Calls
        idle0Calls = 0;
        idle1Calls = 0;
        // reset run calls
        const uint64_t run0 = runn0Calls; // store current runn0Calls
        const uint64_t run1 = runn1Calls; // store current runn1Calls
        runn0Calls = 0;
        runn1Calls = 0;

        // store last idle + run readings
        shiftArrDownThenAdd(idleCallsArr0, calSize, idle0 + run0); // shift down and add current idle0Calls
        shiftArrDownThenAdd(idleCallsArr1, calSize, idle1 + run1); // shift down and add current idle1Calls

        // end calibration period
        if (millis() - startTime >= calTimeLengthMillis)
        {
            // resize maxIdleCalls
            maxIdleCalls0 = calcAverage(idleCallsArr0, calSize) * 1.10;
            maxIdleCalls1 = calcAverage(idleCallsArr1, calSize) * 1.00;

            // trigger calibration complete flag
            _am->setCpuMonitorCalibrating(false);

            // unregister the run tick callback
            esp_deregister_freertos_tick_hook_for_cpu(runn_task_0, 0);
            esp_deregister_freertos_tick_hook_for_cpu(runn_task_1, 1);
            // clear idleCallsArr from memory
            free(idleCallsArr0);
            free(idleCallsArr1);

            if (Serial)
            {
                Serial.println(F("[CAL] CPU monitor calibration complete!"));
                Serial.print(F("[CAL] adjusted max idle calls 0 = "));
                Serial.println(maxIdleCalls0);
                Serial.print(F("[CAL] adjusted max idle calls 1 = "));
                Serial.println(maxIdleCalls1);
            }
            // exit this loop and proceed to normal monitoring
            break;
        }

        // calculate cpu usages
        int cpu0 = 100.f - (float)idle0 / ((float)maxIdleCalls0) * 100.f;
        int cpu1 = 100.f - (float)idle1 / ((float)maxIdleCalls1) * 100.f;

        // log cpu0 usage to storage
        _am->setCpu0(std::max(0, cpu0));
        _am->addCpu0(std::max(0, cpu0));
        // log cpu1 usage to storage
        _am->setCpu1(std::max(0, cpu1));
        _am->addCpu1(std::max(0, cpu1));

#ifdef DEGUG_PERFORMANCE_MONITOR
        if (Serial)
        {
            Serial.print(F("[CAL] Core 0 idle ticks: "));
            Serial.print(idle0);
            Serial.print(F("  "));
            Serial.print(F(" run ticks: "));
            Serial.print(run0);
            Serial.print(F("  "));
            Serial.print(F("Core 0 at "));
            Serial.print(cpu0, 10);
            Serial.println(F("%"));

            Serial.print(F("[CAL] Core 1 idle ticks: "));
            Serial.print(idle1);
            Serial.print(F("  "));
            Serial.print(F(" run ticks: "));
            Serial.print(run1);
            Serial.print(F("  "));
            Serial.print(F("Core 1 at "));
            Serial.print(cpu1, 10);
            Serial.println(F("%"));
        }
#endif

        vTaskDelay(timeSpan / portTICK_PERIOD_MS);
    }

    while (1)
    {
        // reset idle calls
        const uint64_t idle0 = idle0Calls;
        const uint64_t idle1 = idle1Calls;
        idle0Calls = 0;
        idle1Calls = 0;

        // calculate cpu usages
        const int cpu0 = 100.f - (float)MIN(idle0, maxIdleCalls0) / ((float)maxIdleCalls0) * 100.f;
        const int cpu1 = 100.f - (float)MIN(idle1, maxIdleCalls1) / ((float)maxIdleCalls1) * 100.f;

        // log cpu0 usage to storage
        _am->setCpu0(std::max(0, cpu0));
        _am->addCpu0(std::max(0, cpu0));
        // log cpu1 usage to storage
        _am->setCpu1(std::max(0, cpu1));
        _am->addCpu1(std::max(0, cpu1));

        // ESP_LOGI(TAG, "Core 0 at %d%%", cpu0);
        // ESP_LOGI(TAG, "Core 1 at %d%%", cpu1);

        vTaskDelay(timeSpan / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

esp_err_t perfmon_start(AerManager *am, uint32_t stackSize, TaskHandle_t *pxCreatedTask)
{
    ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_task_0, 0));
    ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_task_1, 1));

    ESP_ERROR_CHECK(esp_register_freertos_tick_hook_for_cpu(runn_task_0, 0));
    ESP_ERROR_CHECK(esp_register_freertos_tick_hook_for_cpu(runn_task_1, 1));

    xTaskCreate(perfmon_task, "perfmon", stackSize, (void *)am, 10, pxCreatedTask);
    // xTaskCreatePinnedToCore(perfmon_task, "perfmon", stackSize, (void *)am, 10, NULL, 1);
    return ESP_OK;
}
