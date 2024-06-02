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
    vTaskDelay(4000);

    Serial.print("AerPID Worker 2 starting on core ");
    Serial.println(xPortGetCoreID());

    // initialize
    bool init2 = xAerPID2.init();

    if (!init2)
    {
        Serial.print("Failed Starting AerPID Worker 2 on core ");
        Serial.println(xPortGetCoreID());
        delay(1000);
    }
    else
    {
        Serial.print("Starting AerPID Worker 2 on core ");
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