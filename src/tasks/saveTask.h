#ifndef _AER_SAVE_TASK_H
#define _AER_SAVE_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include <PinDefs.h>
#include <setup.h>

#include "storage/flash.h"
#include "storage/pidStor.h"
#include "storage/pwmStor.h"
#include "storage/tempStor.h"
#include "storage/bumpStor.h"
#include "storage/eepromStor.h"
#include "storage/tftSettingsStor.h"

#include "common/datatypes/mutex.h"
#include "common/enum/ThermalUnitsType.h"
#include "core/controllers/AerManager.h"

typedef struct
{
    AerManager *am;
    Flash *flash;
    EepromStorage *eeprom;
} SaveTaskParams;

void save_task(void *pvParameters)
{
    SaveTaskParams *taskParams = (SaveTaskParams *)pvParameters;
    AerManager *am = taskParams->am;

    am->getAerPID(0)->SET_TEMP_MAX = tempStor.getMaxTemp();
    am->getAerPID(0)->SET_TEMP = tempStor.getTemp();
#if AERPID_COUNT == 2
    am->getAerPID(1)->SET_TEMP_MAX = tempStor.getMaxTemp2();
    am->getAerPID(1)->SET_TEMP = tempStor.getTemp2();
#endif

    if (xSemaphoreTake(spi1_mutex, 1000) == pdTRUE)
    {
        // Load the PID values from Flash
        pidStor.load_pid(am->getAerPID(0)->kP, am->getAerPID(0)->kI, am->getAerPID(0)->kD);
        if (am->getAerPID(0)->kP <= 0.1)
        {
            am->getAerPID(0)->kP = 2.0;
        }
        if (am->getAerPID(0)->kI <= 0.0001)
        {
            am->getAerPID(0)->kI = 0.025;
        }
        else if (am->getAerPID(0)->kI >= 1.0001)
        {
            am->getAerPID(0)->kI = 0.025;
        }
        if (am->getAerPID(0)->kD <= 0.1)
        {
            am->getAerPID(0)->kD = 7.0;
        }
        Serial.print("Loaded PID: ");
        Serial.print(" kP ");
        Serial.print(am->getAerPID(0)->kP, 10);
        Serial.print("  kI ");
        Serial.print(am->getAerPID(0)->kI, 10);
        Serial.print("  kD ");
        Serial.println(am->getAerPID(0)->kD, 10);

#if AERPID_COUNT == 2
        pidStor.load_pid_2(am->getAerPID(1)->kP, am->getAerPID(1)->kI, am->getAerPID(1)->kD);
        if (am->getAerPID(1)->kP <= 0.1)
        {
            am->getAerPID(1)->kP = 2.0;
        }
        if (am->getAerPID(1)->kI <= 0.0001)
        {
            am->getAerPID(1)->kI = 0.025;
        }
        if (am->getAerPID(1)->kD <= 0.1)
        {
            am->getAerPID(1)->kD = 7.0;
        }
        Serial.print("Loaded PID: ");
        Serial.print(" kP ");
        Serial.print(am->getAerPID(1)->kP, 10);
        Serial.print("  kI ");
        Serial.print(am->getAerPID(1)->kI, 10);
        Serial.print("  kD ");
        Serial.println(am->getAerPID(1)->kD, 10);
#endif

        if (pwmStor.pwm_exists())
        {
            int freq;
            // load PWM settings from flash
            pwmStor.load_pwm(freq, am->getAerPID(0)->PWM_ScaleFactor, am->getAerPID(0)->PWM_CycleTime, am->getAerPID(0)->AUTO_TUNE_ACTIVE);
            // am->getAerPID(0)->setPwmFreq(freq);

            if (am->getAerPID(0)->PWM_ScaleFactor <= 0)
            {
                am->getAerPID(0)->PWM_ScaleFactor = 0.125;
                am->getAerPID(0)->pwm_saved = false;
                am->setPressTick(100);
            }
            if (am->getAerPID(0)->PWM_CycleTime <= 0)
            {
                am->getAerPID(0)->PWM_CycleTime = 50;
                am->getAerPID(0)->pwm_saved = false;
                am->setPressTick(100);
            }

            Serial.print("Loaded PWM: ");
            Serial.print(" Frequency ");
            Serial.print(am->getAerPID(0)->getPwmFreq(), 10);
            Serial.print("  Scale Factor ");
            Serial.print(am->getAerPID(0)->PWM_ScaleFactor, 10);
            Serial.print("  Cycle Time ");
            Serial.println(am->getAerPID(0)->PWM_CycleTime, 10);
        }

#if AERPID_COUNT == 2
        if (pwmStor.pwm_2_exists())
        {
            int freq;
            // load PWM settings from flash
            pwmStor.load_pwm_2(freq, am->getAerPID(1)->PWM_ScaleFactor, am->getAerPID(1)->PWM_CycleTime, am->getAerPID(1)->AUTO_TUNE_ACTIVE);
            // am->getAerPID(1)->setPwmFreq(freq);

            if (am->getAerPID(1)->PWM_ScaleFactor <= 0)
            {
                am->getAerPID(1)->PWM_ScaleFactor = 0.125;
                am->getAerPID(1)->pwm_saved = false;
                am->setPressTick(100);
            }
            if (am->getAerPID(1)->PWM_CycleTime <= 0)
            {
                am->getAerPID(1)->PWM_CycleTime = 50;
                am->getAerPID(1)->pwm_saved = false;
                am->setPressTick(100);
            }

            Serial.print("Loaded PWM: ");
            Serial.print(" Frequency ");
            Serial.print(am->getAerPID(1)->getPwmFreq(), 10);
            Serial.print("  Scale Factor ");
            Serial.print(am->getAerPID(1)->PWM_ScaleFactor, 10);
            Serial.print("  Cycle Time ");
            Serial.println(am->getAerPID(1)->PWM_CycleTime, 10);
        }
#endif

        // Load settings from flash
        taskParams->flash->load_settings(am->getAerPID(0)->USAGE_DETECT, am->getAerPID(0)->AUTO_OFF_ENB, am->getAerPID(0)->AUTO_OFF_TIME);
        if (am->getAerPID(0)->AUTO_OFF_TIME <= 1)
        {
            am->getAerPID(0)->AUTO_OFF_TIME = 900;
        }
#if AERPID_COUNT == 2
        taskParams->flash->load_settings(am->getAerPID(1)->USAGE_DETECT, am->getAerPID(1)->AUTO_OFF_ENB, am->getAerPID(1)->AUTO_OFF_TIME);
        if (am->getAerPID(1)->AUTO_OFF_TIME <= 1)
        {
            am->getAerPID(1)->AUTO_OFF_TIME = 900;
        }
#endif

        // Set initial PID tunings
        am->getAerPID(0)->setTunings();
#if AERPID_COUNT == 2
        am->getAerPID(1)->setTunings();
#endif

        // set thermal unit type
        am->setReadingType(am->getSettings()->getThermalUnit());

        // init backlight controls...
        tftSettingsStorage.init(&flash);
        tftSettingsStorage.load();
        if (tftSettingsStorage.isLoaded())
        {
            am->getGUI()->getST7789()->setBacklightDim(tftSettingsStorage.getBacklightDim());
            am->getGUI()->getST7789()->setBacklight(tftSettingsStorage.getBacklightLevel(), true);
            am->getGUI()->getST7789()->setBacklightMin(tftSettingsStorage.getBacklightLevelMin());
            am->getGUI()->getST7789()->setBacklightDimTimeout(tftSettingsStorage.getBacklightDimTimeout());
        }
        else
        {
            am->getGUI()->getST7789()->setBacklight(255, true);
        }
        if (am->getGUI()->getST7789()->getBacklightLevel() < 32)
        {
            am->getGUI()->getST7789()->setBacklight(32, true);
        }
        if (am->getGUI()->getST7789()->getBacklightLevelMin() < 4)
        {
            am->getGUI()->getST7789()->setBacklightMin(4);
        }

        xSemaphoreGive(spi1_mutex);
    }

    if (am->getAerPID(0)->SET_TEMP_MAX < 100)
    {
        am->getAerPID(0)->SET_TEMP_MAX = 850;
        tempStor.setMaxTemp(850);
    }
#if AERPID_COUNT == 2
    if (am->getAerPID(1)->SET_TEMP_MAX < 100)
    {
        am->getAerPID(1)->SET_TEMP_MAX = 850;
        tempStor.setMaxTemp(850);
    }
#endif

    Serial.print("> Starting Save task on core ");
    Serial.println(xPortGetCoreID());
    while (1)
    {

        if (am->isPressTickReady() && am->getAerPID(0)->pid_saved == false)
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    pidStor.save_pid(am->getAerPID(0)->kP, am->getAerPID(0)->kI, am->getAerPID(0)->kD);
                    // myPID.SetTunings(aerStor->kP, aerStor->kI, aerStor->kD);
                    am->getAerPID(0)->setTunings();
                    am->getAerPID(0)->pid_saved = true;
                    Serial.print("Saved: ");
                    Serial.print(" kP ");
                    Serial.print(am->getAerPID(0)->kP, 10);
                    Serial.print(" kI ");
                    Serial.print(am->getAerPID(0)->kI, 10);
                    Serial.print(" kD ");
                    Serial.println(am->getAerPID(0)->kD, 10);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
#if AERPID_COUNT == 2
        if (am->isPressTickReady() && am->getAerPID(1)->pid_saved == false)
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    pidStor.save_pid_2(am->getAerPID(1)->kP, am->getAerPID(1)->kI, am->getAerPID(1)->kD);
                    am->getAerPID(1)->setTunings();
                    am->getAerPID(1)->pid_saved = true;
                    Serial.print("Saved: ");
                    Serial.print(" kP ");
                    Serial.print(am->getAerPID(1)->kP, 10);
                    Serial.print(" kI ");
                    Serial.print(am->getAerPID(1)->kI, 10);
                    Serial.print(" kD ");
                    Serial.println(am->getAerPID(1)->kD, 10);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
#endif
        if (am->isPressTickReady() && am->getAerPID(0)->pwm_saved == false)
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    AerPID *aerpid = am->getAerPID(0);
                    pwmStor.save_pwm(aerpid->getPwmFreq(), aerpid->PWM_ScaleFactor, aerpid->PWM_CycleTime, aerpid->AUTO_TUNE_ACTIVE);
                    am->getAerPID(0)->pwm_saved = true;
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
#if AERPID_COUNT == 2
        if (am->isPressTickReady() && am->getAerPID(1)->pwm_saved == false)
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    AerPID *aerpid = am->getAerPID(1);
                    pwmStor.save_pwm_2(aerpid->getPwmFreq(), aerpid->PWM_ScaleFactor, aerpid->PWM_CycleTime, aerpid->AUTO_TUNE_ACTIVE);
                    am->getAerPID(1)->pwm_saved = true;
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
#endif
        if (am->isPressTickReady() && am->updateTempStor(0))
        {
            Serial.println("saving temps...");
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 500) == pdTRUE)
                {
                    tempStor.setTemp(am->getAerPID(0)->SET_TEMP);
                    tempStor.setMaxTemp(am->getAerPID(0)->SET_TEMP_MAX);
                    tempStor.save1();
                    am->updateTempStor(0, false);
                    Serial.println("saved temps...");
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
#if AERPID_COUNT == 2
        if (am->isPressTickReady() && am->updateTempStor(1))
        {
            Serial.println("saving temps...");
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 500) == pdTRUE)
                {
                    tempStor.setTemp2(am->getAerPID(1)->SET_TEMP);
                    tempStor.setMaxTemp2(am->getAerPID(1)->SET_TEMP_MAX);
                    tempStor.save2();
                    am->updateTempStor(1, false);
                    Serial.println("saved temps...");
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
#endif
        if (am->isPressTickReady() && am->updateLStor())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    am->getLights()->save();
                    am->updateLStor(false);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && am->updateFStor())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    am->getFavs()->save();
                    am->updateFStor(false);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && am->updateBumpStor())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    am->getBump()->save();
                    am->updateBumpStor(false);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && !commstor.isWifiSaved())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    commstor.saveWiFi();
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && !commstor.isUsbSaved())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    commstor.saveUSB();
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && !commstor.isBleSaved())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    commstor.saveBT();
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && (am->getSettings()->isNeedSave() || am->updateKnobState() || am->updateUnitsStor()))
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    if (am->updateUnitsStor())
                    {
                        am->getSettings()->setThermalUnit(am->getReadingType(), true);
                    }
                    am->getSettings()->save();
                    am->updateKnobState(false);
                    am->updateUnitsStor(false);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && am->updateFlashStor())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    taskParams->flash->save_settings(am->getAerPID(0)->USAGE_DETECT, am->getAerPID(0)->AUTO_OFF_ENB, am->getAerPID(0)->AUTO_OFF_TIME);
                    am->updateFlashStor(false);
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && am->getNet()->needSave())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    am->getNet()->save();
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }
        if (am->isPressTickReady() && tftSettingsStorage.isNeedSave())
        {
            if (xSemaphoreTake(sys1_mutex, 500) == pdTRUE)
            {
                if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
                {
                    // tftSettingsStorage.setBacklightDim(am->getGUI()->getST7789()->isBacklightDimOn());
                    // tftSettingsStorage.setBacklightLevel(am->getGUI()->getST7789()->getBacklightLevel());
                    // tftSettingsStorage.setBacklightDimTimeout(am->getGUI()->getST7789()->getBacklightDimTimeout());
                    tftSettingsStorage.save();
                    xSemaphoreGive(spi1_mutex);
                }
                xSemaphoreGive(sys1_mutex);
            }
        }

        // tick press tick...
        am->tickPressTick();

        vTaskDelay(25 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif