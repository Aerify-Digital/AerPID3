#ifndef _MEASURE_TASK_A_H
#define _MEASURE_TASK_A_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "common/datatypes/mutex.h"
#include "core/controllers/AerPID.h"

void measure_task_1(void *pvParameters);

// Task PID Worker
void measure_task_1(void *pvParameters)
{
    while (millis() < 5000 || !xAerPID1.isLoaded())
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    Serial.print(F("AerPID Measure Worker 1 starting on core "));
    Serial.println(xPortGetCoreID());

    for (;;)
    {
        // Tick AerPID 1
        xAerPID1.tick_measure();
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif