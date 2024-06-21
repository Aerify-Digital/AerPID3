#ifndef _LED_TASK_H
#define _LED_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "common/datatypes/mutex.h"
#include "core/controllers/AerManager.h"
#include "core/lighting/FancyLED.h"
#include "storage/lights.h"

void led_task(void *pvParameters);
void tickBacklightLED(AerManager *_am, long &bl_tick, int16_t &targetBacklightLevel, int16_t &priorBacklightLevel);

void led_task(void *pvParameters)
{
    AerManager *_am = (AerManager *)pvParameters; // task parameters
    LightsStor *_lights = _am->getLights();
    delay(250);

    int16_t targetBacklightLevel = 255;
    int16_t priorBacklightLevel = 255;
    long bl_tick = 0;

    FancyLED xled = FancyLED(_lights);

    _am->setFancyLED(&xled);

    // ----------------------------
    // Setup the fancy LED
    uint pixels = xled.init();
    Serial.print(F("[xLED_A] Initialized Pixels "));
    Serial.println(pixels);

    xled.colorPixels(xled.setRGB(0, 255, 0));
    delay(500);

    printf("> Starting Fancy LED task on core %d\n", xPortGetCoreID());
    while (1)
    {
        if (xSemaphoreTake(sys1_mutex, 10) == pdTRUE)
        {
            // tick the fancy leds!
            xled.tickFancyLED(_am, _lights);

            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(15 / portTICK_PERIOD_MS);

        if (xSemaphoreTake(sys1_mutex, 5) == pdTRUE)
        {
            // tick the backlight
            tickBacklightLED(_am, bl_tick, targetBacklightLevel, priorBacklightLevel);

            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

// ===========================================================================
// ===========================================================================

void tickBacklightLED(AerManager *_am, long &bl_tick, int16_t &targetBacklightLevel, int16_t &priorBacklightLevel)
{
    if (millis() > 7000 && millis() - bl_tick > 25)
    {
        AerST7789 *st7789 = _am->getGUI()->getST7789();
        bool blDimEnb = st7789->isBacklightDimOn();
        uint8_t bll = st7789->getBacklightLevel();

        if (!tftSettingsStorage.isLoaded())
        {
            return;
        }

        uint16_t blTimeout = st7789->getBacklightDimTimeout();
        if (st7789->isBacklightDimmed() && _am->getGUI()->activity_counter <= 1)
        {
            st7789->setBacklightDimmed(false);
            st7789->setBacklight(priorBacklightLevel, false);
            priorBacklightLevel = st7789->getBacklightLevelMin();
        }
        // activity_counter = 50 ms interval
        else if ((blDimEnb) && !st7789->isBacklightDimmed() && _am->getGUI()->activity_counter > blTimeout * 20)
        {
            priorBacklightLevel = bll;
            st7789->setBacklightDimmed(true);
            st7789->setBacklight(st7789->getBacklightLevelMin(), false);
        }

        if (targetBacklightLevel == bll)
        {
            bl_tick = millis();
        }
        else
        {
            if (targetBacklightLevel < bll)
            {
                if (targetBacklightLevel < bll * 0.2)
                {
                    targetBacklightLevel += 3;
                }
                else if (targetBacklightLevel < bll * 0.8)
                {
                    targetBacklightLevel += 2;
                }
                else
                {
                    targetBacklightLevel += 1;
                }
                if (targetBacklightLevel > 255)
                {
                    targetBacklightLevel = 255;
                }
            }
            else if (targetBacklightLevel > bll)
            {
                if (targetBacklightLevel > bll * 0.2)
                {
                    targetBacklightLevel -= 3;
                }
                else if (targetBacklightLevel > bll * 0.8)
                {
                    targetBacklightLevel -= 2;
                }
                else
                {
                    targetBacklightLevel -= 1;
                }
                if (targetBacklightLevel < 0)
                {
                    targetBacklightLevel = 0;
                }
            }
            ledcWrite(TFT_BL_PWM_CHAN, targetBacklightLevel);
            bl_tick = millis();
        }
    }
}

#endif