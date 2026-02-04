#ifndef _WEB_FETCH_TASK_H
#define _WEB_FETCH_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "SPI.h"

#include "common/datatypes/mutex.h"
#include "common/datatypes/Version.h"

#include <HTTPClient.h>

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void webFetch_task(void *pvParameters);

// Web Client Worker
void webFetch_task(void *pvParameters)
{
    AerManager *_am = (AerManager *)pvParameters; // task parameters

    Serial.print("[HTTP] Web Fetch Worker starting on core ");
    Serial.println(xPortGetCoreID());

    HTTPClient http;

    http.begin("https://raw.githubusercontent.com/Aerify-Digital/AerPID3/master/version.json");

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
            json j = json::parse(payload);
            unsigned long ul_value1 = std::stoul(j["version"]["VER_MAJOR"].get<std::string>()); 
            unsigned long ul_value2 = std::stoul(j["version"]["VER_MINOR"].get<std::string>()); 
            unsigned long ul_value3 = std::stoul(j["version"]["VER_BUILD"].get<std::string>()); 
            uint vMajor = static_cast<unsigned int>(ul_value1);
            uint vMinor = static_cast<unsigned int>(ul_value2);
            uint vBuild = static_cast<unsigned int>(ul_value3);
            j.clear();

            AppVersion *version = new AppVersion(vMajor, vMinor, vBuild);
            _am->setVersionRemote(version);
            _am->doUpdateCheck();

            /*Serial.print("> Remote App Version: ");
            Serial.print(version->get());
            Serial.println("");*/
        }
    }
    else
    {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    http.~HTTPClient();

    vTaskDelete(NULL);
}

#endif