#ifndef _PID_TASK_A_H
#define _PID_TASK_A_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "common/datatypes/mutex.h"
#include "core/controllers/AerPID.h"

void pid_task_1(void *pvParameters);

// Task PID Worker
void pid_task_1(void *pvParameters)
{
    while (millis() < 6000)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
        vTaskDelay(250 / portTICK_PERIOD_MS);

    Serial.print(F("AerPID Worker 1 starting on core "));
    Serial.println(xPortGetCoreID());

    // initialize
    bool init1 = xAerPID1.init();

    if (!init1)
    {
        Serial.print(F("Failed Starting AerPID Worker 1 on core "));
        Serial.println(xPortGetCoreID());
        delay(1000);
    }
    else
    {
        Serial.print(F("Starting AerPID Worker 1 on core "));
        Serial.println(xPortGetCoreID());
        for (;;)
        {
            // Tick AerPID 1
            xAerPID1.tick();
            vTaskDelay(1 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

#endif