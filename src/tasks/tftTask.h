#ifndef _TFT_TASK_H
#define _TFT_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "common/datatypes/PropsTFT.h"
#include "common/datatypes/mutex.h"

#include "core/controllers/AerManager.h"

#include "common/enum/ElementProtectOpCodes.h"

#include "screen/gui/gui.h"
#include "screen/tft.h"

#define TASK_WAIT 60 // 16 fps at 60ms delay

void tft_task(void *pvParameters)
{
    AerManager *_am = (AerManager *)pvParameters; // task parameters

    AerGUI *_aerGUI = _am->getGUI();
    TFT_eSPI *tft = _aerGUI->getTFT();
    PropsMenu *_menu = _aerGUI->getMenuProps();

    _aerGUI->getST7789()->initBacklight();

    bool modalUpdate = false;
    ELM_PROT_OP_CODE op_state_last = ELM_PROT_OP_CODE::E_PROT_INIT_READY;

    tft->setRotation(1);
    tft->fillScreen(TFT_BLACK);

    // tft->pushImage(0, 0, 320, 67, image_data_aeri_top);
    // tft->pushImage(0, 0, 320, 240, image_data_bg01);

    // TFT_eSprite *spr1 = _aerGUI->getSpriteBuffer(0);
    // TFT_eSprite *spr2 = _aerGUI->getSpriteBuffer(1);
    // TFT_eSprite *spr3 = _aerGUI->getSpriteBuffer(2);

    if (xSemaphoreTake(sys1_mutex, 50) == pdTRUE)
    {
        if (xSemaphoreTake(spi1_mutex, 50) == pdTRUE)
        {
            AerTftUI::showHomeScreen(_am, true, false, false);
            xSemaphoreGive(spi1_mutex);
        }
        xSemaphoreGive(sys1_mutex);
    }

    Serial.print("> Starting TFT task on core ");
    Serial.println(xPortGetCoreID());
    while (1)
    {
        if (millis() < 2500)
        {
            vTaskDelay(200 / portTICK_PERIOD_MS);
            continue;
        }
        SettingsStorage *settings = _am->getSettings();
        if (_aerGUI->activity_counter < settings->getTimeTillScrnSaverOn() / TASK_WAIT && settings->getScreenSaverEnb())
        {
            _aerGUI->activity_counter++;
        }
        if (_aerGUI->activity_counter >= settings->getTimeTillScrnSaverOn() / TASK_WAIT && settings->getScreenSaverEnb())
        {
            if (!_am->isScreenSaverActive() && xSemaphoreTake(spi1_mutex, 200) == pdTRUE)
            {
                tft->fillScreen(TFT_BLACK);
                xSemaphoreGive(spi1_mutex);
            }
            if (!_am->isScreenSaverActive())
            {
                _am->setScreenSaverActive(true);
            }
        }
        else if (_am->isScreenSaverActive())
        {
            _am->setScreenSaverActive(false);
            _menu->menuUpdate = true;
        }

        if (_am->isScreenSaverActive())
        {
            vTaskDelay(750);
            continue;
        }

        bool _menuUpdate = false;
        ELM_PROT_OP_CODE op_state = _am->getOpEP();
        bool _modalOpen = op_state != ELM_PROT_OP_CODE::E_PROT_ACK;
        if (op_state != op_state_last || millis() < 2000)
        {
            if (op_state == ELM_PROT_OP_CODE::E_PROT_ACK || op_state_last == ELM_PROT_OP_CODE::E_PROT_ACK)
            {
                _menuUpdate = true;
            }
            op_state_last = op_state;
            modalUpdate = true;
        }

        uint8_t elementIndex = _aerGUI->getElementIndex();

        if (xSemaphoreTake(sys1_mutex, 50) == pdTRUE)
        {
            if (xSemaphoreTake(spi1_mutex, 25) == pdTRUE)
            {
                switch (_menu->menuIndex)
                {
                case MENU_ROOT_MAIN: /* Main Menu */
                {
                    AerTftUI::showMainMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    AerTftUI::disposeGraphing();
                    break;
                }
                case MENU_ROOT:
                case MENU_ROOT_HOME: /* Home Screen */
                {
                    AerTftUI::showHomeScreen(_am, _menu->menuUpdate || _menuUpdate, _modalOpen, modalUpdate);
                    _menu->menuUpdate = false;
                    break;
                }
                case MENU_MAIN_PID: /* PID Settings */
                {
                    AerTftUI::showPIDMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_PID_P: /* P Settings */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(19, 19, 282, 202, 7, TFT_DARKCYAN);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);
                        _menu->menuUpdate = false;
                        AerTftUI::showPIDMenuP(_aerGUI, _am->getAerPID(elementIndex)->kP, true);
                    }
                    else
                    {
                        AerTftUI::showPIDMenuP(_aerGUI, _am->getAerPID(elementIndex)->kP, false);
                    }
                    break;
                case MENU_PID_I: /* I Settings */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(19, 19, 282, 202, 7, TFT_DARKCYAN);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);
                        _menu->menuUpdate = false;
                        AerTftUI::showPIDMenuI(_aerGUI, _am->getAerPID(elementIndex)->kI, true);
                    }
                    else
                    {
                        AerTftUI::showPIDMenuI(_aerGUI, _am->getAerPID(elementIndex)->kI, false);
                    }
                    break;
                case MENU_PID_D: /* D Settings */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(19, 19, 282, 202, 7, TFT_DARKCYAN);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);
                        _menu->menuUpdate = false;
                        AerTftUI::showPIDMenuD(_aerGUI, _am->getAerPID(elementIndex)->kD, true);
                    }
                    else
                    {
                        AerTftUI::showPIDMenuD(_aerGUI, _am->getAerPID(elementIndex)->kD, false);
                    }
                    break;
                case MENU_PID_TUNING: /* PID Tuning */
                {
                    AerTftUI::showPIDTuneMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_PID_AUTO: /* PID Auto */
                {
                    AerTftUI::showPIDAutoMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_PID_PWM_FACTOR: /* PID PWM Factor */
                {
                    AerTftUI::showPIDpwmFactorMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_PID_PWM_CYCLE: /* PID PWM Cycle */
                {
                    AerTftUI::showPIDpwmCycleMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_PID_PWM_FREQ: /* PID PWM Freq */
                {
                    AerTftUI::showPIDpwmFreqMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_MAIN_FAVS: /* Favorites Menu */
                {
                    AerTftUI::showFavsMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_MAIN_BUMP: /* Bump Settings */
                {
                    AerTftUI::showBumpMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_BUMP_AMOUNT: /* Bump Temp */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_SKYBLUE);
                        _menu->menuUpdate = false;
                        AerTftUI::showBumpTempMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showBumpTempMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_BUMP_TIME: /* Bump Time */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_SKYBLUE);
                        _menu->menuUpdate = false;
                        AerTftUI::showBumpTimeMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showBumpTimeMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_MAIN_GRAPH: /* Charting & Graphs */
                {
                    AerTftUI::showGraphMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE: /* Charting & Graphs Element 1 & 2 */
                {
                    AerTftUI::showGraphTemperature(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_0: /* Charting & Graphs Element 1 */
                {
                    AerTftUI::showGraphTemperature(_am, _menu->menuUpdate, _menu->menuChange, 0);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_1: /* Charting & Graphs Element 1 */
                {
                    AerTftUI::showGraphTemperature(_am, _menu->menuUpdate, _menu->menuChange, 1);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_ZOOM: /* Charting & Graphs Element 1 & 2 */
                {
                    AerTftUI::showGraphTemperatureZoom(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_ZOOM_0: /* Charting & Graphs Element 1 */
                {
                    AerTftUI::showGraphTemperatureZoom(_am, _menu->menuUpdate, _menu->menuChange, 0);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_ZOOM_1: /* Charting & Graphs Element 2 */
                {
                    AerTftUI::showGraphTemperatureZoom(_am, _menu->menuUpdate, _menu->menuChange, 1);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_LONG: /* Charting & Graphs Element 1 & 2 */
                {
                    AerTftUI::showGraphTemperatureLong(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_LONG_0: /* Charting & Graphs Element 1 */
                {
                    AerTftUI::showGraphTemperatureLong(_am, _menu->menuUpdate, _menu->menuChange, 0);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_GRAPH_TEMPERATURE_LONG_1: /* Charting & Graphs Element 2 */
                {
                    AerTftUI::showGraphTemperatureLong(_am, _menu->menuUpdate, _menu->menuChange, 1);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_MAIN_SYSTEM: /* System Settings */
                {
                    AerTftUI::showSysMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_LED_SETUP:
                case MENU_LED_STATUS:
                // case MENU_LED_AMBIENT:
                case MENU_LED_PRESET:
                case MENU_MAIN_LED: /* LED Settings */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, 0x5BB0);
                        tft->drawRoundRect(18, 18, 284, 204, 7, 0x3209);
                        _menu->menuUpdate = false;
                        AerTftUI::showLEDMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showLEDMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_LED_BRIGHTNESS:
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, 0x5BB0);
                        tft->drawRoundRect(18, 18, 284, 204, 7, 0x3209);
                        _menu->menuUpdate = false;
                        AerTftUI::showLEDBrightMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showLEDBrightMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_MAIN_WIFI: /* WiFi Settings */
                {
                    AerTftUI::showWiFiMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_NETWORK_SCAN: /* WiFi Network Scan */
                {
                    AerTftUI::showWifiScanMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_PASSWORD_NETWORK_JOIN: /* WiFi Network Join */
                {
                    AerTftUI::showWifiJoinMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_PASSWORD_NETWORK_JOIN_ERROR: /* WiFi Network Join Error */
                {
                    AerTftUI::showWifiJoinError(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_PASSWORD_DELETE_STORED_ACK: /* WiFi Network Leave Message */
                {
                    AerTftUI::showWifiDeleteMessage(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_NETWORKS_NEARBY: /* WiFi Nearby Networks/Scan Results */
                {
                    AerTftUI::showWifiScanMenuResults(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_PASSWORD: /* WiFi Password */
                {
                    AerTftUI::showKeyboardMenu(_am, _aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_ADDR:
                {
                    AerTftUI::showStaticAddrMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_ENABLED:
                {
                    AerTftUI::showStaticAddrEnabledMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_IP:
                {
                    AerTftUI::showStaticAddrIPMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_GATEWAY:
                {
                    AerTftUI::showStaticAddrGatewayMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_NETMASK:
                {
                    AerTftUI::showStaticAddrNetMaskMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_DNS1:
                {
                    AerTftUI::showStaticAddrDNS1Menu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_WIFI_STATIC_DNS2:
                {
                    AerTftUI::showStaticAddrDNS2Menu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_MAIN_BLE: /* BLE Settings */
                    AerTftUI::showBleMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                case MENU_MAIN_USB: /* USB Settings */
                {
                    AerTftUI::showUSBMenu(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_MAIN_POWER: /* Power Settings */
                {
                    AerTftUI::showPowerMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_SYSTEM_DISPLAY_SETTINGS: /* Display Settings */
                {
                    AerTftUI::showDisplayMenu(_am, _aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_DISPLAY_SCREENSAVER: /* Screen Saver Settings */
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
                        AerTftUI::showScreenSaverMenu(_am, true);
                        _menu->menuUpdate = false;
                    }
                    else
                    {
                        AerTftUI::showScreenSaverMenu(_am, false);
                    }
                    break;
                case MENU_FAV_1: /* Fav Sub Menus */
                case MENU_FAV_2:
                case MENU_FAV_3:
                case MENU_FAV_4:
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(18, 18, 284, 204, 7, TFT_GREEN);
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREEN);
                        _menu->menuUpdate = false;
                        AerTftUI::showExpFavMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showExpFavMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_FAV_1N: /* Fav Name Menus */
                case MENU_FAV_2N:
                case MENU_FAV_3N:
                case MENU_FAV_4N:
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(18, 18, 284, 204, 7, TFT_GREEN);
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREEN);
                        _menu->menuUpdate = false;
                        AerTftUI::showExpFavNameMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showExpFavNameMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_FAV_1T: /* Fav Temp Menus */
                case MENU_FAV_2T:
                case MENU_FAV_3T:
                case MENU_FAV_4T:
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(18, 18, 284, 204, 7, TFT_GREEN);
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREEN);
                        _menu->menuUpdate = false;
                        AerTftUI::showExpFavTempMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showExpFavTempMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_WIFI_PASSWORD_SHOW: /* SSID Password */
                case MENU_WIFI_HOSTNAME:      /* Hostname */
                {
                    AerTftUI::showKeyboardMenu(_am, _aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                    // case MENU_WIFI_NETWORKS_NEARBY: /* Nearyby Networks */
                    /*if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
                        _menu->menuUpdate = false;
                        showNearbyNetworks(_aerGUI, true, "NULL");
                    }
                    else
                    {
                        showNearbyNetworks(_aerGUI, false, "NULL");
                    }
                    break;*/
                case MENU_SYSTEM_LOCAL_TEMPERATURE: /* System Temperature */
                {
                    AerTftUI::showSystemTemperature(_am, _aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
#if AERPID_COUNT == 2
                case MENU_LOCAL_FAN_CONTROL: /* System Fan Control */
                {
                    AerTftUI::showFanControl(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
#endif
                case MENU_MAIN_CLOCK: /* Clock Menu */
                {
                    AerTftUI::showMainClockMenu(_am, _aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_RTC_DATE_SET:
                {
                    AerTftUI::showDateEditMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_RTC_SET_DATE_YEAR:
                case MENU_RTC_SET_DATE_MONTH:
                case MENU_RTC_SET_DATE_DAY:
                {
                    AerTftUI::showDateEditingMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuUpdate = false;
                    break;
                }
                case MENU_RTC_TIME_SET:
                {
                    AerTftUI::showTimeEditMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_RTC_SET_TIME_HOUR:
                case MENU_RTC_SET_TIME_MINUTE:
                case MENU_RTC_SET_TIME_SECOND:
                {
                    AerTftUI::showTimeEditingMenu(_aerGUI, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_SYSTEM_KNOB_ADJUST:
                case MENU_KNOB_ADJUST_AMOUNT:
                case MENU_KNOB_ADJUST_INVERT:
                case MENU_KNOB_DIAGRAM_SHOWN:
                    if (_menu->menuUpdate)
                    {
                        tft->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
                        tft->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
                        _menu->menuUpdate = false;
                        AerTftUI::showKnobMenu(_am, _aerGUI, true);
                    }
                    else
                    {
                        AerTftUI::showKnobMenu(_am, _aerGUI, false);
                    }
                    break;
                case MENU_SYSTEM_FACTORY_RESET:
                {
                    AerTftUI::showFactoryReset(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_SYSTEM_TOGGLE_IDLE:
                {
                    AerTftUI::showIdleToggle(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_SYSTEM_LENGTH_IDLE:
                {
                    AerTftUI::showIdleLength(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_DISPLAY_BACKLIGHT:
                {
                    AerTftUI::showBacklight(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_DISPLAY_AUTO_DIM:
                {
                    AerTftUI::showBacklightDim(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }
                case MENU_DISPLAY_TIMEOUT_DIM:
                {
                    AerTftUI::showBacklightDimTimeout(_am, _menu->menuUpdate, _menu->menuChange);
                    _menu->menuUpdate = false;
                    _menu->menuChange = false;
                    break;
                }

                default: /* Default Menu Setup */
                    if (_menu->menuUpdate)
                    {
                        // AerTftUI::showHomeScreen(_am, _menu->menuUpdate || _menuUpdate, false, false);
                    }
                    AerTftUI::showDefaultMenu(_aerGUI, _menu->menuUpdate);
                    _menu->menuUpdate = false;
                    break;
                }

                // release locks
                xSemaphoreGive(spi1_mutex);
            }
            xSemaphoreGive(sys1_mutex);
        }

        vTaskDelay(TASK_WAIT / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

#endif