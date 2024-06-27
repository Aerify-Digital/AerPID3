/* =============================================================================
 * Copyright (c) 2015-2024 AerifyDigital LLC.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * =============================================================================
 */

#ifndef ST7789_TFT_V_H
#define ST7789_TFT_V_H

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <TFT_eSPI.h>

#include "screen/img/aeri.h"
// #include "screen/img/mainbg.h"
//  #include "screen/img/bg01.h"
#include "screen/img/main/bg02_btmr.h"
#include "screen/img/main/bg02_mid.h"
#include "screen/img/main/bg03_top.h"
#include "screen/img/main/bg03_right.h"
#include "screen/img/main/bg03_btm_b.h"
#include "screen/img/main/bg03_bar_red.h"
#include "screen/img/main/bg03_bar_green.h"
#include "screen/img/main/bg03_bar_blue.h"
#include "screen/img/main/bg03_bar_purple.h"
#include "screen/img/main/bg03_right_btn_on.h"
#include "screen/img/main/bg03_right_btn_off.h"
#include "screen/img/main/bg03_right_btn2_on.h"
#include "screen/img/main/bg03_right_btn2_off.h"
#include "common/datatypes/PropsMenu.h"
#include "common/datatypes/DateTimeRTC.h"
#include "common/enum/ThermalUnitsType.h"
#include "common/util/ElementProtectOpCode.h"
#include "util/thermals.hpp"
#include "util/color565.hpp"
#include "gui/gui.h"
#include "Graphing.h"

#include "core/controllers/AerManager.h"
#include "storage/favsStor.h"
#include "storage/commStor.h"
#include "storage/bumpStor.h"
#include "storage/lights.h"
#include "storage/tempStor.h"
#include "storage/fanControlStor.h"
#include "IPAddress.h"

#include "esp_heap_trace.h"
#include "esp_heap_caps.h"

#include "web/webServer.h"

namespace AerTftUI
{
    void disposeGraphing();
    void drawRoundRectWithBorder1px(TFT_eSPI *lcd, uint x, uint y, uint w, uint h, uint r, uint16_t bgColor, uint16_t bordColor);
    void drawRoundRectWithBorder2px(TFT_eSPI *lcd, uint x, uint y, uint w, uint h, uint r, uint16_t bgColor, uint16_t bordColor);
    void printDigit(TFT_eSPI *lcd, uint number);
    void printDigit(TFT_eSprite *spr, uint number);
    void printSelections(AerGUI *gui, AerMenu menu, uint16_t mindex, uint16_t bgColor);
    void drawSelections(AerGUI *gui, AerMenu menu, uint16_t mindex, uint16_t bgColor);
    std::vector<uint16_t> getSelectionItems(AerMenu menu, uint16_t mindex);
    void showHomeScreen(AerManager *am, bool update, bool modalOpen, bool modalUpdate);
    void drawBarColorScroll(TFT_eSprite *spr, uint t, uint speed = 1);
    void showSysIconsSection(AerManager *am, bool update);
    void showSysInfoSection(AerManager *am, bool update);
    void showMemorySection(AerManager *am);
    void showCPUSection(AerManager *am);
    void showCPUSections(AerManager *am);
    void showCpu0Bar(AerManager *am, uint index, uint width, uint height);
    void showCpu1Bar(AerManager *am, uint index, uint width, uint height);
    void showTempSection(AerManager *am, bool updatescreen, ThermalUnitsType unitType);
    void showMainMenu(AerGUI *gui, bool indexupdate, bool menuChange);
    void showPIDMenu(AerGUI *gui, bool indexupdate, bool change);
    void showPIDMenuP(AerGUI *gui, float kP, bool indexupdate);
    void showPIDMenuI(AerGUI *gui, float kI, bool indexupdate);
    void showPIDMenuD(AerGUI *gui, float kD, bool indexupdate);
    void showPIDTuneMenu(AerManager *am, bool indexupdate, bool change);
    void showPIDAutoMenu(AerManager *am, bool indexupdate, bool change);
    void showPIDOutputBiasMenu(AerManager *am, bool indexupdate, bool change);
    void showPIDWindupLimitMenu(AerManager *am, bool indexupdate, bool change);
    void showPIDpwmFactorMenu(AerManager *am, bool indexupdate, bool change);
    void showPIDpwmCycleMenu(AerManager *am, bool indexupdate, bool change);
    void showPIDpwmFreqMenu(AerManager *am, bool indexupdate, bool change);
    void showLEDMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showLEDColorMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showLEDBrightMenu(AerManager *am, AerGUI *gui, bool update);
    void showFavsMenu(AerGUI *gui, bool indexupdate, bool change);
    void showExpFavMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showExpFavNameMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showExpFavTempMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showBumpMenu(AerGUI *gui, bool update, bool change);
    void showBumpTempMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showBumpTimeMenu(AerManager *am, AerGUI *gui, bool indexupdate);
    void showGraphMenu(AerGUI *gui, bool indexupdate, bool change);
    void showSysMenu(AerGUI *gui, bool indexupdate, bool change);
    void showWiFiMenu(AerManager *am, bool indexupdate, bool change);
    void showWiFiPassMenu(AerManager *am, bool indexupdate);
    void showWifiJoinMenu(AerManager *am, bool indexupdate, bool change);
    void showWifiScanMenu(AerManager *am, bool indexupdate, bool change);
    void showWifiScanMenuResults(AerManager *am, bool indexupdate, bool change);
    void showWifiJoinError(AerManager *am, bool update, bool change);     // error menu
    void showWifiDeleteMessage(AerManager *am, bool update, bool change); // message menu
    void showStaticAddrMenu(AerManager *am, bool update, bool change);
    void showStaticAddrEnabledMenu(AerManager *am, bool update, bool change);
    void showStaticAddrIPMenu(AerManager *am, bool update, bool change);
    void showStaticAddrNetMaskMenu(AerManager *am, bool update, bool change);
    void showStaticAddrGatewayMenu(AerManager *am, bool update, bool change);
    void showStaticAddrDNS1Menu(AerManager *am, bool update, bool change);
    void showStaticAddrDNS2Menu(AerManager *am, bool update, bool change);
    void showUSBMenu(AerManager *am, bool update, bool change);
    void showBleMenu(AerManager *am, bool update, bool change);
    void showKeyboardMenu(AerManager *am, AerGUI *gui, bool indexupdate, bool change);
    void showPowerMenu(AerGUI *gui, bool indexupdate, bool change);
    void showFactoryReset(AerManager *am, bool update, bool change);
    void showDisplayMenu(AerManager *am, AerGUI *gui, bool indexupdate, bool change);
    void showNearbyNetworks(AerGUI *gui, bool indexupdate, String json);
    void showSystemTemperature(AerManager *am, AerGUI *gui, bool update, bool change);
#if AERPID_COUNT == 2
    void showFanControl(AerManager *am, bool update, bool change);
#endif
    void showMainClockMenu(AerManager *am, AerGUI *gui, bool update, bool change);
    void showDateEditMenu(AerGUI *gui, bool update, bool change);
    void showDateEditingMenu(AerGUI *gui, bool update, bool change);
    void showTimeEditMenu(AerGUI *gui, bool update, bool change);
    void showTimeEditingMenu(AerGUI *gui, bool update, bool change);
    void showDefaultMenu(AerGUI *gui, bool update);
    void showTimeOutMenu(AerManager *am, AerGUI *gui, bool update);
    int convertMenuLvlToFav(int menuindex);
    String convertBumpSeconds(double seconds);
    bool isPresetActive(LightsStor *lightsObj, uint16_t mindex);
    void showKnobMenu(AerManager *am, AerGUI *gui, bool update);
    void showScreenSaverMenu(AerManager *am, bool update);
    void showIdleToggle(AerManager *am, bool update, bool change);
    void showIdleLength(AerManager *am, bool update, bool change);
    void showBacklight(AerManager *am, bool update, bool change);
    void showBacklightDim(AerManager *am, bool update, bool change);
    void showBacklightDimTimeout(AerManager *am, bool update, bool change);
    void printSelectedChar(AerGUI *gui, TFT_eSprite *spr, uint type = 0);
    void printSelectedNumber(AerGUI *gui, TFT_eSprite *spr);
    void showGraphTemperature(AerManager *am, bool update, bool change, uint8_t elementIndex);
    void showGraphTemperatureZoom(AerManager *am, bool update, bool change, uint8_t elementIndex);
    void showGraphTemperatureLong(AerManager *am, bool update, bool change, uint8_t elementIndex);
    void showGraphTemperature(AerManager *am, bool update, bool change);
    void showGraphTemperatureZoom(AerManager *am, bool update, bool change);
    void showGraphTemperatureLong(AerManager *am, bool update, bool change);
};

#endif