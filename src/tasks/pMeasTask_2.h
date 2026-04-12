#ifndef _MEASURE_TASK_B_H
#define _MEASURE_TASK_B_H

#if AERPID_COUNT == 2

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "common/datatypes/mutex.h"
#include "core/controllers/AerPID.h"

void measure_task_2(void *pvParameters);

// Task PID Worker
void measure_task_2(void *pvParameters)
{
    while (millis() < 5500 || !xAerPID2.isLoaded())
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    Serial.print(F("AerPID Measure Worker 2 starting on core "));
    Serial.println(xPortGetCoreID());

    for (;;)
    {
        // Tick AerPID
        xAerPID2.tick_measure();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif

#endif