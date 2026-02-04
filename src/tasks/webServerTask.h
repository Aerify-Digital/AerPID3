#ifndef _WEB_SERVER_TASK_H
#define _WEB_SERVER_TASK_H

#include "common/datatypes/mutex.h"
#include "web/webServer.h"

#include "tasks/webFetchTask.h"

// Check if wifi is ready and connected
bool isWifiConnected()
{
    if (WiFi.getMode() == WIFI_MODE_STA && WiFi.status() == WL_CONNECTED)
    {
        return true;
    }
    return false;
}

// Task 1 Worker
void taskWebWorker(void *pvParameters)
{
    while (millis() < 9000)
    {
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    Serial.print(F("Task Web Worker starting on core "));
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

    if (isWifiConnected())
    {
        aerManager.doUpdateCheck();
    }

    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(F("Starting Task Web Worker on core "));
    Serial.println(xPortGetCoreID());
    for (;;)
    { // infinite loop
        if (xSemaphoreTake(sys1_mutex, 100) == pdTRUE)
        {
            if (isWifiConnected() && aerManager.getUpdateState() == UpdateState::UPDATE_CHECK && aerManager.doUpdateCheck())
            {
                xTaskCreate(webFetch_task, "Web_Fetch", 5120, (void *)&aerManager, 10, &webFetchTask);
            }
            webServer.tick();
            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif