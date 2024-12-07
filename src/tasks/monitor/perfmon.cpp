#include "perfmon.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "sdkconfig.h"

#include "esp_log.h"

// check interval span lenth in milliseconds
static const uint checkIntervalLength = 250;

// current idle call count per interval
static uint64_t idle0Calls = 0;
static uint64_t idle1Calls = 0;
// current run call count per interval
static uint64_t runn0Calls = 0;
static uint64_t runn1Calls = 0;

static bool calComplete = false;

// Max idle calls.
static const uint64_t MaxIdleCalls = 1033100; // at 240 mhz

static bool idle_task_0()
{
    idle0Calls += 1;
    return false;
}

static bool idle_task_1()
{
    idle1Calls += 1;
    return false;
}

static void runn_task_0()
{
    runn0Calls += 1;
}

static void runn_task_1()
{
    runn1Calls += 1;
}

static void perfmon_task(void *args)
{
    AerManager *_am = (AerManager *)args;

    uint timeSpan = 1000; // default base time-span for MaxIdleCalls
    // setup interval check rate time-span and max-idle-calls per tick check.
    uint interval = checkIntervalLength >= 100 ? checkIntervalLength : 100;
    uint64_t maxIdleCalls0 = MaxIdleCalls * ((float)interval / timeSpan) * 0.70;
    uint64_t maxIdleCalls1 = MaxIdleCalls * ((float)interval / timeSpan) * 1.00;
    timeSpan = interval; // delay between checks
    uint8_t lag = 1;

    while (1)
    {
        // reset idle calls
        float idle0 = idle0Calls;
        float idle1 = idle1Calls;
        idle0Calls = 0;
        idle1Calls = 0;
        // reset run calls
        float run0 = runn0Calls;
        float run1 = runn1Calls;
        runn0Calls = 0;
        runn1Calls = 0;

        // skip if no idle calls
        if (idle0 == 0 || idle1 == 0)
        {
            if (idle0 == 0)
            {
                _am->setCpu0(100);
                _am->addCpu0(100);
            }
            if (idle1 == 0)
            {
                _am->setCpu1(100);
                _am->addCpu1(100);
            }
            vTaskDelay(timeSpan / portTICK_PERIOD_MS);
            continue;
        }

        if (millis() > 3000 && millis() < 9000) // 30000
        {
            // expand idlecalls...
            if (idle0 > maxIdleCalls0 && idle0 < maxIdleCalls0 * 1.72)
            {
                maxIdleCalls0 = (((maxIdleCalls0 * 9) + idle0) * 0.1f);
            }
            if (idle1 > maxIdleCalls1 && idle1 < maxIdleCalls1 * 1.72)
            {
                maxIdleCalls1 = (((maxIdleCalls1 * 9) + idle1) * 0.1f);
            }
        }
        else if (millis() > 30 && millis() < 3000)
        {
            if (idle0 + run0 > maxIdleCalls0)
            {
                maxIdleCalls0 = idle0 + run0;
            }
            if (idle1 + run1 > maxIdleCalls1)
            {
                maxIdleCalls1 = idle1 + run1;
            }
        }

        if (!calComplete && millis() >= 3000)
        {
            calComplete = true;
            esp_deregister_freertos_tick_hook_for_cpu(runn_task_0, 0);
            esp_deregister_freertos_tick_hook_for_cpu(runn_task_1, 1);
        }

        // calculate cpu usages
        int cpu0 = 100.f - idle0 / (maxIdleCalls0 * lag) * 100.f;
        int cpu1 = 100.f - idle1 / (maxIdleCalls1 * lag) * 100.f;

        // log cpu0 usage to storage
        _am->setCpu0(std::max(0, cpu0));
        _am->addCpu0(std::max(0, cpu0));
        // log cpu1 usage to storage
        _am->setCpu1(std::max(0, cpu1));
        _am->addCpu1(std::max(0, cpu1));

        // ESP_LOGI(TAG, "Core 0 at %d%%", cpu0);
        // ESP_LOGI(TAG, "Core 1 at %d%%", cpu1);

        /*if (Serial)
        {
            Serial.print("Core 0 idle ticks: ");
            Serial.print(idle0);
            Serial.print("  ");
            Serial.print("Core 0 at ");
            Serial.print(cpu0, 10);
            Serial.println("%");

            Serial.print("Core 1 idle ticks: ");
            Serial.print(idle1);
            Serial.print("  ");
            Serial.print("Core 1 at ");
            Serial.print(cpu1, 10);
            Serial.println("%");
        }*/

        /*if (millis() > 30000 && cpu0 + cpu1 >= 170)
        {
            if (lag < 10)
            {
                lag++;
                if (cpu0 + cpu1 >= 190)
                {
                    lag++;
                }
            }
            vTaskDelay((timeSpan * lag * 0.1) / portTICK_PERIOD_MS);
        }*/

        if (lag > 1)
        {
            lag--;
        }

        vTaskDelay(timeSpan / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

esp_err_t perfmon_start(AerManager *am, uint32_t stackSize)
{
    ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_task_0, 0));
    ESP_ERROR_CHECK(esp_register_freertos_idle_hook_for_cpu(idle_task_1, 1));

    // FIXME: these break some tasks... hmm
    ESP_ERROR_CHECK(esp_register_freertos_tick_hook_for_cpu(runn_task_0, 0));
    ESP_ERROR_CHECK(esp_register_freertos_tick_hook_for_cpu(runn_task_1, 1));

    xTaskCreate(perfmon_task, "perfmon", stackSize, (void *)am, 10, NULL);
    // xTaskCreatePinnedToCore(perfmon_task, "perfmon", stackSize, (void *)am, 1, NULL, 1);
    return ESP_OK;
}
