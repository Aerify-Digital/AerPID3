#ifndef _WEB_FETCH_TASK_H
#define _WEB_FETCH_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "SPI.h"

#include "common/datatypes/mutex.h"

#include <HTTPClient.h>

void webFetch_task(void *pvParameters);

// Web Client Worker
void webFetch_task(void *pvParameters)
{
    vTaskDelay(100);

    Serial.print("Web Fetch Worker starting on core ");
    Serial.println(xPortGetCoreID());

    HTTPClient http;

    http.begin("https://raw.githubusercontent.com/Aerify-Digital/AerPID3/master/version.json");

    Serial.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    if (httpCode > 0)
    {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK)
        {
            String payload = http.getString();
            Serial.println(payload);
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();

    vTaskDelete(NULL);
}

#endif