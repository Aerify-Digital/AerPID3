#ifndef _AER_WORKER_TASK_H
#define _AER_WORKER_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include <PinDefs.h>
#include "common/datatypes/mutex.h"
#include "core/controllers/AerManager.h"

#include "util/time/PCF85063A.h"
#include <LM75.h>

void worker_task(void *pvParameters)
{
    AerManager *_am = (AerManager *)pvParameters; // task parameters

    uint32_t tick = 0;

    // Board Temp Sensors
    double tempA = 0;
    double tempB = 0;
#if AERPID_COUNT == 2
    double tempC = 0;
#endif

    // define LM75 objects
    LM75 sensorA;
    LM75 sensorB;
#if AERPID_COUNT == 2
    LM75 sensorC;
#endif

    // check if mutex is free
    if (xSemaphoreTake(i2c1_mutex, 1000) == pdTRUE)
    {
        // initialize LM75 objects
        sensorA = LM75(LM75_ADDRESS_A);
        sensorB = LM75(LM75_ADDRESS_B);
#if AERPID_COUNT == 2
        sensorC = LM75(LM75_ADDRESS_C);
#endif

        xSemaphoreGive(i2c1_mutex);
    }

    bool fInit = true;
    PCF85063A *rtc; // rtc pointer ref
    // check if mutex is free
    if (xSemaphoreTake(i2c1_mutex, 1000) == pdTRUE)
    {
        PCF85063A _rtc = PCF85063A(); // initialize RTC class
        rtc = &_rtc;                  // set RPC pointer object

        if (!rtc_exists()) // Check if chip exists on bus
        {
            printf("RTC was not found at address %x .\n", PCF85063_I2C_ADDR);
            fInit = false;
        }
        xSemaphoreGive(i2c1_mutex);
    }

    /*if (xSemaphoreTake(i2c1_mutex, 1000) == pdTRUE)
    {
        if (rtc->getYear() < 20 || rtc->getYear() > 2100)
        {
            rtc->reset();
            // (hour, minute, second)
            rtc->setTime(4, 20, 00); // 24H mode, ex. 6:54:00
            // (weekday, day, month, year)
            rtc->setDate(6, 15, 7, 23);
        }
        xSemaphoreGive(i2c1_mutex);
    }*/

    vTaskDelay(400);
    printf("> Starting Real Time Clock task on core %d..\n", xPortGetCoreID());
    while (1 && fInit)
    {
        if (tick-- <= 0)
        {
            tick = 3;
            if (xSemaphoreTake(sys1_mutex, 50) == pdTRUE)
            {
                if (xSemaphoreTake(i2c1_mutex, 100) == pdTRUE)
                {
                    tempA = sensorA.temp(); // cel
                    tempB = sensorB.temp(); // cel
#if AERPID_COUNT == 1
                    _am->setLocalTemps(tempA, tempB);
#endif
#if AERPID_COUNT == 2
                    tempC = sensorC.temp(); // cel
                    _am->setLocalTemps(tempA, tempB, tempC);
#endif
                    xSemaphoreGive(i2c1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }

        if (xSemaphoreTake(sys1_mutex, 50) == pdTRUE)
        {
            if (xSemaphoreTake(i2c1_mutex, 150) == pdTRUE)
            {
                // if (!Wire.busy())
                {
                    if (_am->isUpdateDateTime())
                    {
                        rtc->reset();
                        DateTimeRTC t = _am->getDateTime();
                        rtc->setTime(t.hour, t.minute, t.second);
                        rtc->setDate(0, t.day, t.month, t.year);
                        _am->setUpdateDateTime(false);
                    }
                    else
                    {
                        rtc->readTime();
                        _am->setDateTime(rtc->getHour(), rtc->getMinute(), rtc->getSecond(), rtc->getYear(), rtc->getMonth(), rtc->getDay());
                    }
                }

                /*
                // these prints don't work...
                Serial.printf("%i:", rtc->getHour());
                Serial.printf("%i:", rtc->getMinute());
                Serial.printf("%i  ", rtc->getSecond());
                Serial.printf("%i-", rtc->getYear());
                Serial.printf("%i-", rtc->getMonth());
                Serial.printf("%i", rtc->getDay());
                Serial.println(" ... ");
                */

                xSemaphoreGive(i2c1_mutex);
            }
            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(950 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void print2digits(int number)
{
    if (number >= 0 && number < 10)
    {
        Serial.write('0');
    }
    Serial.print(number);
}

#endif