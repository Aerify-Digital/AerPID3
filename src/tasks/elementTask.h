#ifndef _AER_ELEMENT_TASK_H
#define _AER_ELEMENT_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include <PinDefs.h>
#include "common/datatypes/mutex.h"
#include "core/controllers/AerManager.h"

#if AERPID_COUNT == 1
long bumpCounter[1] = {0};
bool bumpTriggered[1] = {false};
#endif
#if AERPID_COUNT == 2
long bumpCounter[2] = {0, 0};
bool bumpTriggered[2] = {false, false};
#endif

void handleBump(AerManager *am, uint8_t elementIndex)
{
    AerPID *aerpid = am->getAerPID(elementIndex);
    BumpStor *bump = am->getBump();
    bool bumpEnabled = bump->getEnabled(elementIndex);

    if (bumpCounter[elementIndex] > 0)
    {
        bumpCounter[elementIndex]--;
    }
    if (bumpCounter[elementIndex] <= 0 && bumpTriggered[elementIndex])
    {
        bump->setEnabled(elementIndex, false);
    }

    if (bumpEnabled && !bumpTriggered[elementIndex])
    {
        aerpid->SET_TEMP_LAST = aerpid->SET_TEMP;
        double newtemp = bump->getTemp();
        if (aerpid->SET_TEMP + bump->getTemp() > aerpid->SET_TEMP_MAX)
        {
            while (aerpid->SET_TEMP + newtemp > aerpid->SET_TEMP_MAX)
            {
                newtemp--;
            }
        }
        aerpid->SET_TEMP += newtemp;
        bumpCounter[elementIndex] = bump->getTime();
        bumpTriggered[elementIndex] = true;
        printf("Bump Enabled!\n");
    }

    if (!bumpEnabled && bumpTriggered[elementIndex])
    {
        aerpid->SET_TEMP = aerpid->SET_TEMP_LAST;
        bumpCounter[elementIndex] = 0;
        bumpTriggered[elementIndex] = false;
        printf("Bump Disabled!\n");
    }
}

void element_task(void *pvParameters)
{
    while (millis() < 3000)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    AerManager *_am = (AerManager *)pvParameters; // task parameters

    printf("> Starting Element Feature task on core %d..\n", xPortGetCoreID());
    while (1)
    {
        handleBump(_am, 0);
#if AERPID_COUNT == 2
        handleBump(_am, 1);
#endif
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif