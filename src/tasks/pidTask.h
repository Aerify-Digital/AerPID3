#ifndef _PID_TASK_H
#define _PID_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "common/datatypes/mutex.h"
#include "core/controllers/AerPID.h"

void pid_task(void *pvParameters);

// Task PID Worker
void pid_task(void *pvParameters)
{
    vTaskDelay(3700);

    Serial.print("AerPID Worker starting on core ");
    Serial.println(xPortGetCoreID());

    // initialize
    bool init1 = xAerPID1.init();
#if AERPID_COUNT == 2
    bool init2 = xAerPID2.init();
#else
    bool init2 = true;
#endif
    if (!init1 || !init2)
    {
        Serial.print("Failed Starting AerPID Worker on core ");
        Serial.println(xPortGetCoreID());
        delay(1000);
    }
    else
    {
        Serial.print("Starting AerPID Worker on core ");
        Serial.println(xPortGetCoreID());
        for (;;)
        {
            // Tick AerPID
            xAerPID1.tick();
            vTaskDelay(1 / portTICK_PERIOD_MS);
#if AERPID_COUNT == 2
            xAerPID2.tick();
            vTaskDelay(1 / portTICK_PERIOD_MS);
#endif
        }
    }
    vTaskDelete(NULL);
}

#endif