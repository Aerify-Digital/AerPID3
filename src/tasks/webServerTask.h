#ifndef _WEB_SERVER_TASK_H
#define _WEB_SERVER_TASK_H

#include "common/datatypes/mutex.h"
#include "web/webServer.h"

#include "tasks/webFetchTask.h"

// Task 1 Worker
void taskWebWorker(void *parameter)
{
    delay(5000);
    Serial.print("Task Web Worker starting on core ");
    Serial.println(xPortGetCoreID());

    // setup web server..
    bool _setup = webServer.setup();
    if (_setup)
    {
        Serial.print(F("Web Server Started at "));
        Serial.print("http://" + webServer.local_IP + ":" + HTTP_ADDRESS);
        Serial.print(F("  with Hostname "));
        Serial.println(webServer.getHost());
        Serial.println(F(" "));
    }
    else
    {
        Serial.println(F("Web Server Failed to Start!!\n"));
        vTaskDelete(NULL);
        return;
    }

    // xTaskCreate(webFetch_task, "Web_Fetch", 8192, (void *)&aerManager, 10, &webFetchTask);

    Serial.print("Starting Task Web Worker on core ");
    Serial.println(xPortGetCoreID());
    for (;;)
    { // infinite loop
        if (xSemaphoreTake(sys1_mutex, 100) == pdTRUE)
        {
            webServer.tick();
            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif