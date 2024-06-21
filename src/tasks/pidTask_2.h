#ifndef _PID_TASK_B_H
#define _PID_TASK_B_H

#if AERPID_COUNT == 2

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "common/datatypes/mutex.h"
#include "core/controllers/AerPID.h"

void pid_task_2(void *pvParameters);

// Task PID Worker
void pid_task_2(void *pvParameters)
{
    while (millis() < 7400)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.print(F("AerPID Worker 2 starting on core "));
    Serial.println(xPortGetCoreID());

    // initialize
    bool init2 = xAerPID2.init();

    if (!init2)
    {
        Serial.print(F("Failed Starting AerPID Worker 2 on core "));
        Serial.println(xPortGetCoreID());
        delay(1000);
    }
    else
    {
        Serial.print(F("Starting AerPID Worker 2 on core "));
        Serial.println(xPortGetCoreID());
        for (;;)
        {
            // Tick AerPID
            xAerPID2.tick();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

#endif

#endif