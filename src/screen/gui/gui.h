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

#ifndef _AER_GUI_H
#define _AER_GUI_H

#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>
#include <string.h>

#include "AerMenu.h"

#include "screen/st7789.h"

#include "common/enum/menu_definition.h"
#include "common/datatypes/PropsMenu.h"
#include "common/datatypes/PropsTFT.h"

#include "screen/img/icon/home.h"
#include "screen/img/icon/home_sel.h"
#include "screen/img/icon/pid.h"
#include "screen/img/icon/favs.h"
#include "screen/img/icon/favs_sel.h"
#include "screen/img/icon/bump.h"
#include "screen/img/icon/bump_num.h"
#include "screen/img/icon/bump_time.h"
#include "screen/img/icon/sys.h"
#include "screen/img/icon/leds.h"
#include "screen/img/icon/wifi.h"
#include "screen/img/icon/wifi_scn.h"
#include "screen/img/icon/ble.h"
#include "screen/img/icon/power.h"
#include "screen/img/icon/power_off.h"
#include "screen/img/icon/usb.h"
#include "screen/img/icon/clock.h"
#include "screen/img/icon/clock2.h"
#include "screen/img/icon/alarm.h"
#include "screen/img/icon/timer.h"
#include "screen/img/icon/update.h"
#include "screen/img/icon/thermal.h"
#include "screen/img/icon/thermal2.h"
#include "screen/img/icon/thermal_cpu.h"
#include "screen/img/icon/thermal_ssr.h"
#include "screen/img/icon/thermal_sys.h"
#include "screen/img/icon/reset.h"
#include "screen/img/icon/display.h"
#include "screen/img/icon/graphs.h"
#include "screen/img/icon/temp_c_l2.h"
#include "screen/img/icon/temp_f_l2.h"
#include "screen/img/icon/temp_k_l2.h"
#include "screen/img/icon/wifi_on.h"
#include "screen/img/icon/wifi_off.h"
#include "screen/img/icon/wifi_act.h"
#include "screen/img/icon/wifi_con.h"
#include "screen/img/icon/wifi_rad.h"
#include "screen/img/icon/ble_on.h"
#include "screen/img/icon/ble_off.h"
#include "screen/img/icon/usb_on.h"
#include "screen/img/icon/usb_off.h"
#include "screen/img/icon/usb_act.h"
#include "screen/img/icon/web_on.h"
#include "screen/img/icon/web_off.h"
#include "screen/img/icon/net_host.h"
#include "screen/img/icon/net_ip.h"
#include "screen/img/icon/net_mac.h"
#include "screen/img/icon/net_pass.h"
#include "screen/img/icon/knob.h"
#include "screen/img/icon/idle_len.h"
#include "screen/img/icon/idle_tog.h"
#include "screen/img/icon/tray.h"
#include "screen/img/icon/stars.h"
#include "screen/img/icon/stars_enb.h"
#include "screen/img/icon/stars_time.h"
#include "screen/img/icon/stars_bl.h"
#include "screen/img/icon/theme.h"
#include "screen/img/icon/bulb_glow.h"
#include "screen/img/icon/bulb_blue.h"
#include "screen/img/icon/bulb_red.h"
#include "screen/img/icon/bulb_green.h"
#include "screen/img/icon/pwm_auto.h"
#include "screen/img/icon/pwm_factor.h"
#include "screen/img/icon/pwm_cycle.h"
#include "screen/img/icon/pwm_freq.h"
#include "screen/img/icon/fan_1.h"

/// @brief Storage accesor class for cross thread graphics
class AerGUI
{
public:
    AerGUI() { this->intialized = false; }
    AerGUI(bool innit)
    {
        if (innit)
        {
            init();
        }
    }

    uint32_t activity_counter = 0;

    /// @brief Gets the ST7789 Display driver
    /// @return AerST7789
    AerST7789 *getST7789();

    TFT_eSPI *getTFT();
    TFT_eSprite *getSpriteBuffer(uint bufferNumber);

    /// @brief Gets the Menu struct
    /// @return PropsMenu
    PropsMenu *getMenuProps();

    void updateDisplayMenu(uint16_t index, uint16_t level);
    void updateDisplayMenuLevel(uint16_t level);
    void updateMenu(bool change = false);
    void setMenu(uint16_t index, AerMenu menu);

    bool incrDisplayMenuLevel(); // increments menu level
    bool decrDisplayMenuLevel(); // decrements menu level

    bool selectMenuLevel(); // selects menu at current index

    AerMenu *getCurrentMenu();               // gets pointer reference to current menu
    AerMenu getSelectedMenu();               // gets mapping entry to current menu
    AerMenu getSelectedMenu(uint16_t index); // gets mapping entry of given index

    String getMenuName(int index); // user friendly menu names

    void gotoMenu(uint16_t menu_id, uint16_t menu_val = 0, uint16_t id = 0);

    // Cursor select for modifying values..
    void setCursorModify(uint16_t cursor);
    void clearCursorModify();
    uint16_t getCursorModify();
    bool isCursorModify();

    void setElementIndex(uint8_t ei);
    uint8_t getElementIndex();

    // print icon for given menu idnex to screen
    void printIcon(uint x, uint y, uint16_t menuIndex, bool selected);
    void printIcon(TFT_eSprite *spr, uint x, uint y, uint16_t menuIndex, bool selected);

    char getABC(uint i, bool lower = true);
    char getNumber(uint i);
    char getSymbol(uint i, bool adv = false);

    char getCharFromIndex(int i, uint type = 0);

protected:
    void init();

private:
    AerST7789 *st7789;
    PropsMenu *menuStor;

    std::vector<AerMenu> menus;
    std::map<int, String> menuNames;
    AerMenu *lastMenu;

    uint16_t cursorModify = 0;

    uint8_t elementIndex = 0;

    bool intialized;

    bool buildMenuDefinitions();

    std::string abcLower{"abcdefghijklmnopqrstuvwxyz"};
    std::string abcUpper{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
    std::string nbrChars{"0123456789"};
    std::string spcChars{".-!?@#$%^*()+=_[];,|<>~"};
};

#endif
