#ifndef _ENCODER_TASK_H
#define _ENCODER_TASK_H

// #define ENCODER_DO_NOT_USE_INTERRUPTS

/**
 * A basic example of using the EncoderButton library.
 */
#include <EncoderButton.h>
#include <PinDefs.h>
#include "common/datatypes/mutex.h"
#include "common/datatypes/PropsTFT.h"
#include "common/enum/menu_definition.h"
#include "screen/gui/gui.h"
#include "serial/SerialCmdOp.h"
#include "core/controllers/AerManager.h"
#include "common/datatypes/DateTimeRTC.h"
#include "common/enum/ThermalUnitsType.h"
#include "storage/tftSettingsStor.h"
#if AERPID_COUNT == 2
#include "storage/fanControlStor.h"
#endif

#include "web/webServer.h"

/**
 * Instatiate an EncoderButton.
 */
EncoderButton eb1(ENC_A, ENC_B, ENC_SW);
EncoderButton bt1(BTN_A);
EncoderButton bt2(BTN_B);

AerManager *enc_am;

// Task
void enc_task(void *pvParameters);

// Specifiy ISR to handle the 'encoder' event
// This is required 1 per encoder
void IRAM_ATTR readEncoderISR()
{
    eb1.EncoderISR();
}
void IRAM_ATTR readButton1ISR()
{
    bt1.EncoderISR();
}
void IRAM_ATTR readButton2ISR()
{
    bt2.EncoderISR();
}

/**
 * Function to handle the 'clicked' event of alt button 1
 */
void onBt1Clicked(EncoderButton &eb)
{
    // Serial.print("bt1 clickCount: ");
    // Serial.println(eb.clickCount());

    AerGUI *enc_aerGUI = enc_am->getGUI();

    enc_aerGUI->activity_counter = 0;
    if (enc_am->isScreenSaverActive())
    {
        enc_aerGUI->updateMenu(true);
        return;
    }
    if (enc_am->getNetVars()->doscan || enc_am->getNetVars()->scanning)
    {
        return;
    }

    if (enc_am->getAerPID(0)->hasFaultError() && !enc_am->getAerPID(0)->hasFaultErrorAlerted())
    {
        enc_am->getAerPID(0)->setFaultErrorAlert(true);
        enc_aerGUI->updateMenu();
        return;
    }
#if AERPID_COUNT == 2
    if (enc_am->getAerPID(1)->hasFaultError() && !enc_am->getAerPID(1)->hasFaultErrorAlerted())
    {
        enc_am->getAerPID(1)->setFaultErrorAlert(true);
        enc_aerGUI->updateMenu();
        return;
    }
#endif

    if (eb.clickCount() >= 3)
    {
        if (xSemaphoreTake(i2c1_mutex, 200) == pdTRUE)
        {
            Wire.beginTransmission(0x2D);
            if (enc_am->isEnabledDTR())
            {
                Wire.write(200);
                enc_am->setEnabledDTR(false);
            }
            else
            {
                Wire.write(100);
                enc_am->setEnabledDTR(true);
            }
            Wire.endTransmission();
            xSemaphoreGive(i2c1_mutex);
        }
        return;
    }

    if (eb.clickCount() > 1)
    {
        return;
    }

    uint8_t elementIndex = enc_aerGUI->getElementIndex();

    if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_LOCAL_TEMPERATURE)
    {
        if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LOCAL_CELSIUS_TOGGLE)
        {
            enc_am->setReadingType(ThermalUnitsType::FAHRENHEIT);
            enc_am->setPressTick(500);
            enc_am->updateUnitsStor(true);
            enc_aerGUI->updateMenu();
        }
        return;
    }
#if AERPID_COUNT == 2
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LOCAL_FAN_CONTROL)
    {
        if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_FAN_CONTROL_SPEED)
        {
            int s = fanControlStorage.getFanSpeed();
            if (s + 8 <= 255)
            {
                s += 10;
            }
            else if (s + 10 >= 255)
            {
                s = 255;
            }
            fanControlStorage.setFanSpeed(s);
            enc_am->setPressTick(100);
            enc_aerGUI->updateMenu();
            return;
        }
    }
#endif
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_KNOB_ADJUST)
    {
        if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_ADJUST_AMOUNT)
        {
            if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT + 1 <= 100)
            {
                enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT++;
                enc_am->setPressTick(500);
                enc_am->updateKnobState(true);
                enc_aerGUI->updateMenu();
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_ADJUST_INVERT)
        {
            enc_am->getSettings()->setKnobInvert(true);
            enc_am->setPressTick(500);
            enc_am->updateKnobState(true);
            enc_aerGUI->updateMenu();
        }
        else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_DIAGRAM_SHOWN)
        {
            enc_am->getSettings()->setKnobShowUI(true);
            enc_am->setPressTick(500);
            enc_am->updateKnobState(true);
            enc_aerGUI->updateMenu();
        }
        return;
    }

    if (enc_aerGUI->getMenuProps()->menuIndex <= 1)
    {
        SerialCmdOp *sco = new SerialCmdOp(COIL_TOGGLE);
        sco->Val(!enc_am->getAerPID(elementIndex)->PID_ON);
        sco->build();
        sco->emit();
        delayMicroseconds(250);
        enc_am->getAerPID(elementIndex)->PID_ON = !enc_am->getAerPID(elementIndex)->PID_ON;
    }
}

/**
 * Function to handle the 'clicked' event of alt button 2
 */
void onBt2Clicked(EncoderButton &eb)
{
    // Serial.print("bt2 clickCount: ");
    // Serial.println(eb.clickCount());

    AerGUI *enc_aerGUI = enc_am->getGUI();

    enc_aerGUI->activity_counter = 0;
    if (enc_am->isScreenSaverActive())
    {
        enc_aerGUI->updateMenu(true);
        return;
    }
    if (enc_am->getNetVars()->doscan || enc_am->getNetVars()->scanning)
    {
        return;
    }

    if (enc_am->getAerPID(0)->hasFaultError() && !enc_am->getAerPID(0)->hasFaultErrorAlerted())
    {
        enc_am->getAerPID(0)->setFaultErrorAlert(true);
        enc_aerGUI->updateMenu();
        return;
    }
#if AERPID_COUNT == 2
    if (enc_am->getAerPID(1)->hasFaultError() && !enc_am->getAerPID(1)->hasFaultErrorAlerted())
    {
        enc_am->getAerPID(1)->setFaultErrorAlert(true);
        enc_aerGUI->updateMenu();
        return;
    }
#endif

    uint8_t elementIndex = enc_aerGUI->getElementIndex();

    if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_LOCAL_TEMPERATURE)
    {
        if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LOCAL_CELSIUS_TOGGLE)
        {
            enc_am->setReadingType(ThermalUnitsType::CELSIUS);
            enc_am->setPressTick(250);
            enc_am->updateUnitsStor(true);
            enc_aerGUI->updateMenu();
        }
        return;
    }
#if AERPID_COUNT == 2
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LOCAL_FAN_CONTROL)
    {
        if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_FAN_CONTROL_SPEED)
        {
            int s = fanControlStorage.getFanSpeed();
            if (s - 10 >= 0)
            {
                s -= 10;
            }
            else if (s - 10 < 0)
            {
                s = 0;
            }
            fanControlStorage.setFanSpeed(s);
            enc_am->setPressTick(100);
            enc_aerGUI->updateMenu();
            return;
        }
    }
#endif
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_KNOB_ADJUST)
    {
        if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_ADJUST_AMOUNT)
        {
            if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT - 1 > 0)
            {
                enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT--;
                enc_am->setPressTick(250);
                enc_am->updateKnobState(true);
                enc_aerGUI->updateMenu();
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_ADJUST_INVERT)
        {
            enc_am->getSettings()->setKnobInvert(false);
            enc_am->setPressTick(500);
            enc_am->updateKnobState(true);
            enc_aerGUI->updateMenu();
        }
        else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_DIAGRAM_SHOWN)
        {
            enc_am->getSettings()->setKnobShowUI(false);
            enc_am->setPressTick(500);
            enc_am->updateKnobState(true);
            enc_aerGUI->updateMenu();
        }
        return;
    }

    if (enc_aerGUI->getMenuProps()->menuIndex <= 1)
    {
        enc_am->getBump()->setEnabled(elementIndex, !enc_am->getBump()->getEnabled(elementIndex));
    }
}

/**
 * A function to handle the 'clicked' event of the encoder.
 * Requires 'EncoderButton&' as its only parameter.
 */
void onEb1Clicked(EncoderButton &eb)
{
    // Serial.print("eb1 clickCount: ");
    // Serial.println(eb.clickCount());

    AerGUI *enc_aerGUI = enc_am->getGUI();

    enc_aerGUI->activity_counter = 0;

    if (!enc_aerGUI->getST7789()->isBacklightDimmed() && enc_aerGUI->activity_counter > enc_aerGUI->getST7789()->getBacklightDimTimeout())
    {
        enc_aerGUI->getST7789()->setBacklightDimmed(true);
        enc_aerGUI->getST7789()->setBacklight(32);
    }

    if (enc_am->isScreenSaverActive())
    {
        enc_aerGUI->updateMenu(true);
        return;
    }
    if (enc_am->getNetVars()->doscan || enc_am->getNetVars()->scanning)
    {
        return;
    }

    if (enc_am->getAerPID(0)->hasFaultError() && !enc_am->getAerPID(0)->hasFaultErrorAlerted())
    {
        enc_am->getAerPID(0)->setFaultErrorAlert(true);
        enc_aerGUI->updateMenu();
        return;
    }
#if AERPID_COUNT == 2
    if (enc_am->getAerPID(1)->hasFaultError() && !enc_am->getAerPID(1)->hasFaultErrorAlerted())
    {
        enc_am->getAerPID(1)->setFaultErrorAlert(true);
        enc_aerGUI->updateMenu();
        return;
    }
#endif

    if (eb.clickCount() == 1)
    {
        // temperature type toggle (C/F)
        if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_LOCAL_TEMPERATURE)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LOCAL_CELSIUS_TOGGLE)
            {
                ThermalUnitsType rt = enc_am->getReadingType();
                if (rt == ThermalUnitsType::CELSIUS)
                {
                    rt = ThermalUnitsType::FAHRENHEIT;
                }
                else if (rt == ThermalUnitsType::FAHRENHEIT)
                {
                    rt = ThermalUnitsType::KELVIN;
                }
                else
                {
                    rt = ThermalUnitsType::CELSIUS;
                }
                enc_am->setReadingType(rt);
                enc_am->setPressTick(300);
                enc_am->updateUnitsStor(true);
                enc_aerGUI->updateMenu();
                return;
            }
        }
#if AERPID_COUNT == 2
        if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LOCAL_FAN_CONTROL)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_FAN_CONTROL_ENABLED)
            {
                fanControlStorage.setFanEnabled(!fanControlStorage.getFanEnabled());
                enc_am->setPressTick(100);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_FAN_CONTROL_MODE)
            {
                uint8_t m = fanControlStorage.getFanMode();
                if (m + 1 > 2)
                {
                    m = 0;
                }
                else
                {
                    m++;
                }
                fanControlStorage.setFanMode(m);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_FAN_CONTROL_SPEED)
            {
                uint8_t s = fanControlStorage.getFanSpeed();
                s += 20;
                fanControlStorage.setFanSpeed(s);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
        }
#endif
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_FACTORY_RESET)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SYSTEM_FACTORY_RESET_CONFIRM)
            {
                enc_aerGUI->updateMenu();
                // TODO: show some message or something...
                enc_am->getSettings()->resetFlash();
                delay(1000);
                esp_restart();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_KNOB_ADJUST)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_ADJUST_INVERT)
            {
                enc_am->getSettings()->setKnobInvert(!enc_am->getSettings()->getKnobInvert());
                enc_am->updateKnobState(true);
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_DIAGRAM_SHOWN)
            {
                enc_am->getSettings()->setKnobShowUI(!enc_am->getSettings()->getKnobShowUI());
                enc_am->updateKnobState(true);
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_KNOB_ADJUST_AMOUNT)
            {
                if (enc_aerGUI->getCursorModify() > 0)
                {
                    enc_aerGUI->clearCursorModify();
                }
                else
                {
                    enc_aerGUI->setCursorModify(MENU_KNOB_ADJUST_AMOUNT);
                }
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_TOGGLE_IDLE)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SYSTEM_TOGGLE_IDLE_VAR)
            {
                bool enb = enc_am->getAerPID(enc_aerGUI->getElementIndex())->AUTO_OFF_ENB;
                enc_am->getAerPID(enc_aerGUI->getElementIndex())->AUTO_OFF_ENB = !enb;
                /*if (enc_aerGUI->getCursorModify() > 0)
                {
                    enc_aerGUI->clearCursorModify();
                }
                else
                {
                    enc_aerGUI->setCursorModify(MENU_SYSTEM_TOGGLE_IDLE_VAR);
                }*/
                enc_am->updateFlashStor(true);
                enc_am->setPressTick(300);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_LENGTH_IDLE)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SYSTEM_LENGTH_IDLE_VAR)
            {
                // enc_am->getAerPID(enc_aerGUI->getElementIndex())->AUTO_OFF_TIME + 1;
                if (enc_aerGUI->getCursorModify() > 0)
                {
                    enc_aerGUI->clearCursorModify();
                }
                else
                {
                    enc_aerGUI->setCursorModify(MENU_SYSTEM_LENGTH_IDLE_VAR);
                }
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_MAIN_LED)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_MAIN_TOGGLE)
            {
                enc_am->getLights()->isEnabled(!enc_am->getLights()->isEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDState(true);
                enc_am->updateLightState(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_DISPLAY_BACKLIGHT)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_DISPLAY_BACKLIGHT_VAR)
            {
                if (enc_aerGUI->getCursorModify() > 0)
                {
                    enc_aerGUI->clearCursorModify();
                }
                else
                {
                    enc_aerGUI->setCursorModify(MENU_DISPLAY_BACKLIGHT_VAR);
                }
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_DISPLAY_BACKLIGHT_MIN_VAR)
            {
                if (enc_aerGUI->getCursorModify() > 0)
                {
                    enc_aerGUI->clearCursorModify();
                }
                else
                {
                    enc_aerGUI->setCursorModify(MENU_DISPLAY_BACKLIGHT_MIN_VAR);
                }
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_DISPLAY_AUTO_DIM)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_DISPLAY_AUTO_DIM_VAR)
            {
                enc_aerGUI->getST7789()->setBacklightDim(!enc_aerGUI->getST7789()->isBacklightDimOn());
                tftSettingsStorage.setBacklightDim(enc_aerGUI->getST7789()->isBacklightDimOn());
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_DISPLAY_TIMEOUT_DIM)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_DISPLAY_TIMEOUT_DIM_VAR)
            {
                if (enc_aerGUI->getCursorModify() > 0)
                {
                    enc_aerGUI->clearCursorModify();
                }
                else
                {
                    enc_aerGUI->setCursorModify(MENU_DISPLAY_TIMEOUT_DIM_VAR);
                }
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_STATIC_ENABLED)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_ENABLED_VAR)
            {
                wifiStorage.setUsingStaticIP(!wifiStorage.isUsingStaticIP());
                enc_am->setPressTick(500);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_STATIC_IP)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_IP_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_STATIC_IP_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    uint8_t i = enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getMenuProps()->menuItemId;
                    enc_aerGUI->updateMenu();
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex > 3)
                    {
                        enc_aerGUI->getMenuProps()->menuItemId = -4;
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex + 1 < 4)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex - 1 >= 0)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex--;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[0] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[1] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[2] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[3] = 0;
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -4)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_IP_SAVE)
            {
                enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                // save and update hostname
                const uint8_t *ip = enc_aerGUI->getMenuProps()->menuItemIpAddr;
                wifiStorage.setLocalIP(IPAddress(ip[0], ip[1], ip[2], ip[3]));
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_WIFI_STATIC_ADDR);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_STATIC_GATEWAY)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_GATEWAY_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_STATIC_GATEWAY_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    uint8_t i = enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex;
                    // enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getNumber(enc_aerGUI->getMenuProps()->menuItemId);
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getMenuProps()->menuItemId;
                    enc_aerGUI->updateMenu();
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex > 3)
                    {
                        enc_aerGUI->getMenuProps()->menuItemId = -4;
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex + 1 < 4)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex - 1 >= 0)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex--;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[0] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[1] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[2] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[3] = 0;
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -4)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_GATEWAY_SAVE)
            {
                enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                // save and update hostname
                const uint8_t *ip = enc_aerGUI->getMenuProps()->menuItemIpAddr;
                wifiStorage.setGatewayIP(IPAddress(ip[0], ip[1], ip[2], ip[3]));
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_WIFI_STATIC_ADDR);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_STATIC_NETMASK)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_NETMASK_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_STATIC_NETMASK_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    uint8_t i = enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex;
                    // enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getNumber(enc_aerGUI->getMenuProps()->menuItemId);
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getMenuProps()->menuItemId;
                    enc_aerGUI->updateMenu();
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex > 3)
                    {
                        enc_aerGUI->getMenuProps()->menuItemId = -4;
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex + 1 < 4)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex - 1 >= 0)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex--;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[0] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[1] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[2] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[3] = 0;
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -4)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_NETMASK_SAVE)
            {
                enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                // save and update hostname
                const uint8_t *ip = enc_aerGUI->getMenuProps()->menuItemIpAddr;
                wifiStorage.setNetMask(IPAddress(ip[0], ip[1], ip[2], ip[3]));
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_WIFI_STATIC_ADDR);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_STATIC_DNS1)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_DNS1_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_STATIC_DNS1_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    uint8_t i = enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex;
                    // enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getNumber(enc_aerGUI->getMenuProps()->menuItemId);
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getMenuProps()->menuItemId;
                    enc_aerGUI->updateMenu();
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex > 3)
                    {
                        enc_aerGUI->getMenuProps()->menuItemId = -4;
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex + 1 < 4)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex - 1 >= 0)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex--;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[0] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[1] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[2] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[3] = 0;
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -4)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_DNS1_SAVE)
            {
                enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                // save and update hostname
                const uint8_t *ip = enc_aerGUI->getMenuProps()->menuItemIpAddr;
                wifiStorage.setDNS1(IPAddress(ip[0], ip[1], ip[2], ip[3]));
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_WIFI_STATIC_ADDR);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_STATIC_DNS2)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_DNS2_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_STATIC_DNS2_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    uint8_t i = enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex;
                    // enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getNumber(enc_aerGUI->getMenuProps()->menuItemId);
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[i] = enc_aerGUI->getMenuProps()->menuItemId;
                    enc_aerGUI->updateMenu();
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex > 3)
                    {
                        enc_aerGUI->getMenuProps()->menuItemId = -4;
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex + 1 < 4)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex++;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    if (enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex - 1 >= 0)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex--;
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[0] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[1] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[2] = 0;
                    enc_aerGUI->getMenuProps()->menuItemIpAddr[3] = 0;
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -4)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_STATIC_DNS2_SAVE)
            {
                enc_aerGUI->getMenuProps()->menuItemSelIpAddrIndex = 0;
                // save and update hostname
                const uint8_t *ip = enc_aerGUI->getMenuProps()->menuItemIpAddr;
                wifiStorage.setDNS2(IPAddress(ip[0], ip[1], ip[2], ip[3]));
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_WIFI_STATIC_ADDR);
                return;
            }
        }
        /*else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LED_AMBIENT)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_AMBIENT_TOGGLE)
            {
                if (!enc_am->getLights()->ambientEnabled())
                {
                    enc_am->getLights()->presetsEnabled(false);
                    enc_am->getLights()->statusEnabled(false);
                }
                enc_am->getLights()->ambientEnabled(!enc_am->getLights()->ambientEnabled());
                enc_am->updateLStor(true);
                enc_am->setPressTick(1000);
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_ABLINK)
            {
                if (!enc_am->getLights()->amBlinkEnabled())
                {
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->amBlinkEnabled(!enc_am->getLights()->amBlinkEnabled());
                enc_am->updateLStor(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_APULSE)
            {
                if (!enc_am->getLights()->amPulseEnabled())
                {
                    enc_am->getLights()->amBlinkEnabled(false);
                }
                enc_am->getLights()->amPulseEnabled(!enc_am->getLights()->amPulseEnabled());
                enc_am->updateLStor(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
        }*/
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LED_PRESET)
        {
            /*if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_PRESET_TOGGLE)
            {
                if (!enc_am->getLights()->presetsEnabled())
                {
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->statusEnabled(false);
                }
                enc_am->getLights()->presetsEnabled(!enc_am->getLights()->presetsEnabled());
                enc_am->updateLStor(true);
                enc_am->setPressTick(1000);
                return;
            }*/
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_RWAVE)
            {
                if (!enc_am->getLights()->preRainEnabled())
                {
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->prePulseEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->preRainEnabled(!enc_am->getLights()->preRainEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_RAINBOW)
            {
                if (!enc_am->getLights()->preRain2Enabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->prePulseEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->preRain2Enabled(!enc_am->getLights()->preRain2Enabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_RPULSE)
            {
                if (!enc_am->getLights()->preRain3Enabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->prePulseEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->preRain3Enabled(!enc_am->getLights()->preRain3Enabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_CSHIFT)
            {
                if (!enc_am->getLights()->preShiftEnabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->prePulseEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->preShiftEnabled(!enc_am->getLights()->preShiftEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_CPULSE)
            {
                if (!enc_am->getLights()->prePulseEnabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->prePulseEnabled(!enc_am->getLights()->prePulseEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_ASTATIC)
            {
                if (!enc_am->getLights()->ambientEnabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->ambientEnabled(!enc_am->getLights()->ambientEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_ABLINK)
            {
                if (!enc_am->getLights()->amBlinkEnabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amPulseEnabled(false);
                }
                enc_am->getLights()->amBlinkEnabled(!enc_am->getLights()->amBlinkEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_APULSE)
            {
                if (!enc_am->getLights()->amPulseEnabled())
                {
                    enc_am->getLights()->preRainEnabled(false);
                    enc_am->getLights()->preRain2Enabled(false);
                    enc_am->getLights()->preRain3Enabled(false);
                    enc_am->getLights()->preShiftEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                    enc_am->getLights()->amBlinkEnabled(false);
                }
                enc_am->getLights()->amPulseEnabled(!enc_am->getLights()->amPulseEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDMode(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LED_STATUS)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_LED_STATUS_TOGGLE)
            {
                /*if (!enc_am->getLights()->statusEnabled())
                {
                    enc_am->getLights()->presetsEnabled(false);
                    enc_am->getLights()->ambientEnabled(false);
                }*/
                enc_am->getLights()->statusEnabled(!enc_am->getLights()->statusEnabled());
                enc_am->updateLStor(true);
                enc_am->webUpdateLEDStatus(true);
                enc_am->setPressTick(1000);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_SYSTEM_DISPLAY_SETTINGS)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_DISPLAY_SYS_TRAY)
            {
                uint8_t opt = enc_am->getSettings()->getDispTypeOptions();
                if (opt == 0)
                {
                    opt = 1;
                }
                else if (opt == 1)
                {
                    opt = 0;
                }
                else
                {
                    opt = 0;
                }
                enc_am->getSettings()->setDispTypeOptions(opt);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_DISPLAY_SCREENSAVER)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SCREENSAVER_ENABLED)
            {
                enc_am->getSettings()->setScreenSaverEnb(!enc_am->getSettings()->getScreenSaverEnb());
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SCREENSAVER_TOGGLE)
            {
                enc_aerGUI->gotoMenu(1);
                enc_aerGUI->activity_counter = enc_am->getSettings()->getTimeTillScrnSaverOn() + 1;
                enc_am->setScreenSaverActive(true);
                enc_am->setPressTick(200);
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SCREENSAVER_TIME)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_SCREENSAVER_TIME);
                }
                else
                {
                    enc_aerGUI->clearCursorModify();
                }
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SCREENSAVER_STARS)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_SCREENSAVER_STARS);
                }
                else
                {
                    enc_aerGUI->clearCursorModify();
                }
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_MAIN_USB)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_USB_ENABLE)
            {
                enc_am->getComms()->setUSBEn(!enc_am->getComms()->getUSBEn());
                enc_am->webUpdateUSB(true);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_USB_MONITOR)
            {
                enc_am->getComms()->setUSBMon(!enc_am->getComms()->getUSBMon());
                enc_am->webUpdateUSB(true);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_USB_DISCOVER)
            {
                enc_am->getComms()->setUSBDis(!enc_am->getComms()->getUSBDis());
                enc_am->webUpdateUSB(true);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_MAIN_BLE)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_BLE_ENABLE)
            {
                enc_am->getComms()->setBTEn(!enc_am->getComms()->getBTEn());
                enc_am->webUpdateBLE(true);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_BLE_MONITOR)
            {
                enc_am->getComms()->setBTMon(!enc_am->getComms()->getBTMon());
                enc_am->webUpdateBLE(true);
                enc_am->setPressTick(200);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_MAIN_POWER)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_SYS_RESET)
            {
                ESP.restart();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_NETWORKS_NEARBY)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal != MENU_MAIN_WIFI)
            {
                // handle ssid select
                uint16_t sel = enc_aerGUI->getMenuProps()->menuLevelVal;
                String selSSID = enc_am->getNetVars()->scanResults[(int)sel - 1000];
                commstor.setSSID(selSSID.c_str());
                // commstor.setSSIDSet(true);
                enc_am->webUpdateWIFI(true);
                enc_am->setPressTick(300);
                enc_aerGUI->gotoMenu(MENU_MAIN_WIFI, MENU_WIFI_NETWORKS_NEARBY, 3);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_MAIN_WIFI)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_IP_ADDR)
            {
                // ignore
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_MAC_ADDR)
            {
                // ignore
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_NETWORK_SCAN)
            {
                if (commstor.getWifiEn())
                {
                    if (enc_am->getAerPID(0)->PID_ON)
                    {
                        // ignore if pid on...
                        return;
                    }
#if AERPID_COUNT == 2
                    if (enc_am->getAerPID(1)->PID_ON)
                    {
                        // ignore if pid on...
                        return;
                    }
#endif
                    // trigger scan
                    enc_am->getNetVars()->scanned = false;
                    enc_am->getNetVars()->doscan = true;
                }
                else
                {
                    return;
                }
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_NETWORKS_NEARBY)
            {
                // check if server running and safe to check
                if (!webServer.isRunning())
                {
                    return;
                }
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_PASSWORD_NETWORK_JOIN)
            {
                if (commstor.hasWifiSetup())
                {
                    // trigger network join
                    commstor.setSSIDSet(true);
                    commstor.setWifiEn(true);
                    enc_am->webUpdateWIFI(true);
                    enc_am->setPressTick(200);
                    enc_aerGUI->gotoMenu(MENU_WIFI_PASSWORD_NETWORK_JOIN);
                }
                else
                {
                    enc_aerGUI->gotoMenu(MENU_WIFI_PASSWORD_NETWORK_JOIN_ERROR);
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_PASSWORD_NETWORK_JOIN_ERROR)
            {
                enc_aerGUI->gotoMenu(MENU_MAIN_WIFI);
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_PASSWORD_DELETE_STORED)
            {
                // reset ssid and password data
                commstor.setSSIDSet(false);
                commstor.setSSID("\0");
                commstor.setPSK("\0");
                enc_am->webUpdateWIFI(true);
                enc_am->setPressTick(100);
                enc_aerGUI->gotoMenu(MENU_WIFI_PASSWORD_DELETE_STORED_ACK);
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_NETWORK_ENABLED)
            {
                commstor.setWifiEn(!commstor.getWifiEn());
                enc_am->webUpdateWIFI(true);
                enc_am->setPressTick(100);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_PASSWORD)
        {
            // ssid password edit
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_PASSWORD_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_PASSWORD_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelStr += enc_aerGUI->getCharFromIndex(enc_aerGUI->getMenuProps()->menuItemId, 1);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    uint len = strlen(enc_aerGUI->getMenuProps()->menuItemSelStr.c_str());
                    if (len < 33)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelStr = enc_aerGUI->getMenuProps()->menuItemSelStr.substr(0, len - 1);
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelStr = "";
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_PASSWORD_SAVE)
            {
                commstor.setPSK(enc_aerGUI->getMenuProps()->menuItemSelStr.c_str());
                enc_am->webUpdateWIFI(true);
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_MAIN_WIFI);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_HOSTNAME)
        {
            // hostname edit
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_HOSTNAME_EDIT)
            {
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_WIFI_HOSTNAME_EDIT);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId >= 0)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelStr += enc_aerGUI->getCharFromIndex(enc_aerGUI->getMenuProps()->menuItemId);
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -1)
                {
                    uint len = strlen(enc_aerGUI->getMenuProps()->menuItemSelStr.c_str());
                    if (len < 33)
                    {
                        enc_aerGUI->getMenuProps()->menuItemSelStr = enc_aerGUI->getMenuProps()->menuItemSelStr.substr(0, len - 1);
                        enc_aerGUI->updateMenu();
                    }
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -2)
                {
                    enc_aerGUI->getMenuProps()->menuItemSelStr = "";
                    enc_aerGUI->updateMenu();
                }
                else if (enc_aerGUI->getMenuProps()->menuItemId == -3)
                {
                    enc_aerGUI->getMenuProps()->menuItemId = 0;
                    enc_aerGUI->clearCursorModify();
                    enc_aerGUI->updateMenu();
                }
                return;
            }
            else if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_WIFI_HOSTNAME_SAVE)
            {
                // save and update hostname
                enc_am->getNet()->setHostname(enc_aerGUI->getMenuProps()->menuItemSelStr.c_str());
                enc_am->webUpdateWIFI(true);
                enc_am->setPressTick(200);
                enc_aerGUI->gotoMenu(MENU_MAIN_WIFI);
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_AUTO)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_AUTO_TOGGLE)
            {
                uint8_t elementIndex = enc_aerGUI->getElementIndex();
                enc_am->getAerPID(elementIndex)->AUTO_TUNE_ACTIVE = !enc_am->getAerPID(elementIndex)->AUTO_TUNE_ACTIVE;
                enc_am->getAerPID(elementIndex)->pwm_saved = false;
                enc_am->webUpdatePID(true);
                enc_am->setPressTick(100);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_MEASURE_MODE)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_MEASURE_MODE_VAR)
            {
                if (enc_am->getMeasureMode() == 0)
                {
                    enc_am->setMeasureMode(1);
                }
                else if (enc_am->getMeasureMode() == 1)
                {
                    enc_am->setMeasureMode(2);
                }
                else if (enc_am->getMeasureMode() == 2)
                {
                    enc_am->setMeasureMode(0);
                }
                else
                {
                    enc_am->setMeasureMode(0);
                }
                enc_am->setPressTick(120);
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_PWM_CYCLE)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_PWM_CYCLE_VAR)
            {
                uint8_t elementIndex = enc_aerGUI->getElementIndex();
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_PID_PWM_CYCLE_VAR);
                }
                else
                {
                    enc_aerGUI->clearCursorModify();
                    enc_am->getAerPID(elementIndex)->pwm_saved = false;
                    enc_am->webUpdatePID(true);
                    enc_am->setPressTick(100);
                }
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_PWM_BIAS)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_PWM_BIAS_VAR)
            {
                uint8_t elementIndex = enc_aerGUI->getElementIndex();
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_PID_PWM_BIAS_VAR);
                }
                else
                {
                    enc_aerGUI->clearCursorModify();
                    enc_am->getAerPID(elementIndex)->pwm_saved = false;
                    enc_am->webUpdatePID(true);
                    enc_am->setPressTick(100);
                }
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_WINDUP_LIMIT)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_WINDUP_LIMIT_VAR)
            {
                uint8_t elementIndex = enc_aerGUI->getElementIndex();
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_PID_WINDUP_LIMIT_VAR);
                }
                else
                {
                    enc_aerGUI->clearCursorModify();
                    enc_am->getAerPID(elementIndex)->pwm_saved = false;
                    enc_am->webUpdatePID(true);
                    enc_am->setPressTick(100);
                }
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_PWM_FACTOR)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_PWM_FACTOR_VAR)
            {
                uint8_t elementIndex = enc_aerGUI->getElementIndex();
                if (!enc_aerGUI->isCursorModify())
                {
                    enc_aerGUI->setCursorModify(MENU_PID_PWM_FACTOR_VAR);
                }
                else
                {
                    enc_aerGUI->clearCursorModify();
                    enc_am->getAerPID(elementIndex)->pwm_saved = false;
                    enc_am->webUpdatePID(true);
                    enc_am->setPressTick(100);
                }
                enc_aerGUI->updateMenu();
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_PWM_FREQ)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_PID_PWM_FREQ_VAR)
            {
                // this is for display only...
                return;
            }
        }

        // Fav Temp Name selections.
        if (enc_aerGUI->getMenuProps()->menuLevelVal >= 240 && enc_aerGUI->getMenuProps()->menuLevelVal <= 275) // (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_FAV_NAME_FAVORITE1)
        {
            String sname = enc_aerGUI->getMenuName(enc_aerGUI->getMenuProps()->menuLevelVal);
            char *name = new char[sname.length() + 1];
            strcpy(name, sname.c_str());
            uint16_t menuindex = 0;
            if (enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_1N)
            {
                enc_am->getFavs()->setFavName(name, 1);
                enc_am->addWebFName(0);
                menuindex = MENU_FAV_1;
            }
            else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_2N)
            {
                enc_am->getFavs()->setFavName(name, 2);
                enc_am->addWebFName(1);
                menuindex = MENU_FAV_2;
            }
            else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_3N)
            {
                enc_am->getFavs()->setFavName(name, 3);
                enc_am->addWebFName(2);
                menuindex = MENU_FAV_3;
            }
            else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_4N)
            {
                enc_am->getFavs()->setFavName(name, 4);
                enc_am->addWebFName(3);
                menuindex = MENU_FAV_4;
            }
            enc_am->updateFStor(true);
            enc_am->webUpdateFavs(true);
            enc_am->setPressTick(500);
            enc_aerGUI->gotoMenu(menuindex, MENU_MAIN_FAVS);
            return;
        }

        // Bump Amount and Length
        if (enc_aerGUI->getMenuProps()->menuIndex == MENU_BUMP_AMOUNT || enc_aerGUI->getMenuProps()->menuIndex == MENU_BUMP_TIME)
        {
            enc_am->updateBumpStor(true);
            enc_am->webUpdateBump(true);
            enc_aerGUI->selectMenuLevel();
            return;
        }

        // Fav Temp Save
        if (enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_1T || enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_2T || enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_3T || enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_4T)
        {
            enc_am->updateFStor(true);
            enc_am->webUpdateFavs(true);
            enc_am->setPressTick(1000);
            enc_aerGUI->selectMenuLevel();
            return;
        }

        if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LED_BRIGHTNESS)
        {
            enc_am->updateLStor(true);
            enc_am->webUpdateLEDBright(true);
            enc_am->setPressTick(1000);
            enc_aerGUI->selectMenuLevel();
            return;
        }

        if (enc_aerGUI->getMenuProps()->menuLevelId == 0)
        {
            enc_aerGUI->selectMenuLevel();
            return;
        }

        // Date Time set handles
        if (enc_aerGUI->getMenuProps()->menuIndex == MENU_RTC_SET_DATE_YEAR)
        {
            DateTimeRTC t = enc_am->getDateTime();
            int lvl = enc_aerGUI->getMenuProps()->menuLevelVal;
            int val = lvl - 50;
            int year = 23 + val;
            enc_am->setDateTime(t.hour, t.minute, t.second, year, t.month, t.day);
            enc_am->setUpdateDateTime(true);
            // enc_aerGUI->updateDisplayMenu(MENU_RTC_DATE_SET, 0);
            enc_aerGUI->gotoMenu(MENU_RTC_DATE_SET, MENU_MAIN_CLOCK);
            return;
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_RTC_SET_DATE_MONTH)
        {
            DateTimeRTC t = enc_am->getDateTime();
            int lvl = enc_aerGUI->getMenuProps()->menuLevelVal;
            int val = lvl - 50;
            enc_am->setDateTime(t.hour, t.minute, t.second, t.year, val, t.day);
            enc_am->setUpdateDateTime(true);
            // enc_aerGUI->updateDisplayMenu(MENU_RTC_DATE_SET, 0);
            enc_aerGUI->gotoMenu(MENU_RTC_DATE_SET, MENU_MAIN_CLOCK);
            return;
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_RTC_SET_DATE_DAY)
        {
            DateTimeRTC t = enc_am->getDateTime();
            int lvl = enc_aerGUI->getMenuProps()->menuLevelVal;
            int val = lvl - 50;
            enc_am->setDateTime(t.hour, t.minute, t.second, t.year, t.month, val);
            enc_am->setUpdateDateTime(true);
            // enc_aerGUI->updateDisplayMenu(MENU_RTC_DATE_SET, 0);
            enc_aerGUI->gotoMenu(MENU_RTC_DATE_SET, MENU_MAIN_CLOCK);
            return;
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_RTC_SET_TIME_HOUR)
        {
            DateTimeRTC t = enc_am->getDateTime();
            int lvl = enc_aerGUI->getMenuProps()->menuLevelVal;
            int val = lvl - 50;
            enc_am->setDateTime(val, t.minute, t.second, t.year, t.month, t.day);
            enc_am->setUpdateDateTime(true);
            // enc_aerGUI->updateDisplayMenu(MENU_RTC_TIME_SET, 0);
            enc_aerGUI->gotoMenu(MENU_RTC_TIME_SET, MENU_MAIN_CLOCK);
            return;
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_RTC_SET_TIME_MINUTE)
        {
            DateTimeRTC t = enc_am->getDateTime();
            int lvl = enc_aerGUI->getMenuProps()->menuLevelVal;
            int val = lvl - 50;
            enc_am->setDateTime(t.hour, val, t.second, t.year, t.month, t.day);
            enc_am->setUpdateDateTime(true);
            // enc_aerGUI->updateDisplayMenu(MENU_RTC_TIME_SET, 0);
            enc_aerGUI->gotoMenu(MENU_RTC_TIME_SET, MENU_MAIN_CLOCK);
            return;
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_RTC_SET_TIME_SECOND)
        {
            DateTimeRTC t = enc_am->getDateTime();
            int lvl = enc_aerGUI->getMenuProps()->menuLevelVal;
            int val = lvl - 50;
            enc_am->setDateTime(t.hour, t.minute, val, t.year, t.month, t.day);
            enc_am->setUpdateDateTime(true);
            // enc_aerGUI->updateDisplayMenu(MENU_RTC_TIME_SET, 0);
            enc_aerGUI->gotoMenu(MENU_RTC_TIME_SET, MENU_MAIN_CLOCK);
            return;
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_MAIN_CLOCK)
        {
            if (enc_aerGUI->getMenuProps()->menuLevelVal == MENU_RTC_CURRENT)
            {
                return;
            }
        }
        else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LED_COLOR)
        {
            // stuff
            switch (enc_aerGUI->getMenuProps()->menuLevelVal)
            {
            case MENU_COLOR_MAROON:
                lightstor.setRgbVal('r', 128);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_DARK_RED:
                lightstor.setRgbVal('r', 139);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_BROWN:
                lightstor.setRgbVal('r', 165);
                lightstor.setRgbVal('g', 42);
                lightstor.setRgbVal('b', 42);
                break;
            case MENU_COLOR_FIREBRICK:
                lightstor.setRgbVal('r', 178);
                lightstor.setRgbVal('g', 34);
                lightstor.setRgbVal('b', 34);
                break;
            case MENU_COLOR_CRIMSON:
                lightstor.setRgbVal('r', 220);
                lightstor.setRgbVal('g', 20);
                lightstor.setRgbVal('b', 60);
                break;
            case MENU_COLOR_RED:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_TOMATO:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 99);
                lightstor.setRgbVal('b', 71);
                break;
            case MENU_COLOR_CORAL:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 127);
                lightstor.setRgbVal('b', 80);
                break;
            case MENU_COLOR_INDIAN_RED:
                lightstor.setRgbVal('r', 205);
                lightstor.setRgbVal('g', 92);
                lightstor.setRgbVal('b', 92);
                break;
            case MENU_COLOR_LIGHT_CORAL:
                lightstor.setRgbVal('r', 240);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 128);
                break;
            case MENU_COLOR_DARK_SALMON:
                lightstor.setRgbVal('r', 233);
                lightstor.setRgbVal('g', 150);
                lightstor.setRgbVal('b', 122);
                break;
            case MENU_COLOR_SALMON:
                lightstor.setRgbVal('r', 250);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 114);
                break;
            case MENU_COLOR_LIGHT_SALMON:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 160);
                lightstor.setRgbVal('b', 122);
                break;
            case MENU_COLOR_ORANGE_RED:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 69);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_DARK_ORANGE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 140);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_ORANGE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 165);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_GOLD:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 215);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_DARK_GOLDEN_ROD:
                lightstor.setRgbVal('r', 184);
                lightstor.setRgbVal('g', 134);
                lightstor.setRgbVal('b', 11);
                break;
            case MENU_COLOR_GOLDEN_ROD:
                lightstor.setRgbVal('r', 218);
                lightstor.setRgbVal('g', 165);
                lightstor.setRgbVal('b', 32);
                break;
            case MENU_COLOR_PALE_GOLDEN_ROD:
                lightstor.setRgbVal('r', 238);
                lightstor.setRgbVal('g', 232);
                lightstor.setRgbVal('b', 170);
                break;
            case MENU_COLOR_DARK_KHAKI:
                lightstor.setRgbVal('r', 189);
                lightstor.setRgbVal('g', 183);
                lightstor.setRgbVal('b', 107);
                break;
            case MENU_COLOR_KHAKI:
                lightstor.setRgbVal('r', 240);
                lightstor.setRgbVal('g', 230);
                lightstor.setRgbVal('b', 140);
                break;
            case MENU_COLOR_OLIVE:
                lightstor.setRgbVal('r', 128);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_YELLOW:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_YELLOW_GREEN:
                lightstor.setRgbVal('r', 154);
                lightstor.setRgbVal('g', 205);
                lightstor.setRgbVal('b', 50);
                break;
            case MENU_COLOR_DARK_OLIVE_GREEN:
                lightstor.setRgbVal('r', 85);
                lightstor.setRgbVal('g', 107);
                lightstor.setRgbVal('b', 47);
                break;
            case MENU_COLOR_OLIVE_DRAB:
                lightstor.setRgbVal('r', 107);
                lightstor.setRgbVal('g', 142);
                lightstor.setRgbVal('b', 35);
                break;
            case MENU_COLOR_LAWN_GREEN:
                lightstor.setRgbVal('r', 124);
                lightstor.setRgbVal('g', 252);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_CHARTREUSE:
                lightstor.setRgbVal('r', 127);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_GREEN_YELLOW:
                lightstor.setRgbVal('r', 173);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 47);
                break;
            case MENU_COLOR_DARK_GREEN:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 100);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_GREEN:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_FOREST_GREEN:
                lightstor.setRgbVal('r', 34);
                lightstor.setRgbVal('g', 139);
                lightstor.setRgbVal('b', 34);
                break;
            case MENU_COLOR_LIME:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_LIME_GREEN:
                lightstor.setRgbVal('r', 50);
                lightstor.setRgbVal('g', 205);
                lightstor.setRgbVal('b', 50);
                break;
            case MENU_COLOR_LIGHT_GREEN:
                lightstor.setRgbVal('r', 144);
                lightstor.setRgbVal('g', 238);
                lightstor.setRgbVal('b', 144);
                break;
            case MENU_COLOR_PALE_GREEN:
                lightstor.setRgbVal('r', 152);
                lightstor.setRgbVal('g', 251);
                lightstor.setRgbVal('b', 152);
                break;
            case MENU_COLOR_DARK_SEA_GREEN:
                lightstor.setRgbVal('r', 143);
                lightstor.setRgbVal('g', 188);
                lightstor.setRgbVal('b', 143);
                break;
            case MENU_COLOR_MEDIUM_SPRING_GREEN:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 250);
                lightstor.setRgbVal('b', 154);
                break;
            case MENU_COLOR_SPRING_GREEN:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 127);
                break;
            case MENU_COLOR_SEA_GREEN:
                lightstor.setRgbVal('r', 46);
                lightstor.setRgbVal('g', 139);
                lightstor.setRgbVal('b', 87);
                break;
            case MENU_COLOR_MEDIUM_AQUA_MARINE:
                lightstor.setRgbVal('r', 102);
                lightstor.setRgbVal('g', 205);
                lightstor.setRgbVal('b', 170);
                break;
            case MENU_COLOR_MEDIUM_SEA_GREEN:
                lightstor.setRgbVal('r', 60);
                lightstor.setRgbVal('g', 179);
                lightstor.setRgbVal('b', 113);
                break;
            case MENU_COLOR_LIGHT_SEA_GREEN:
                lightstor.setRgbVal('r', 32);
                lightstor.setRgbVal('g', 178);
                lightstor.setRgbVal('b', 170);
                break;
            case MENU_COLOR_DARK_SLATE_GRAY:
                lightstor.setRgbVal('r', 47);
                lightstor.setRgbVal('g', 79);
                lightstor.setRgbVal('b', 79);
                break;
            case MENU_COLOR_TEAL:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 128);
                break;
            case MENU_COLOR_DARK_CYAN:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 139);
                lightstor.setRgbVal('b', 139);
                break;
            case MENU_COLOR_AQUA:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_CYAN:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_DARK_TURQUOISE:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 206);
                lightstor.setRgbVal('b', 209);
                break;
            case MENU_COLOR_TURQUOISE:
                lightstor.setRgbVal('r', 64);
                lightstor.setRgbVal('g', 224);
                lightstor.setRgbVal('b', 208);
                break;
            case MENU_COLOR_MEDIUM_TURQUOISE:
                lightstor.setRgbVal('r', 72);
                lightstor.setRgbVal('g', 209);
                lightstor.setRgbVal('b', 204);
                break;
            case MENU_COLOR_PALE_TURQUOISE:
                lightstor.setRgbVal('r', 175);
                lightstor.setRgbVal('g', 238);
                lightstor.setRgbVal('b', 238);
                break;
            case MENU_COLOR_AQUA_MARINE:
                lightstor.setRgbVal('r', 127);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 212);
                break;
            case MENU_COLOR_POWDER_BLUE:
                lightstor.setRgbVal('r', 176);
                lightstor.setRgbVal('g', 224);
                lightstor.setRgbVal('b', 230);
                break;
            case MENU_COLOR_CADET_BLUE:
                lightstor.setRgbVal('r', 95);
                lightstor.setRgbVal('g', 158);
                lightstor.setRgbVal('b', 160);
                break;
            case MENU_COLOR_STEEL_BLUE:
                lightstor.setRgbVal('r', 70);
                lightstor.setRgbVal('g', 130);
                lightstor.setRgbVal('b', 180);
                break;
            case MENU_COLOR_CORN_FLOWER_BLUE:
                lightstor.setRgbVal('r', 100);
                lightstor.setRgbVal('g', 149);
                lightstor.setRgbVal('b', 237);
                break;
            case MENU_COLOR_DEEP_SKY_BLUE:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 191);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_DODGER_BLUE:
                lightstor.setRgbVal('r', 30);
                lightstor.setRgbVal('g', 144);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_LIGHT_BLUE:
                lightstor.setRgbVal('r', 173);
                lightstor.setRgbVal('g', 216);
                lightstor.setRgbVal('b', 230);
                break;
            case MENU_COLOR_SKY_BLUE:
                lightstor.setRgbVal('r', 135);
                lightstor.setRgbVal('g', 206);
                lightstor.setRgbVal('b', 250);
                break;
            case MENU_COLOR_MIDNIGHT_BLUE:
                lightstor.setRgbVal('r', 25);
                lightstor.setRgbVal('g', 25);
                lightstor.setRgbVal('b', 112);
                break;
            case MENU_COLOR_NAVY:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 128);
                break;
            case MENU_COLOR_DARK_BLUE:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 139);
                break;
            case MENU_COLOR_MEDIUM_BLUE:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 205);
                break;
            case MENU_COLOR_BLUE:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_ROYAL_BLUE:
                lightstor.setRgbVal('r', 65);
                lightstor.setRgbVal('g', 105);
                lightstor.setRgbVal('b', 225);
                break;
            case MENU_COLOR_BLUE_VIOLET:
                lightstor.setRgbVal('r', 138);
                lightstor.setRgbVal('g', 43);
                lightstor.setRgbVal('b', 226);
                break;
            case MENU_COLOR_INDIGO:
                lightstor.setRgbVal('r', 75);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 130);
                break;
            case MENU_COLOR_DARK_SLATE_BLUE:
                lightstor.setRgbVal('r', 72);
                lightstor.setRgbVal('g', 61);
                lightstor.setRgbVal('b', 139);
                break;
            case MENU_COLOR_SLATE_BLUE:
                lightstor.setRgbVal('r', 106);
                lightstor.setRgbVal('g', 90);
                lightstor.setRgbVal('b', 205);
                break;
            case MENU_COLOR_MEDIUM_SLATE_BLUE:
                lightstor.setRgbVal('r', 123);
                lightstor.setRgbVal('g', 104);
                lightstor.setRgbVal('b', 238);
                break;
            case MENU_COLOR_MEDIUM_PURPLE:
                lightstor.setRgbVal('r', 147);
                lightstor.setRgbVal('g', 112);
                lightstor.setRgbVal('b', 219);
                break;
            case MENU_COLOR_DARK_MAGENTA:
                lightstor.setRgbVal('r', 139);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 139);
                break;
            case MENU_COLOR_DARK_VIOLET:
                lightstor.setRgbVal('r', 148);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 211);
                break;
            case MENU_COLOR_DARK_ORCHID:
                lightstor.setRgbVal('r', 153);
                lightstor.setRgbVal('g', 50);
                lightstor.setRgbVal('b', 205);
                break;
            case MENU_COLOR_MEDIUM_ORCHID:
                lightstor.setRgbVal('r', 186);
                lightstor.setRgbVal('g', 85);
                lightstor.setRgbVal('b', 211);
                break;
            case MENU_COLOR_PURPLE:
                lightstor.setRgbVal('r', 128);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 128);
                break;
            case MENU_COLOR_THISTLE:
                lightstor.setRgbVal('r', 216);
                lightstor.setRgbVal('g', 191);
                lightstor.setRgbVal('b', 216);
                break;
            case MENU_COLOR_PLUM:
                lightstor.setRgbVal('r', 221);
                lightstor.setRgbVal('g', 160);
                lightstor.setRgbVal('b', 221);
                break;
            case MENU_COLOR_VIOLET:
                lightstor.setRgbVal('r', 238);
                lightstor.setRgbVal('g', 130);
                lightstor.setRgbVal('b', 238);
                break;
            case MENU_COLOR_MAGENTA:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_ORCHID:
                lightstor.setRgbVal('r', 218);
                lightstor.setRgbVal('g', 112);
                lightstor.setRgbVal('b', 215);
                break;
            case MENU_COLOR_MEDIUM_VIOLET_RED:
                lightstor.setRgbVal('r', 199);
                lightstor.setRgbVal('g', 21);
                lightstor.setRgbVal('b', 133);
                break;
            case MENU_COLOR_PALE_VIOLET_RED:
                lightstor.setRgbVal('r', 219);
                lightstor.setRgbVal('g', 112);
                lightstor.setRgbVal('b', 147);
                break;
            case MENU_COLOR_DEEP_PINK:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 20);
                lightstor.setRgbVal('b', 147);
                break;
            case MENU_COLOR_HOT_PINK:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 105);
                lightstor.setRgbVal('b', 180);
                break;
            case MENU_COLOR_LIGHT_PINK:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 182);
                lightstor.setRgbVal('b', 193);
                break;
            case MENU_COLOR_PINK:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 192);
                lightstor.setRgbVal('b', 203);
                break;
            case MENU_COLOR_ANTIQUE_WHITE:
                lightstor.setRgbVal('r', 250);
                lightstor.setRgbVal('g', 192);
                lightstor.setRgbVal('b', 203);
                break;
            case MENU_COLOR_BEIGE:
                lightstor.setRgbVal('r', 245);
                lightstor.setRgbVal('g', 245);
                lightstor.setRgbVal('b', 220);
                break;
            case MENU_COLOR_BISQUE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 228);
                lightstor.setRgbVal('b', 196);
                break;
            case MENU_COLOR_BLANCHED_ALMOND:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 235);
                lightstor.setRgbVal('b', 205);
                break;
            case MENU_COLOR_WHEAT:
                lightstor.setRgbVal('r', 245);
                lightstor.setRgbVal('g', 222);
                lightstor.setRgbVal('b', 179);
                break;
            case MENU_COLOR_CORN_SILK:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 248);
                lightstor.setRgbVal('b', 220);
                break;
            case MENU_COLOR_LEMON_CHIFFON:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 250);
                lightstor.setRgbVal('b', 205);
                break;
            case MENU_COLOR_LIGHT_GOLDEN_ROD_YELLOW:
                lightstor.setRgbVal('r', 250);
                lightstor.setRgbVal('g', 250);
                lightstor.setRgbVal('b', 210);
                break;
            case MENU_COLOR_LIGHT_YELLOW:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 224);
                break;
            case MENU_COLOR_SADDLE_BROWN:
                lightstor.setRgbVal('r', 139);
                lightstor.setRgbVal('g', 69);
                lightstor.setRgbVal('b', 19);
                break;
            case MENU_COLOR_SIENNA:
                lightstor.setRgbVal('r', 160);
                lightstor.setRgbVal('g', 82);
                lightstor.setRgbVal('b', 45);
                break;
            case MENU_COLOR_CHOCOLATE:
                lightstor.setRgbVal('r', 210);
                lightstor.setRgbVal('g', 105);
                lightstor.setRgbVal('b', 30);
                break;
            case MENU_COLOR_PERU:
                lightstor.setRgbVal('r', 205);
                lightstor.setRgbVal('g', 133);
                lightstor.setRgbVal('b', 63);
                break;
            case MENU_COLOR_SANDY_BROWN:
                lightstor.setRgbVal('r', 244);
                lightstor.setRgbVal('g', 164);
                lightstor.setRgbVal('b', 96);
                break;
            case MENU_COLOR_BURLY_WOOD:
                lightstor.setRgbVal('r', 222);
                lightstor.setRgbVal('g', 184);
                lightstor.setRgbVal('b', 135);
                break;
            case MENU_COLOR_TAN:
                lightstor.setRgbVal('r', 210);
                lightstor.setRgbVal('g', 180);
                lightstor.setRgbVal('b', 140);
                break;
            case MENU_COLOR_ROSY_BROWN:
                lightstor.setRgbVal('r', 188);
                lightstor.setRgbVal('g', 143);
                lightstor.setRgbVal('b', 143);
                break;
            case MENU_COLOR_MOCCASIN:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 228);
                lightstor.setRgbVal('b', 181);
                break;
            case MENU_COLOR_NAVAJO_WHITE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 222);
                lightstor.setRgbVal('b', 173);
                break;
            case MENU_COLOR_PEACH_PUFF:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 218);
                lightstor.setRgbVal('b', 185);
                break;
            case MENU_COLOR_MISTY_ROSE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 228);
                lightstor.setRgbVal('b', 225);
                break;
            case MENU_COLOR_LAVENDER_BLUSH:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 240);
                lightstor.setRgbVal('b', 245);
                break;
            case MENU_COLOR_LINEN:
                lightstor.setRgbVal('r', 250);
                lightstor.setRgbVal('g', 240);
                lightstor.setRgbVal('b', 245);
                break;
            case MENU_COLOR_OLD_LACE:
                lightstor.setRgbVal('r', 253);
                lightstor.setRgbVal('g', 240);
                lightstor.setRgbVal('b', 230);
                break;
            case MENU_COLOR_PAPAYA_WHIP:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 239);
                lightstor.setRgbVal('b', 213);
                break;
            case MENU_COLOR_SEA_SHELL:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 245);
                lightstor.setRgbVal('b', 238);
                break;
            case MENU_COLOR_MINT_CREAM:
                lightstor.setRgbVal('r', 245);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 250);
                break;
            case MENU_COLOR_SLATE_GRAY:
                lightstor.setRgbVal('r', 112);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 144);
                break;
            case MENU_COLOR_LIGHT_SLATE_GRAY:
                lightstor.setRgbVal('r', 119);
                lightstor.setRgbVal('g', 136);
                lightstor.setRgbVal('b', 153);
                break;
            case MENU_COLOR_LIGHT_STEEL_BLUE:
                lightstor.setRgbVal('r', 176);
                lightstor.setRgbVal('g', 196);
                lightstor.setRgbVal('b', 222);
                break;
            case MENU_COLOR_LAVENDER:
                lightstor.setRgbVal('r', 230);
                lightstor.setRgbVal('g', 230);
                lightstor.setRgbVal('b', 250);
                break;
            case MENU_COLOR_FLORAL_WHITE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 250);
                lightstor.setRgbVal('b', 240);
                break;
            case MENU_COLOR_ALICE_BLUE:
                lightstor.setRgbVal('r', 240);
                lightstor.setRgbVal('g', 248);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_GHOST_WHITE:
                lightstor.setRgbVal('r', 248);
                lightstor.setRgbVal('g', 248);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_HONEY_DEW:
                lightstor.setRgbVal('r', 240);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 240);
                break;
            case MENU_COLOR_IVORY:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 240);
                break;
            case MENU_COLOR_AZURE:
                lightstor.setRgbVal('r', 240);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 255);
                break;
            case MENU_COLOR_SNOW:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 250);
                lightstor.setRgbVal('b', 250);
                break;
            case MENU_COLOR_BLACK:
                lightstor.setRgbVal('r', 0);
                lightstor.setRgbVal('g', 0);
                lightstor.setRgbVal('b', 0);
                break;
            case MENU_COLOR_DIM_GRAY:
                lightstor.setRgbVal('r', 105);
                lightstor.setRgbVal('g', 105);
                lightstor.setRgbVal('b', 105);
                break;
            case MENU_COLOR_GRAY:
                lightstor.setRgbVal('r', 128);
                lightstor.setRgbVal('g', 128);
                lightstor.setRgbVal('b', 128);
                break;
            case MENU_COLOR_DARK_GRAY:
                lightstor.setRgbVal('r', 169);
                lightstor.setRgbVal('g', 169);
                lightstor.setRgbVal('b', 169);
                break;
            case MENU_COLOR_SILVER:
                lightstor.setRgbVal('r', 192);
                lightstor.setRgbVal('g', 192);
                lightstor.setRgbVal('b', 192);
                break;
            case MENU_COLOR_LIGHT_GRAY:
                lightstor.setRgbVal('r', 211);
                lightstor.setRgbVal('g', 211);
                lightstor.setRgbVal('b', 211);
                break;
            case MENU_COLOR_GAINSBORO:
                lightstor.setRgbVal('r', 220);
                lightstor.setRgbVal('g', 220);
                lightstor.setRgbVal('b', 220);
                break;
            case MENU_COLOR_WHITE_SMOKE:
                lightstor.setRgbVal('r', 245);
                lightstor.setRgbVal('g', 245);
                lightstor.setRgbVal('b', 245);
                break;
            case MENU_COLOR_WHITE:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 255);
                break;
            default:
                lightstor.setRgbVal('r', 255);
                lightstor.setRgbVal('g', 255);
                lightstor.setRgbVal('b', 255);
                break;
            }

            enc_am->setPressTick(400);
            enc_am->updateLightState(true);
            enc_am->updateLStor(true);
            enc_am->webUpdateLEDColor(true);
            // enc_aerGUI->selectMenuLevel();
            enc_aerGUI->gotoMenu(MENU_LED_SETUP, MENU_LED_COLOR);
            return;
        }

        // select current menu level
        enc_aerGUI->selectMenuLevel();
    }
    else if (eb.clickCount() == 2 && !enc_aerGUI->isCursorModify()) // Menu Back
    {
        if (enc_aerGUI->getMenuProps()->menuIndex > 1)
        {
            AerMenu current = enc_aerGUI->getSelectedMenu();
            if (current.getParent() > 1)
            {
                // enc_aerGUI->updateDisplayMenu(current.getParent(), 0);
                enc_aerGUI->clearCursorModify();
                enc_aerGUI->gotoMenu(current.getParent());
            }
        }
#if AERPID_COUNT == 2
        if (enc_aerGUI->getMenuProps()->menuIndex <= 1)
        {
            uint8_t ei = enc_aerGUI->getElementIndex();
            if (ei == 1)
            {
                ei = 0;
            }
            else if (ei == 0)
            {
                ei = 1;
            }
            enc_aerGUI->setElementIndex(ei);
            // enc_aerGUI->gotoMenu(1);
            enc_aerGUI->updateMenu();
        }
#endif
    }
    else if (eb.clickCount() == 3) // Send to Home Screen
    {
        if (enc_aerGUI->getMenuProps()->menuIndex > 1)
        {
            // enc_aerGUI->updateDisplayMenu(1, 0);
            enc_aerGUI->clearCursorModify();
            enc_aerGUI->gotoMenu(1);
        }
        else
        {
            uint8_t opt = enc_am->getSettings()->getDispTypeOptions();
            if (opt == 0)
            {
                opt = 1;
            }
            else if (opt == 1)
            {
                opt = 0;
            }
            else
            {
                opt = 0;
            }
            enc_am->getSettings()->setDispTypeOptions(opt);
            enc_am->setPressTick(200);
            enc_aerGUI->updateMenu();
        }
    }
}

/**
 * A function to handle the 'encoder' event
 */
void onEb1Encoder(EncoderButton &eb)
{
    // Serial.print("eb1 incremented by: ");
    // Serial.println(eb.increment());
    //  Serial.print("eb1 position is: ");
    //  Serial.println(eb.position());

    AerGUI *enc_aerGUI = enc_am->getGUI();

    enc_aerGUI->activity_counter = 0;
    if (enc_am->isScreenSaverActive())
    {
        enc_aerGUI->updateMenu(true);
        return;
    }

    uint8_t elementIndex = enc_aerGUI->getElementIndex();

    int dir = 1;
    if (enc_am->getSettings()->getKnobInvert())
    {
        dir = -1;
    }

    if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_P)
    {
        if (eb.increment() > 0)
        {
            enc_am->getAerPID(elementIndex)->kP -= 0.1 * dir;
        }
        else if (eb.increment() < 0)
        {
            enc_am->getAerPID(elementIndex)->kP += 0.1 * dir;
        }
        enc_aerGUI->updateMenu();
        enc_am->setPressTick(300);
        enc_am->getAerPID(elementIndex)->pid_saved = false;
        enc_am->webUpdatePID(true);
        return;
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_I)
    {
        if (eb.increment() > 0)
        {
            enc_am->getAerPID(elementIndex)->kI -= 0.001 * dir;
        }
        else if (eb.increment() < 0)
        {
            enc_am->getAerPID(elementIndex)->kI += 0.001 * dir;
        }
        enc_aerGUI->updateMenu();
        enc_am->setPressTick(300);
        enc_am->getAerPID(elementIndex)->pid_saved = false;
        enc_am->webUpdatePID(true);
        return;
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_PID_D)
    {
        if (eb.increment() > 0)
        {
            enc_am->getAerPID(elementIndex)->kD -= 0.5 * dir;
        }
        else if (eb.increment() < 0)
        {
            enc_am->getAerPID(elementIndex)->kD += 0.5 * dir;
        }
        enc_aerGUI->updateMenu();
        enc_am->setPressTick(300);
        enc_am->getAerPID(elementIndex)->pid_saved = false;
        enc_am->webUpdatePID(true);
        return;
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_ROOT || enc_aerGUI->getMenuProps()->menuIndex == MENU_ROOT_HOME)
    {
        if (eb.increment() > 0)
        {
            double amt = enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT * abs(eb.increment());
            if (eb.increment() > 1)
            {
                enc_am->getAerPID(elementIndex)->SET_TEMP -= amt * 2 * dir;
            }
            else
            {
                enc_am->getAerPID(elementIndex)->SET_TEMP -= amt * dir;
            }
        }
        else if (eb.increment() < 0)
        {
            double amt = enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT * abs(eb.increment());
            if (eb.increment() < -1)
            {
                enc_am->getAerPID(elementIndex)->SET_TEMP += amt * 2 * dir;
            }
            else
            {
                enc_am->getAerPID(elementIndex)->SET_TEMP += amt * dir;
            }
        }
        enc_am->setPressTick(400);
        enc_am->updateTempStor(elementIndex, true);
        enc_am->webUpdateTemp(true);
        return;
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_1T || enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_2T || enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_3T || enc_aerGUI->getMenuProps()->menuIndex == MENU_FAV_4T)
    {
        // Meow
        double ftemp = 0;
        int fav = 0;
        switch (enc_aerGUI->getMenuProps()->menuIndex)
        {
        case MENU_FAV_1T:
            ftemp = enc_am->getFavs()->getTemp(1);
            fav = 1;
            enc_am->addWebFTemp(0);
            break;
        case MENU_FAV_2T:
            ftemp = enc_am->getFavs()->getTemp(2);
            fav = 2;
            enc_am->addWebFTemp(1);
            break;
        case MENU_FAV_3T:
            ftemp = enc_am->getFavs()->getTemp(3);
            fav = 3;
            enc_am->addWebFTemp(2);
            break;
        case MENU_FAV_4T:
            ftemp = enc_am->getFavs()->getTemp(4);
            fav = 4;
            enc_am->addWebFTemp(3);
            break;
        default:
            ftemp = enc_am->getFavs()->getTemp(1);
            break;
        }
        if (eb.increment() > 0)
        {
            ftemp -= abs(eb.increment()) * dir;
        }
        else if (eb.increment() < 0)
        {
            ftemp += abs(eb.increment()) * dir;
        }
        enc_am->setPressTick(500);
        enc_am->getFavs()->softSetTemp(ftemp, fav);
        // enc_am->addWebFTemp(fav > 0 ? fav-1: 0);
        enc_am->webUpdateFavs(true);
        return;
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_LED_BRIGHTNESS)
    {
        bool change = false;
        uint8_t bright = enc_am->getLights()->getBright();
        if (eb.increment() > 0)
        {
            if (bright > 0)
            {
                bright -= 1 * dir;
                change = true;
            }
        }
        else if (eb.increment() < 0)
        {
            if (bright < 255)
            {
                bright += 1 * dir;
                change = true;
            }
        }
        if (change)
        {
            enc_am->getLights()->setBright(bright);
            enc_am->updateLStor(true);
            enc_am->webUpdateLEDBright();
            enc_am->setPressTick(400);
        }
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_BUMP_AMOUNT)
    {
        double btemp = enc_am->getBump()->getTemp();
        if (eb.increment() > 0)
        {
            btemp -= abs(eb.increment()) * dir;
        }
        else if (eb.increment() < 0)
        {
            btemp += abs(eb.increment()) * dir;
        }
        enc_am->setPressTick(400);
        enc_am->getBump()->setTemp(btemp);
        enc_am->webUpdateBump(true);
        return;
    }
    else if (enc_aerGUI->getMenuProps()->menuIndex == MENU_BUMP_TIME)
    {
        double btime = enc_am->getBump()->getTime();
        if (eb.increment() > 0)
        {
            btime -= 1 * dir;
        }
        else if (eb.increment() < 0)
        {
            btime += 1 * dir;
        }
        enc_am->setPressTick(400);
        enc_am->getBump()->setTime(btime);
        enc_am->webUpdateBump(true);
        return;
    }

    if (enc_aerGUI->isCursorModify())
    {
        uint16_t cursor = enc_aerGUI->getCursorModify();
        switch (cursor)
        {
        case MENU_SCREENSAVER_TIME:
        {
            uint time = enc_am->getSettings()->getTimeTillScrnSaverOn() * 0.001;
            if (eb.increment() > 0)
            {
                if (time >= 5 * 60)
                {
                    time -= 60 * dir;
                }
                else
                {
                    time -= 30 * dir;
                }
            }
            else if (eb.increment() < 0)
            {
                if (time >= 5 * 60)
                {
                    time += 60 * dir;
                }
                else
                {
                    time += 30 * dir;
                }
            }
            // min
            if (time < 30)
            {
                time = 30;
            }
            // max
            if (time > 60 * 60)
            {
                time = 60 * 60;
            }
            enc_am->getSettings()->setTimeTillScrnSaverOn(time);
            enc_am->setPressTick(420);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_SCREENSAVER_STARS:
        {
            uint stars = enc_am->getSettings()->getScrnSaverStarCount();
            if (eb.increment() > 0)
            {
                stars -= 16 * dir;
            }
            else if (eb.increment() < 0)
            {
                stars += 16 * dir;
            }
            // min
            if (stars < 16)
            {
                stars = 16;
            }
            // max
            if (stars > 1024)
            {
                stars = 1024;
            }
            enc_am->getSettings()->setScrnSaverStarCount(stars);
            enc_am->setPressTick(420);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_WIFI_HOSTNAME:
        // case MENU_WIFI_PASSWORD_EDIT:
        case MENU_WIFI_HOSTNAME_EDIT:
        {
            int chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 1 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 1 * dir;
            }
            int adv = 0;
            if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_PASSWORD)
            {
                adv = 21;
            }
            if (enc_aerGUI->getMenuProps()->menuItemId + chng >= -3 && enc_aerGUI->getMenuProps()->menuItemId + chng < (26 * 2) + 10 + 2 + adv)
            {
                enc_aerGUI->getMenuProps()->menuItemId += chng;
            }
            enc_aerGUI->updateMenu();
            enc_am->webUpdateWIFI(true);
            break;
        }
        case MENU_WIFI_STATIC_IP_EDIT:
        case MENU_WIFI_STATIC_GATEWAY_EDIT:
        case MENU_WIFI_STATIC_NETMASK_EDIT:
        case MENU_WIFI_STATIC_DNS1_EDIT:
        case MENU_WIFI_STATIC_DNS2_EDIT:
        {
            int chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 1 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 1 * dir;
            }
            if (enc_aerGUI->getMenuProps()->menuItemId + chng <= -5)
            {
                enc_aerGUI->getMenuProps()->menuItemId = 255;
            }
            else if (enc_aerGUI->getMenuProps()->menuItemId + chng >= 256)
            {
                enc_aerGUI->getMenuProps()->menuItemId = -4;
            }
            else if (enc_aerGUI->getMenuProps()->menuItemId + chng >= -4 && enc_aerGUI->getMenuProps()->menuItemId + chng < 256)
            {
                enc_aerGUI->getMenuProps()->menuItemId += chng;
            }
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_PID_PWM_FACTOR_VAR:
        {
            double chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 0.001 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 0.001 * dir;
            }
            enc_am->getAerPID(elementIndex)->PWM_ScaleFactor += chng;
            enc_aerGUI->updateMenu();
            enc_am->webUpdatePID(true);
            break;
        }
        case MENU_PID_PWM_BIAS_VAR:
        {
            double chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 0.5 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 0.5 * dir;
            }
            double newVal = enc_am->getAerPID(elementIndex)->getOutputBias() + chng;
            enc_am->getAerPID(elementIndex)->setOutputBias(newVal);
            enc_aerGUI->updateMenu();
            enc_am->webUpdatePID(true);
            break;
        }
        case MENU_PID_WINDUP_LIMIT_VAR:
        {
            double chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 0.2 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 0.2 * dir;
            }
            double newVal = enc_am->getAerPID(elementIndex)->getWindupLimit() + chng;
            enc_am->getAerPID(elementIndex)->setWindupLimit(newVal);
            enc_aerGUI->updateMenu();
            enc_am->webUpdatePID(true);
            break;
        }
        case MENU_PID_PWM_CYCLE_VAR:
        {
            double chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 1 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 1 * dir;
            }
            enc_am->getAerPID(elementIndex)->PWM_CycleTime += chng;
            enc_aerGUI->updateMenu();
            enc_am->webUpdatePID(true);
            break;
        }
        case MENU_SYSTEM_LENGTH_IDLE_VAR:
        {
            int chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 30 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 30 * dir;
            }
            enc_am->getAerPID(elementIndex)->AUTO_OFF_TIME += chng;
            if (enc_am->getAerPID(elementIndex)->AUTO_OFF_TIME < 30)
            {
                enc_am->getAerPID(elementIndex)->AUTO_OFF_TIME = 30;
            }
            if (enc_am->getAerPID(elementIndex)->AUTO_OFF_TIME > 18000)
            {
                enc_am->getAerPID(elementIndex)->AUTO_OFF_TIME = 900;
            }
            enc_am->updateFlashStor(true);
            enc_am->setPressTick(300);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_SYSTEM_TOGGLE_IDLE_VAR:
        {
            bool enb = enc_am->getAerPID(elementIndex)->AUTO_OFF_ENB;
            enc_am->getAerPID(elementIndex)->AUTO_OFF_ENB = !enb;
            enc_am->updateFlashStor(true);
            enc_am->setPressTick(300);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_DISPLAY_BACKLIGHT_VAR:
        {
            int chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 2 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 2 * dir;
            }
            if (enc_aerGUI->getST7789()->getBacklightLevel() + chng > 255)
            {
                enc_aerGUI->getST7789()->setBacklight(255);
            }
            else if (enc_aerGUI->getST7789()->getBacklightLevel() + chng < 0)
            {
                enc_aerGUI->getST7789()->setBacklight(0);
            }
            else
            {
                enc_aerGUI->getST7789()->setBacklight(enc_aerGUI->getST7789()->getBacklightLevel() + chng, false);
            }
            tftSettingsStorage.setBacklightLevel(enc_aerGUI->getST7789()->getBacklightLevel());
            enc_am->setPressTick(300);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_DISPLAY_BACKLIGHT_MIN_VAR:
        {
            int chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 2 * dir;
            }
            else if (eb.increment() < 0)
            {
                chng += 2 * dir;
            }
            if (enc_aerGUI->getST7789()->getBacklightLevelMin() + chng > 255)
            {
                enc_aerGUI->getST7789()->setBacklightMin(255);
            }
            else if (enc_aerGUI->getST7789()->getBacklightLevelMin() + chng < 0)
            {
                enc_aerGUI->getST7789()->setBacklightMin(0);
            }
            else
            {
                enc_aerGUI->getST7789()->setBacklightMin(enc_aerGUI->getST7789()->getBacklightLevelMin() + chng);
            }
            tftSettingsStorage.setBacklightLevelMin(enc_aerGUI->getST7789()->getBacklightLevelMin());
            enc_am->setPressTick(300);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_DISPLAY_TIMEOUT_DIM_VAR:
        {
            int timeout = enc_aerGUI->getST7789()->getBacklightDimTimeout();
            int chng = 0;
            if (timeout >= 60)
            {
                if (eb.increment() > 0)
                {
                    chng -= 30 * dir;
                }
                else if (eb.increment() < 0)
                {
                    chng += 30 * dir;
                }
            }
            else
            {
                if (eb.increment() > 0)
                {
                    chng -= 5 * dir;
                }
                else if (eb.increment() < 0)
                {
                    chng += 5 * dir;
                }
            }

            if (enc_aerGUI->getST7789()->getBacklightDimTimeout() + chng > 9000)
            {
                enc_aerGUI->getST7789()->setBacklightDimTimeout(900);
            }
            else if (enc_aerGUI->getST7789()->getBacklightDimTimeout() + chng < 5)
            {
                enc_aerGUI->getST7789()->setBacklightDimTimeout(5);
            }
            else
            {
                enc_aerGUI->getST7789()->setBacklightDimTimeout(enc_aerGUI->getST7789()->getBacklightDimTimeout() + chng);
            }

            tftSettingsStorage.setBacklightDimTimeout(enc_aerGUI->getST7789()->getBacklightDimTimeout());
            enc_am->setPressTick(400);
            enc_aerGUI->updateMenu();
            break;
        }
        case MENU_KNOB_ADJUST_AMOUNT:
        {
            double chng = 0;
            if (eb.increment() > 0)
            {
                chng -= 1 * dir;
                if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT - chng * 0.1 < 2 || enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT - chng < 2)
                {
                    chng *= 0.1;
                }
            }
            else if (eb.increment() < 0)
            {
                chng += 1 * dir;
                if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT + chng * 0.1 <= 2 || enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT + chng < 2)
                {
                    chng *= 0.1;
                }
            }
            if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT + chng < 0.1)
            {
                chng = 0.1;
            }
            else if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT + chng > 255)
            {
                chng = 255;
            }
            if (enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT + chng >= 0.1)
            {
                enc_am->getAerPID(elementIndex)->TEMP_ADJ_AMOUNT += chng;
                enc_am->setPressTick(300);
                enc_aerGUI->updateMenu();
            }
            break;
        }
        default:
            break;
        }
        return;
    }

    if (enc_aerGUI->getMenuProps()->menuIndex == MENU_WIFI_NETWORKS_NEARBY)
    {
        if (enc_am->getNetVars()->scanResults.size() == 0)
        {
            return;
        }
        if (enc_aerGUI->getMenuProps()->menuLevelId > 0)
        {
            // handle ssid select...
            if (dir > 0)
            {
                if (eb.increment() > 0)
                {
                    enc_aerGUI->decrDisplayMenuLevel();
                }
                else if (eb.increment() < 0 && enc_aerGUI->getMenuProps()->menuLevelId < enc_am->getNetVars()->scanResults.size())
                {
                    enc_aerGUI->incrDisplayMenuLevel();
                }
            }
            else
            {
                if (eb.increment() > 0 && enc_aerGUI->getMenuProps()->menuLevelId < enc_am->getNetVars()->scanResults.size())
                {
                    enc_aerGUI->incrDisplayMenuLevel();
                }
                else if (eb.increment() < 0)
                {
                    enc_aerGUI->decrDisplayMenuLevel();
                }
            }
            return;
        }
    }

    if (dir > 0)
    {
        if (eb.increment() > 0)
        {
            enc_aerGUI->decrDisplayMenuLevel();
        }
        else if (eb.increment() < 0)
        {
            enc_aerGUI->incrDisplayMenuLevel();
        }
    }
    else
    {
        if (eb.increment() > 0)
        {
            enc_aerGUI->incrDisplayMenuLevel();
        }
        else if (eb.increment() < 0)
        {
            enc_aerGUI->decrDisplayMenuLevel();
        }
    }
}

void enc_setup()
{
    Serial.println("> Init EncoderButton Setup");

    eb1.Encoderbegin();
    // Setup the interrupt for the encoder
    eb1.Encodersetup(readEncoderISR);
    // Link the events to functions
    eb1.setClickHandler(onEb1Clicked);
    eb1.setEncoderHandler(onEb1Encoder);
    eb1.setDebounceInterval(8); // 10 is default

    bt1.setClickHandler(onBt1Clicked);
    bt1.setDebounceInterval(8);
    bt2.setClickHandler(onBt2Clicked);
    bt2.setDebounceInterval(8);
}

void enc_loop()
{
    // You must call update() for every defined EncoderButton.
    // This will update the state of the encoder & button and
    // fire the appropriat events.
    eb1.update();
    bt1.update();
    bt2.update();
}

void enc_task(void *pvParameters)
{
    AerManager *_am = (AerManager *)pvParameters; // task parameters

    enc_am = _am;

    // setup encoder
    enc_setup();

    while (1)
    {
        if (xSemaphoreTake(sys1_mutex, 10) == pdTRUE)
        {
            enc_loop();
            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif