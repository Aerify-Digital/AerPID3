#include "tft.h"

namespace AerTftUI
{

    double lastTemp = 0;
    double lastTemp_set = 0;
    uint16_t lastindex = 0;
    double lastFavTemp = 0;
    double lastBumpTemp = 0;
    double lastLedBright = 0;
    double lastBumpTime = 0;
    double lastTimeoutTime = 0;
    uint64_t lastMonitorUpdateTime = 0;

    uint tick_icon = 0;

    int bar_index[2] = {0, 0};

    // charting object for graphs
    AerChart *graph;

    void disposeGraphing()
    {
        graph = nullptr;
    }

    // ============================================================================
    // ============================================================================

    void drawRoundRectWithBorder2px(TFT_eSPI *lcd, uint x, uint y, uint w, uint h, uint r, uint16_t bgColor, uint16_t bordColor)
    {
        lcd->fillRoundRect(x, y, w, h, r + 1, bgColor);
        lcd->drawRoundRect(x - 2, y - 2, w + 4, h + 4, r, bordColor);
        lcd->drawRoundRect(x - 2, y - 2, w + 4, h + 4, r - 1, bordColor);
        lcd->drawRoundRect(x - 1, y - 1, w + 2, h + 2, r, bordColor);
        lcd->drawRoundRect(x - 1, y - 1, w + 2, h + 2, r + 1, bordColor);
    }

    void drawRoundRectWithBorder1px(TFT_eSPI *lcd, uint x, uint y, uint w, uint h, uint r, uint16_t bgColor, uint16_t bordColor)
    {
        lcd->fillRoundRect(x, y, w, h, r + 1, bgColor);
        lcd->drawRoundRect(x - 1, y - 1, w + 2, h + 2, r, bordColor);
    }

    // ============================================================================
    // ============================================================================

    void printDigit(TFT_eSprite *spr, uint number)
    {
        if (number >= 0 && number < 10)
        {
            spr->print("0");
            spr->print(number);
        }
        else
        {
            spr->print(number);
        }
    }

    void printDigit(TFT_eSPI *lcd, uint number)
    {
        if (number >= 0 && number < 10)
        {
            lcd->print("0");
            lcd->print(number);
        }
        else
        {
            lcd->print(number);
        }
    }

    std::vector<uint16_t> getSelectionItems(AerMenu menu, uint16_t mindex)
    {
        std::vector<uint16_t> items;
        bool bSelect = false;
        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }
        return items;
    }

    void printSelections(AerGUI *gui, AerMenu menu, uint16_t mindex, uint16_t bgColor)
    {
        TFT_eSPI *lcd = gui->getTFT();

        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        uint8_t offset = 30;
        lcd->setTextSize(3);
        for (int i = 0; i < items.size(); i++)
        {
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == menu.getChildren()[0])
            {
                // lcd->drawRect(32 - 2, 65 + (offset * i) - 2, 32 - 4, 32 - 4, TFT_GREEN);
                // lcd->fillRect(32 - 1, 65 + (offset * i) - 1, 32 - 6, 32 - 6, TFT_YELLOW);
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], true);
            }
            else
            {
                // lcd->drawRect(32 - 2, 65 + (offset * i) - 2, 32 - 4, 32 - 4, TFT_LIGHTGREY);
                // lcd->fillRect(32 - 1, 65 + (offset * i) - 1, 32 - 6, 32 - 6, TFT_GREEN);
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], false);
            }

            // lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == mindex)
            {
                lcd->setTextColor(TFT_CYAN, bgColor);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
            else
            {
                lcd->setTextColor(TFT_WHITE, bgColor);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
        }
    }

    void drawSelections(AerGUI *gui, AerMenu menu, uint16_t mindex, uint16_t bgColor)
    {
        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        uint x = 30;
        uint y = 63;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        uint8_t offset = 30;

        for (int i = 0; i < std::min(5, (int)menu.getChildren().size()); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
    }

    // ============================================================================
    // ============================================================================
    // ============================================================================

    // Main home screen
    void showHomeScreen(AerManager *am, bool update, bool modalOpen, bool modalUpdate)
    {
        AerGUI *aerGUI = am->getGUI();
        TFT_eSPI *tft = aerGUI->getTFT();
        SettingsStorage *settings = am->getSettings();
        ELM_PROT_OP_CODE op_state = am->getOpEP();

        TFT_eSprite *spr1 = aerGUI->getSpriteBuffer(0);
        TFT_eSprite *spr2 = aerGUI->getSpriteBuffer(1);
        TFT_eSprite *spr3 = aerGUI->getSpriteBuffer(2);
        TFT_eSprite *spr4 = aerGUI->getSpriteBuffer(3);
        TFT_eSprite *spr5 = aerGUI->getSpriteBuffer(4);

        if ((update) && !modalOpen)
        {
            showTempSection(am, true, am->getReadingType());
        }
        else if (!modalOpen)
        {
            showTempSection(am, millis() < 7200, am->getReadingType());
        }

        if (!modalOpen)
        {
            spr3->createSprite(48, 240);
            spr3->pushImage(0, 0, 48, 240, image_data_bg03_right);

            if (am->isEnabledDTR())
            {
                spr3->fillRoundRect(277 - 272, 1, 43, 10, 0, tft->color565(30, 255, 200));
                spr3->drawRoundRect(276 - 272, 0, 45, 12, 0, TFT_BLUE);
                spr3->setCursor(283 - 272, 3);
                spr3->setTextSize(1);
                spr3->setTextColor(TFT_MAROON, tft->color565(30, 255, 200));
                spr3->print("DEBUG");
            }
            else
            {
                // spr3->fillRoundRect(277 - 272, 0, 44, 12, 0, 0x31C8);
            }

            uint8_t elementIndex = aerGUI->getElementIndex();

            if (am->getAerPID(elementIndex)->PID_ON)
            {
                spr3->pushImage(4, 40, 44, 43, image_data_bg03_right_btn_on);
            }
            else
            {
                spr3->pushImage(4, 40, 44, 43, image_data_bg03_right_btn_off);
            }
            if (bumpStor.getEnabled(elementIndex))
            {
                spr3->pushImage(4, 156, 44, 43, image_data_bg03_right_btn2_on);
            }
            else
            {
                spr3->pushImage(4, 156, 44, 43, image_data_bg03_right_btn2_off);
            }
            spr3->pushSprite(272, 0);
            spr3->deleteSprite();
        }

        if (modalOpen)
        {
            if (modalUpdate)
            {
                spr4->createSprite(272, 96);
                spr4->fillRoundRect(21 - 20, 44 - 43, 270, 94, 3, tft->color565(20, 10, 4));
                spr4->drawRoundRect(20 - 20, 43 - 43, 272, 96, 3, TFT_MAROON);
                spr4->setCursor(38 - 20, 63 - 43); // line 1
                spr4->setTextColor(TFT_WHITE, tft->color565(20, 10, 4));
                spr4->setTextSize(3);
                spr4->print("SSR Op State:");
                // tft->print("Element State");
                spr4->setCursor(38 - 20, 99 - 43); // line 2
                uint16_t color = TFT_YELLOW;
                if (op_state == ELM_PROT_OP_CODE::E_PROT_OKAY || op_state == ELM_PROT_OP_CODE::E_PROT_ACK)
                {
                    color = TFT_GREEN;
                }
                else if (op_state == ELM_PROT_OP_CODE::E_PROT_INIT_READY)
                {
                    color = TFT_GREENYELLOW;
                }
                else if (op_state == ELM_PROT_OP_CODE::E_PROT_RESET)
                {
                    color = TFT_ORANGE;
                }
                else if (op_state == ELM_PROT_OP_CODE::E_PROT_ERROR || op_state == ELM_PROT_OP_CODE::E_PROT_NACK)
                {
                    color = TFT_RED;
                }
                spr4->setTextColor(color, tft->color565(20, 10, 4));
                String op = OperationToString(op_state);
                spr4->print(op);
                modalUpdate = false;

                spr4->pushSprite(20, 43);
                spr4->deleteSprite();
            }
        }

        if (update)
        {
            if (settings->getDispTypeOptions() == 0)
            {
                showSysInfoSection(am, true);
            }
            else if (settings->getDispTypeOptions() == 1)
            {
                showSysIconsSection(am, true);
            }
        }
        else
        {
            if (settings->getDispTypeOptions() == 0)
            {
                showSysInfoSection(am, false);
            }
            if (settings->getDispTypeOptions() == 1)
            {
                showSysIconsSection(am, false);
            }
        }

        if (!modalOpen)
        {
            uint8_t elementIndex = aerGUI->getElementIndex();

            spr5->createSprite(272, 14);
            spr5->drawRect(0, 0, 272, 3, TFT_BLACK);
            spr5->pushImage(0, 3, 272, 11, image_data_bg02_mid);
            /*if (am->getAerPID(0)->PID_ON)
            {
                drawBarColorScroll(spr5, 1, 1); // green
                drawBarColorScroll(spr5, 2, 2); // blue
            }
            else
            {
                drawBarColorScroll(spr5, 0, 1); // red
                drawBarColorScroll(spr5, 3, 2); // purple
            }*/

#if AERPID_COUNT == 2
            spr5->setTextSize(1);
            spr5->setTextColor(elementIndex == 0 ? TFT_GOLD : 0x0ff6);
            spr5->setCursor(200, 5);
            spr5->print("Element #");
            spr5->print(elementIndex + 1);
#endif

            spr5->pushSprite(0, 120);
            spr5->deleteSprite();
        }
    }

    // scroll thingy on middle bar...
    void drawBarColorScroll(TFT_eSprite *spr, uint i, uint speed)
    {
        if (i > 3)
        {
            i = 3;
        }
        uint t = 1;
        if (i == 0 || i == 1)
        {
            t = 0;
        }
        uint x = bar_index[t] - 11;
        uint y = 3;
        switch (i)
        {
        case 0:
            spr->pushImage(x, y, 13, 11, image_data_bg03_bar_red);
            break;
        case 1:
            spr->pushImage(x, y, 13, 11, image_data_bg03_bar_green);
            break;
        case 2:
            spr->pushImage(x, y, 13, 11, image_data_bg03_bar_blue);
            break;
        case 3:
            spr->pushImage(x, y, 13, 11, image_data_bg03_bar_purple);
            break;
        default:
            spr->pushImage(x, y, 13, 11, image_data_bg03_bar_green);
            break;
        }
        bar_index[t] += 2 * speed;
        if (bar_index[t] > 280)
        {
            bar_index[t] = 0;
        }
    }

    // sys tray icons
    void showSysIconsSection(AerManager *am, bool update)
    {
        if (millis() < 3000)
        {
            return;
        }
        AerGUI *gui = am->getGUI();
        TFT_eSPI *tft = gui->getTFT();
        CommStor *coms = am->getComms();

        if (update)
        {
            tft->pushImage(0, 134, 68, 106, image_data_bg02_btmr);
            // tft->fillRect(0, 140, 64, 128, 0x31C8);
        }

        // Time
        DateTimeRTC t = am->getDateTime();
        if (t.year < 100 && t.month <= 12 && t.day < 32 && t.minute < 61)
        {
            tft->setCursor(4, 144);
            tft->setTextSize(2);
            tft->setTextColor(TFT_WHITE, 0x31C8);
            printDigit(tft, t.hour);
            if (t.second % 2 == 0)
            {
                tft->setTextColor(TFT_SKYBLUE, 0x31C8);
            }
            else
            {
                tft->setTextColor(TFT_BLACK, 0x31C8);
            }
            tft->print(":");
            tft->setTextColor(TFT_WHITE, 0x31C8);
            printDigit(tft, t.minute);

            tft->setCursor(4, 164);
            tft->setTextSize(1);
            tft->setTextColor(TFT_WHITE, 0x31C8);
            tft->print("20");
            printDigit(tft, t.year);
            tft->print("/");
            printDigit(tft, t.month);
            tft->print("/");
            printDigit(tft, t.day);
        }

        if (!update)
        {
            // return;
            if (tick_icon++ % 2 == 0)
            {
                return;
            }
        }

        uint x = 4;   // section base x
        uint y = 178; // section base y

        uint x1 = x + 0;  // wifi
        uint y1 = y + 0;  // wifi
        uint x2 = x + 34; // stat
        uint y2 = y + 33; // stat
        uint x3 = x + 0;  // usb
        uint y3 = y + 33; // usb
        uint x4 = x + 34; // net
        uint y4 = y + 0;  // net

        // wifi
        if (coms->getWifiEn())
        {
            if (am->getActiveWifi())
            {
                tft->pushImage(x1, y1, 24, 24, image_data_wifi_act, 0xc831);
            }
            else if (am->getConnectWifi())
            {
                tft->pushImage(x1, y1, 24, 24, image_data_wifi_on, 0xc831);
            }
            else
            {
                if (tick_icon >= 5)
                {
                    tft->pushImage(x1, y1, 24, 24, image_data_wifi_con, 0xc831);
                }
                else
                {
                    tft->pushImage(x1, y1, 24, 24, image_data_wifi_off, 0xc831);
                }
            }
        }
        else
        {
            tft->pushImage(x1, y1, 24, 24, image_data_wifi_off, 0xc831);
        }
        if (tick_icon > 10)
        {
            tick_icon = 0;
        }

        // ble
        /*if (coms->getBTEn())
        {
            tft->pushImage(x2, y2, 24, 24, image_data_ble_on, 0xc831);
        }
        else
        {
            tft->pushImage(x2, y2, 24, 24, image_data_ble_off, 0xc831);
        }*/

        TFT_eSprite *spr1 = gui->getSpriteBuffer(0);
        spr1->createSprite(24 + 6, 24 + 6);
        spr1->fillRoundRect(0, 0, 30, 30, 3, 0x2146);
        // spr1->drawRoundRect(0, 0, 30, 30, 3, 0x18E4);
        spr1->drawRoundRect(0, 0, 30, 30, 2, 0x18E4);
        spr1->setTextSize(1);
        // header
        spr1->setCursor(6, 2);
        spr1->setTextColor(0xd71d);
        spr1->println("CPU");
        // cpu0
        int _cpu0 = am->getCpu0Avg(5);
        spr1->setCursor(_cpu0 < 100 ? 6 : 3, 11);
        spr1->setTextColor(_cpu0 < 60 ? (_cpu0 < 40 ? 0x47A9 : 0xB7A8) : (_cpu0 < 85 ? 0xF528 : 0xF268));
        spr1->print(_cpu0, 10);
        spr1->println("%");
        // cpu1
        int _cpu1 = am->getCpu1Avg(5);
        spr1->setCursor(_cpu1 < 100 ? 6 : 3, 20);
        spr1->setTextColor(_cpu1 < 60 ? (_cpu1 < 40 ? 0x47A9 : 0xB7A8) : (_cpu1 < 85 ? 0xF528 : 0xF268));
        spr1->print(_cpu1, 10);
        spr1->println("%");

        spr1->pushSprite(x2 - 3, y2 - 3, TFT_BLACK);
        spr1->deleteSprite();

        // usb
        if (coms->getUSBEn() && (Serial && am->getActiveUsb()))
        {
            tft->pushImage(x3, y3, 24, 24, image_data_usb_act, 0xc831);
        }
        else if (coms->getUSBEn() && (Serial))
        {
            tft->pushImage(x3, y3, 24, 24, image_data_usb_on, 0xc831);
        }
        else
        {
            tft->pushImage(x3, y3, 24, 24, image_data_usb_off, 0xc831);
        }

        // web
        if (true && am->getConnectWifi()) // TODO: get web status
        {
            tft->pushImage(x4, y4, 24, 24, image_data_web_on, 0xc831);
        }
        else
        {
            tft->pushImage(x4, y4, 24, 24, image_data_web_off, 0xc831);
        }
    }

    // show cpu and memory info
    void showSysInfoSection(AerManager *am, bool update)
    {
        AerGUI *gui = am->getGUI();
        TFT_eSPI *lcd = gui->getTFT();

        if (update)
        {
            lcd->pushImage(0, 134, 68, 106, image_data_bg02_btmr);
            // lcd->fillRect(0, 138, 68, 102, 0x31C8);
        }

        showMemorySection(am); // RAM % bar
        showCPUSections(am);   // CPU % bar (2x7)

        // showCPUSection(am); // CPU % bar (2x1)

        lcd->setTextSize(1);
        lcd->setTextColor(TFT_WHITE, 0x31C8);
        lcd->setCursor(4, 141);
        lcd->print("SYS MEMORY");

        lcd->setTextSize(1);
        lcd->setTextColor(TFT_WHITE, 0x31C8);
        lcd->setCursor(4, 170);
        lcd->print("CPU0");
        lcd->setCursor(37, 170);
        lcd->print("CPU1");
    }

    void showMemorySection(AerManager *am)
    {
        if (millis() - lastMonitorUpdateTime < 400 || millis() < 3600)
        {
            return;
        }
        lastMonitorUpdateTime = millis();

        // object pointers
        AerGUI *gui = am->getGUI();
        TFT_eSPI *lcd = gui->getTFT();

        uint w = 62;     // mem bar width
        uint h = 15 / 3; // mem bar height
        uint x = 2;      // mem loc x
        uint y = 150;
        uint y1 = y;      // mem loc y
        uint y2 = y1 + h; // mem loc y
        uint y3 = y2 + h; // mem loc y

        // executable code mem
        multi_heap_info_t infoA;
        heap_caps_get_info(&infoA, MALLOC_CAP_EXEC);
        uint32_t max_heapA = infoA.total_free_bytes + infoA.total_allocated_bytes;
        uint32_t free_heapA = infoA.total_free_bytes;
        uint32_t fPercentA = ((float)free_heapA / max_heapA) * w;

        // 8bit data mem
        multi_heap_info_t infoB;
        heap_caps_get_info(&infoB, MALLOC_CAP_8BIT);
        uint32_t max_heapB = infoB.total_free_bytes + infoB.total_allocated_bytes;
        uint32_t free_heapB = infoB.total_free_bytes;
        uint32_t fPercentB = ((float)free_heapB / max_heapB) * w;

        // internal memory
        uint32_t max_heapC = ESP.getHeapSize();
        uint32_t free_heapC = ESP.getFreeHeap();
        uint32_t fPercentC = ((float)free_heapC / max_heapC) * w;

        uint32_t color1 = color565(128, 128, 0);                                                  // free mem (olive)
        uint32_t color2 = color565(255 - (fPercentA * 0.6), 108 + (fPercentA * 0.4), 0);          // used mem
        uint32_t color3 = color565(255 - fPercentB, std::min(208 + fPercentB, (uint32_t)255), 0); // used mem
        uint32_t color4 = color565(150 + (fPercentC * 0.6), 255 - (fPercentC * 0.4), 30);         // used mem

        // widths A bar 1
        uint w1a = std::min(fPercentA, w);
        uint w2a = std::min(w - fPercentA, w);
        // widths B bar 2
        uint w1b = std::min(fPercentB, w);
        uint w2b = std::min(w - fPercentB, w);
        // widths C bar 3
        uint w1c = std::min(fPercentC, w);
        uint w2c = std::min(w - fPercentC, w);

        // bar 1
        lcd->fillRect(x + w2a, y1, w1a, h, color1);
        lcd->fillRect(x, y1, w2a, h, color2);
        // bar 2
        lcd->fillRect(x + w2b, y2, w1b, h, color1);
        lcd->fillRect(x, y2, w2b, h, color3);
        // bar 3
        lcd->fillRect(x + w2c, y3, w1c, h, color1);
        lcd->fillRect(x, y3, w2c, h, color4);

        // border around
        lcd->drawRect(x - 1, y - 1, w + 2, (h * 3) + 2, TFT_DARKGREY);
    }

    void showCPUSections(AerManager *am)
    {
        uint tWidth = 30;
        uint tHeight = 58;
        uint siz = am->getCpuBarCount();
        uint width = (uint)((float)tWidth / siz);
        for (uint i = 0; i < siz; i++)
        {
            showCpu0Bar(am, i, width, tHeight);
            showCpu1Bar(am, i, width, tHeight);
        }
    }

    void showCpu0Bar(AerManager *am, uint i, uint width, uint height)
    {
        AerGUI *gui = am->getGUI();
        TFT_eSPI *lcd = gui->getTFT();

        // slot % usage for cpu 0
        int _cpu0 = am->getCpu0s()[i];

        uint32_t color1 = TFT_GREENYELLOW;      // used ticks
        uint32_t color2 = color565(28, 64, 32); // free ticks

        int r = std::min(255, 90 + (int)(_cpu0 * (_cpu0 > 64 ? _cpu0 > 90 ? 3.0 : 2.15 : 1)));
        int g = std::max(10, 255 - (int)(_cpu0 * (_cpu0 > 47 ? _cpu0 > 75 ? 2.5 : 1.20 : 1)));
        int b = std::max(0, 150 - (int)(_cpu0 * 1.5));
        uint32_t color3 = color565(r, g, b); // scaled color ticks

        int x1 = 2 + (i * width); // cpu0 loc x
        int y = 180;              // loc y
        int w = width;            // width (30)
        int h = height;           // height

        int cpu0 = (_cpu0 / 100.f) * h; // convert to bounds

        int h1_0 = std::max(0, std::min(cpu0, h));
        int h2_0 = std::max(0, std::min(h - cpu0, h));

        lcd->fillRect(x1, h2_0 + y, w, h1_0, _cpu0 < 20 ? color1 : color3);
        lcd->fillRect(x1, y, w, h2_0, color2);

        lcd->drawRect(2 - 1, y - 1, 30 + 2, h + 2, color565(16, 33, 30));
    }

    void showCpu1Bar(AerManager *am, uint i, uint width, uint height)
    {
        AerGUI *gui = am->getGUI();
        TFT_eSPI *lcd = gui->getTFT();

        // slot % usage for cpu 1
        int _cpu1 = am->getCpu1s()[i];

        uint32_t color1 = TFT_GREENYELLOW;      // used ticks
        uint32_t color2 = color565(28, 64, 32); // free ticks

        int r = std::min(255, 90 + (int)(_cpu1 * (_cpu1 > 64 ? _cpu1 > 90 ? 3.0 : 2.15 : 1)));
        int g = std::max(10, 255 - (int)(_cpu1 * (_cpu1 > 47 ? _cpu1 > 75 ? 2.5 : 1.20 : 1)));
        int b = std::max(0, 150 - (int)(_cpu1 * 1.5));
        uint32_t color3 = color565(r, g, b); // scaled color ticks

        int x2 = 35 + (i * width); // cpu1 loc x
        int y = 180;               // loc y
        int w = width;             // width (30)
        int h = height;            // height

        int cpu1 = (_cpu1 / 100.f) * h; // convert to bounds

        int h1_1 = std::max(0, std::min(cpu1, h));
        int h2_1 = std::max(0, std::min(h - cpu1, h));

        lcd->fillRect(x2, h2_1 + y, w, h1_1, _cpu1 < 20 ? color1 : color3);
        lcd->fillRect(x2, y, w, h2_1, color2);

        lcd->drawRect(35 - 1, y - 1, 30 + 2, h + 2, color565(16, 33, 30));
    }

    void showCPUSection(AerManager *am)
    {
        AerGUI *gui = am->getGUI();
        TFT_eSPI *lcd = gui->getTFT();

        uint x1 = 2;  // cpu0 loc x
        uint x2 = 35; // cpu1 loc x

        uint y = 180; // loc y
        uint w = 30;  // width
        uint h = 58;  // height

        uint32_t color1 = TFT_GREENYELLOW;    // used ticks
        uint32_t color2 = color565(0, 96, 0); // free ticks

        int cpu0 = (am->getCpu0() / 100.f) * h;
        int cpu1 = (am->getCpu1() / 100.f) * h;

        int h1_0 = std::max(0, std::min(cpu0, (int)h));
        int h2_0 = std::max(0, std::min((int)h - cpu0, (int)h));
        int h1_1 = std::max(0, std::min(cpu1, (int)h));
        int h2_1 = std::max(0, std::min((int)h - cpu1, (int)h));

        lcd->fillRect(x1, h2_0 + y, w, h1_0, color1);
        lcd->fillRect(x1, y, w, h2_0, color2);

        lcd->fillRect(x2, h2_1 + y, w, h1_1, color1);
        lcd->fillRect(x2, y, w, h2_1, color2);
    }

    void showTempSection(AerManager *am, bool update, ThermalUnitsType unitType)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        bool updatesProcessed = false;

        uint8_t elementIndex = gui->getElementIndex();
        double temp = am->getAerPID(elementIndex)->MES_TEMP;
        double temp_avg = am->getAerPID(elementIndex)->AVG_TEMP;
        double temp_set = am->getAerPID(elementIndex)->SET_TEMP;

        if (lastTemp == temp && lastTemp_set == temp_set && !update)
        {
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        TFT_eSprite *spr1 = gui->getSpriteBuffer(0);
        TFT_eSprite *spr2 = gui->getSpriteBuffer(1);

        // lcd->setTextWrap(false);

        std::string tempStr;
        std::string tempAvgStr;
        std::string tempSetStr;
        const uint16_t *image;
        if (unitType == ThermalUnitsType::FAHRENHEIT)
        {
            if (temp >= 537.7)
            {
                tempStr = paddedStr(toStrF(temp, 0, true), 4);
            }
            else
            {
                tempStr = paddedStr(toStrF(temp, 1, true), 5);
            }
            if (temp_avg >= 537.7)
            {
                tempAvgStr = paddedStr(toStrF(temp_avg, 0, true), 4);
            }
            else
            {
                tempAvgStr = paddedStr(toStrF(temp_avg, 1, true), 5);
            }
            if (temp_set >= 537.7)
            {
                tempSetStr = paddedStr(toStrF(temp_set, 0, true), 4);
            }
            else
            {
                tempSetStr = paddedStr(toStrF(temp_set, 1, true), 5);
            }
            image = image_data_temp_f_l2;
        }
        else if (unitType == ThermalUnitsType::CELSIUS)
        {
            tempStr = paddedStr(toStrC(temp, 1), 5);
            tempAvgStr = paddedStr(toStrC(temp_avg, 1), 5);
            tempSetStr = paddedStr(toStrC(temp_set, 0), 4);
            image = image_data_temp_c_l2;
        }
        else if (unitType == ThermalUnitsType::KELVIN)
        {
            tempStr = paddedStr(toStrK(temp, 0, true), 5);
            tempAvgStr = paddedStr(toStrK(temp_avg, 0, true), 4);
            tempSetStr = paddedStr(toStrK(temp_set, 0, true), 4);
            image = image_data_temp_k_l2;
        }
        else
        {
            tempStr = "";
            tempSetStr = "";
            image = {};
        }

        if (lastTemp != temp || update)
        {
            // Serial.println("[ShowTempSection] >> Set Temp Update.");
            spr1->createSprite(272, 46);
            spr1->pushImage(0, 0, 272, 46, image_data_bg03_top);
            // spr1->fillRect(0, 0, 272, 42, color565(101, 109, 118));
            spr1->setCursor(8, 10);
            spr1->setTextColor(TFT_WHITE);
            spr1->setTextSize(3);
            spr1->print("Measured Value");
            spr1->pushSprite(0, 0);
            spr1->deleteSprite();

            uint sz = 64;
            // temperature average
            spr1->createSprite(272 - sz, 74);
            spr1->fillRect(0, 0, 272 - sz, 2, 0x2966);
            spr1->fillRect(0, 2, 272 - sz, 76, 0x31C8);
            spr1->setCursor(20, 61 - 42);
            spr1->setTextColor(TFT_WHITE, 0x31C8);
            spr1->setTextSize(6);
            spr1->print(tempAvgStr.c_str());
            spr1->pushSprite(0, 46);
            spr1->deleteSprite();

            // temperature live
            spr1->createSprite(sz, 74);
            spr1->fillRect(0, 0, sz, 2, 0x2966);
            spr1->fillRect(0, 2, sz, 76, 0x31C8);
            if (image)
            {
                spr1->pushImage(14, 65 - 40, 36, 32, image);
            }
            spr1->setCursor(0, 56);
            spr1->setTextColor(TFT_SKYBLUE, 0x31C8);
            spr1->setTextSize(2);
            spr1->print(tempStr.c_str());
            spr1->pushSprite(272 - sz, 46);
            spr1->deleteSprite();

            lastTemp = temp;
            updatesProcessed = true;
        }

        if (lastTemp_set != temp_set || update)
        {
            // Serial.println("[ShowTempSection] >> Set Temp Update.");
            spr2->createSprite(204, 38);
            spr2->pushImage(0, 0, 204, 38, image_data_bg03_btm_b);
            // spr2->fillRect(3, 3, 204, 32, color565(101, 109, 118));
            // spr1->fillRect(3, 7, 204, 2, 0x2966);
            spr2->fillRect(0, 38, 3, 38, TFT_BLACK);
            spr2->setCursor(15, 8);
            spr2->setTextColor(TFT_WHITE);
            spr2->setTextSize(3);
            spr2->print("Set Value");
            spr2->pushSprite(68, 134);
            spr2->deleteSprite();

            spr2->createSprite(204, 106 - 38);
            spr2->fillRect(3, 0, 204 - 3, 106 - 38, 0x31C8);
            spr2->setCursor(88 - 68, 190 - 134 - 38);
            spr2->setTextColor(TFT_WHITE, 0x31C8);
            spr2->setTextSize(4);
            spr2->print(tempSetStr.c_str());
            if (image)
            {
                spr2->pushImage(222 - 68, 190 - 134 - 38, 36, 32, image);
            }
            spr2->pushSprite(68, 134 + 38);
            spr2->deleteSprite();
            lastTemp_set = temp_set;
            updatesProcessed = true;
        }
        image = {};
        tempStr = "";
        tempSetStr = "";
    }

    void showMainMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(19, 19, 282, 202, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_LIGHTGREY);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(52, 24);
        lcd->print("Main Menu");
        // printSelections(gui, menu, mindex, TFT_DARKGREY);
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showPIDMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(19, 19, 282, 202, 7, TFT_DARKCYAN);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);
        }

        uint8_t elementIndex = gui->getElementIndex();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKCYAN);
        lcd->setTextSize(4);
        lcd->setCursor(45, 24);
        lcd->print("PID ");
        lcd->print(elementIndex + 1);
        lcd->print(" Menu");
        lcd->setTextSize(3);
        drawSelections(gui, menu, mindex, TFT_DARKCYAN);
        lastindex = mindex;
    }

    void showPIDMenuP(AerGUI *gui, float kP, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKCYAN);
        lcd->setTextSize(4);
        lcd->setCursor(46, 24);
        lcd->print("P Setting");
        lcd->setTextSize(3);

        lcd->setTextColor(TFT_GOLD, TFT_DARKCYAN);
        lcd->setCursor(64, 70);
        lcd->print(kP);
        // lcd->print("1.0");
    }

    void showPIDMenuI(AerGUI *gui, float kI, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKCYAN);
        lcd->setTextSize(4);
        lcd->setCursor(46, 24);
        lcd->print("I Setting");
        lcd->setTextSize(3);

        lcd->setTextColor(TFT_GOLD, TFT_DARKCYAN);
        lcd->setCursor(64, 70);

        char b[10];
        for (int i = 0; i < 10; i++)
        {
            b[i] = '\0';
        }
        dtostrf(kI, 5, 3, b);
        lcd->print(b);
        // lcd->print("0.001");
    }

    void showPIDMenuD(AerGUI *gui, float kD, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKCYAN);
        lcd->setTextSize(4);
        lcd->setCursor(46, 24);
        lcd->print("D Setting");
        lcd->setTextSize(3);

        lcd->setTextColor(TFT_GOLD, TFT_DARKCYAN);
        lcd->setCursor(64, 70);
        lcd->print(kD);
        // lcd->print("10.0");
    }

    void showPIDTuneMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(19, 19, 282, 202, 7, TFT_DARKCYAN);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKCYAN);
        lcd->setTextSize(4);
        lcd->setCursor(42, 24);
        lcd->print("PID Tuning");
        lcd->setTextSize(3);
        drawSelections(gui, menu, mindex, TFT_DARKCYAN);
        lastindex = mindex;
    }

    void showPIDAutoMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 18, 18, 284, 204, 7, bgColor, TFT_CYAN);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(42, 24);
            lcd->print("Auto Tune");
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_PID_AUTO_TOGGLE)
            {
                uint8_t elementIndex = gui->getElementIndex();
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? gui->isCursorModify() ? TFT_RED : TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(2);
                spr->print(am->getAerPID(elementIndex)->AUTO_TUNE_ACTIVE ? "ON" : "OFF");
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        // end printSelections
        lastindex = mindex;
    }

    void showPIDpwmFactorMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 18, 18, 284, 204, 7, bgColor, TFT_CYAN);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(42, 24);
            lcd->print("PWM Factor");
        }

        uint8_t elementIndex = gui->getElementIndex();

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_PID_PWM_FACTOR_VAR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? gui->isCursorModify() ? TFT_RED : TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(2);
                // spr->print(am->getAerPID(0)->PWM_ScaleFactor);
                char b[10];
                for (int i = 0; i < 10; i++)
                {
                    b[i] = '\0';
                }
                dtostrf((double)am->getAerPID(elementIndex)->PWM_ScaleFactor * 100, 5, 1, b);
                spr->print(b);
                spr->print("%");
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        // end printSelections
        lastindex = mindex;
    }

    void showPIDpwmCycleMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 18, 18, 284, 204, 7, bgColor, TFT_CYAN);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(42, 24);
            lcd->print("PWM Cycle");
        }

        uint8_t elementIndex = gui->getElementIndex();

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_PID_PWM_CYCLE_VAR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? gui->isCursorModify() ? TFT_RED : TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(2);
                spr->print(am->getAerPID(elementIndex)->PWM_CycleTime);
                spr->print(" ms");
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        // end printSelections
        lastindex = mindex;
    }

    void showPIDpwmFreqMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 18, 18, 284, 204, 7, bgColor, TFT_CYAN);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(42, 24);
            lcd->print("PWM Freq");
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_PID_PWM_FREQ_VAR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(2);
                spr->print(am->getAerPID(0)->getPwmFreq());
                spr->print(" Hz");
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        // end printSelections
        lastindex = mindex;
    }

    void showLEDMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update) // Indexes match and update is false; return
        {
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        LightsStor *lightsObj = am->getLights();
        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, 0x5BB0);
        lcd->setTextSize(4);
        lcd->setCursor(42, 24);
        if (mlvl == MENU_MAIN_LED)
        {
            lcd->setCursor(65, 24);
            lcd->print("LED Menu");
        }
        else if (mlvl == MENU_LED_SETUP)
        {
            lcd->print("LED:Color");
        }
        else if (mlvl == MENU_LED_STATUS)
        {
            lcd->print("L:Status");
        }
        else if (mlvl == MENU_LED_PRESET)
        {
            lcd->print("L:Presets");
        }

        uint8_t offset = 30;
        lcd->setTextSize(3);
        // printSelections(gui, menu, mindex, TFT_GOLD);
        for (int i = 0; i < items.size(); i++)
        {
            lcd->setTextColor(TFT_WHITE, 0x5BB0);
            lcd->setCursor(32, 65 + (offset * i));

            if (isPresetActive(lightsObj, items[i]))
            {
                lcd->pushImage(32 - 2, 65 + (offset * i) - 2, 28, 28, image_data_favs_sel, 0xffff);
            }
            else
            {
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], false);
            }

            if (items[i] == MENU_LED_MAIN_TOGGLE)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, 0x5BB0);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, 0x5BB0);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));

                if (lightsObj->isEnabled())
                {
                    lcd->setTextColor(TFT_GREEN, 0x5BB0);
                    lcd->print(" On ");
                }
                else
                {
                    lcd->setTextColor(TFT_RED, 0x5BB0);
                    lcd->print(" Off");
                }
                lcd->setTextColor(TFT_CYAN, 0x5BB0);
            }
            else if (items[i] == MENU_LED_STATUS_TOGGLE)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, 0x5BB0);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, 0x5BB0);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));

                if (lightsObj->statusEnabled())
                {
                    lcd->setTextColor(TFT_GREEN, 0x5BB0);
                    lcd->print(" On ");
                }
                else
                {
                    lcd->setTextColor(TFT_RED, 0x5BB0);
                    lcd->print(" Off");
                }
                lcd->setTextColor(TFT_CYAN, 0x5BB0);
            }
            else if (items[i] == MENU_LED_AMBIENT_TOGGLE)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, 0x5BB0);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, 0x5BB0);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));

                if (lightsObj->ambientEnabled())
                {
                    lcd->setTextColor(TFT_GREEN, 0x5BB0);
                    lcd->print(" On ");
                }
                else
                {
                    lcd->setTextColor(TFT_RED, 0x5BB0);
                    lcd->print(" Off");
                }
                lcd->setTextColor(TFT_CYAN, 0x5BB0);
            }
            else if (items[i] == MENU_LED_STATUS_TOGGLE)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, 0x5BB0);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, 0x5BB0);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));

                if (lightsObj->statusEnabled())
                {
                    lcd->setTextColor(TFT_GREEN, 0x5BB0);
                    lcd->print(" On ");
                }
                else
                {
                    lcd->setTextColor(TFT_RED, 0x5BB0);
                    lcd->print(" Off");
                }
                lcd->setTextColor(TFT_CYAN, 0x5BB0);
            }
            else if (items[i] == mindex)
            {
                lcd->setTextColor(TFT_CYAN, 0x5BB0);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
            else
            {
                lcd->setTextColor(TFT_WHITE, 0x5BB0);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
        }
        lastindex = mindex;
    }

    void showLEDColorMenu(AerManager *am, AerGUI *gui, bool update)
    {
        return;
    }

    void showLEDBrightMenu(AerManager *am, AerGUI *gui, bool update)
    {
        /*if (update && change)
        {
            lcd->fillRoundRect(20, 20, 280, 200, 7, 0x5BB0);
            lcd->drawRoundRect(18, 18, 284, 204, 7, 0x3209);
        }*/
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        LightsStor *ledObj = am->getLights();
        TFT_eSPI *lcd = gui->getTFT();
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, 0x5BB0);
        lcd->setTextSize(4);
        int favindex = convertMenuLvlToFav(mlvl);
        std::string brightStr = paddedStr(toStrC(ledObj->getBright(), 1), 5);
        if (lastindex == mindex && !update)
        {
            if (lastLedBright != ledObj->getBright())
            {
                lcd->setTextSize(5);
                lcd->setCursor(70, 90);
                lcd->print(brightStr.c_str());
                lastLedBright = ledObj->getBright();
            }
            // Indexes match and update is false; return
            return;
        }

        std::vector<uint16_t> items;
        bool bSelect = false;
        // uint8_t offset = 30;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextSize(4);
        lcd->setCursor(33, 30);
        lcd->print("Brightness");
        lcd->setTextSize(5);
        lcd->setCursor(70, 90);
        lcd->print(brightStr.c_str());

        lastLedBright = ledObj->getBright();

        lastindex = mindex;
    }

    bool isPresetActive(LightsStor *lightsObj, uint16_t mindex)
    {
        // LightsStor *lightsObj = gui->getLights();
        switch (mindex)
        {
        case MENU_LED_RWAVE:
            return lightsObj->preRainEnabled();
        case MENU_LED_RAINBOW:
            return lightsObj->preRain2Enabled();
        case MENU_LED_RPULSE:
            return lightsObj->preRain3Enabled();
        case MENU_LED_CSHIFT:
            return lightsObj->preShiftEnabled();
        case MENU_LED_CPULSE:
            return lightsObj->prePulseEnabled();
        case MENU_LED_ASTATIC:
            return lightsObj->ambientEnabled();
        case MENU_LED_ABLINK:
            return lightsObj->amBlinkEnabled();
        case MENU_LED_APULSE:
            return lightsObj->amPulseEnabled();
        default:
            return false;
        }
        return false;
    }

    void showFavsMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREEN);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_GREEN);
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREEN);
        lcd->setTextSize(4);
        lcd->setCursor(40, 20);
        lcd->print("Favs Menu");
        lcd->setTextSize(3);
        drawSelections(gui, menu, mindex, TFT_DARKGREEN);
        lastindex = mindex;
    }

    void showExpFavMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        FavsStor *favsObj = am->getFavs();
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREEN);
        lcd->setTextSize(4);
        lcd->setCursor(40, 24);
        lcd->print(gui->getMenuName(mlvl));
        lcd->setTextSize(3);
        printSelections(gui, menu, mindex, TFT_DARKGREEN);
        lastindex = mindex;
    }

    void showExpFavNameMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        FavsStor *favsObj = am->getFavs();
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREEN);
        lcd->setTextSize(4);
        lcd->setCursor(40, 24);
        switch (mlvl)
        {
        case MENU_FAV_1N: /* Fav1 Name */
            lcd->print(favsObj->getName(1));
            break;
        case MENU_FAV_2N: /* Fav1 Name */
            lcd->print(favsObj->getName(2));
            break;
        case MENU_FAV_3N: /* Fav1 Name */
            lcd->print(favsObj->getName(3));
            break;
        case MENU_FAV_4N: /* Fav1 Name */
            lcd->print(favsObj->getName(4));
            break;
        default:
            lcd->print("Default Name");
            break;
        }

        lcd->setTextSize(3);
        printSelections(gui, menu, mindex, TFT_DARKGREEN);
        lastindex = mindex;
    }

    void showExpFavTempMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        FavsStor *favsObj = am->getFavs();
        TFT_eSPI *lcd = gui->getTFT();
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREEN);
        lcd->setTextSize(4);
        int favindex = convertMenuLvlToFav(mlvl);
        double favtemp = favsObj->getTemp(favindex);

        std::string tempStr;
        const uint16_t *image;
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            tempStr = paddedStr(toStrF(favtemp, 1, true), 5);
        }
        else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
        {
            tempStr = paddedStr(toStrC(favtemp, 1), 5);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            tempStr = paddedStr(toStrK(favtemp, 0, true), 5);
        }
        else
        {
            tempStr = "";
        }

        if (lastindex == mindex && !update)
        {
            if (lastFavTemp != favtemp)
            {
                lcd->setCursor(70, 90);
                lcd->print(tempStr.c_str());
                lastFavTemp = favtemp;
            }
            // Indexes match and update is false; return
            return;
        }

        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setCursor(40, 24);
        switch (mlvl)
        {
        case MENU_FAV_1T:
            lcd->print("Fav-Temp:1");
            break;
        case MENU_FAV_2T:
            lcd->print("Fav-Temp:2");
            break;
        case MENU_FAV_3T:
            lcd->print("Fav-Temp:3");
            break;
        case MENU_FAV_4T:
            lcd->print("Fav-Temp:4");
            break;
        default:
            lcd->print("Default Temp");
            break;
        }
        lcd->setCursor(70, 90);
        lcd->print(tempStr.c_str());
        lastFavTemp = favtemp;

        lastindex = mindex;
    }

    void showBumpMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_SKYBLUE);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(45, 24);
        lcd->print("Bump Menu");
        lcd->setTextSize(3);
        printSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showBumpTempMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        BumpStor *bumpObj = am->getBump();
        TFT_eSPI *lcd = gui->getTFT();
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        int favindex = convertMenuLvlToFav(mlvl);
        if (lastindex == mindex && !update)
        {
            if (lastBumpTemp != bumpObj->getTemp())
            {
                std::string temp = "";
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    temp = toStrUnitF(bumpObj->getTemp(), 2, true);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    temp = toStrUnitC(bumpObj->getTemp());
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    temp = toStrUnitK(bumpObj->getTemp(), 1, true);
                }

                lcd->setTextSize(5);
                lcd->setCursor(42, 100);
                lcd->print(temp.c_str());
                lastBumpTemp = bumpObj->getTemp();
            }
            // Indexes match and update is false; return
            return;
        }

        std::vector<uint16_t> items;
        bool bSelect = false;
        // uint8_t offset = 30;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        std::string temp = "";
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            temp = toStrUnitF(bumpObj->getTemp(), 2, true);
        }
        else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
        {
            temp = toStrUnitC(bumpObj->getTemp());
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            temp = toStrUnitK(bumpObj->getTemp(), 1, true);
        }

        lcd->setTextSize(4);
        lcd->setCursor(33, 30);
        lcd->print("Bump Amount");
        lcd->setTextSize(5);
        lcd->setCursor(42, 100);
        lcd->print(temp.c_str());

        lastBumpTemp = bumpObj->getTemp();

        lastindex = mindex;
    }

    void showBumpTimeMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        BumpStor *bumpObj = am->getBump();
        TFT_eSPI *lcd = gui->getTFT();
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);

        String timeout = convertBumpSeconds(bumpObj->getTime());

        if (lastindex == mindex && !update)
        {
            if (lastBumpTime != bumpObj->getTime())
            {
                lcd->setCursor(70, 100);
                lcd->setTextSize(4);
                lcd->print(timeout);
                lastBumpTime = bumpObj->getTime();
            }
            // Indexes match and update is false; return
            return;
        }

        std::vector<uint16_t> items;
        bool bSelect = false;
        // uint8_t offset = 30;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextSize(4);
        lcd->setCursor(33, 30);
        lcd->print("Bump Length");
        lcd->setTextSize(4);
        lcd->setCursor(70, 100);
        lcd->print(timeout);
        lcd->setTextSize(2);
        lcd->setCursor(87, 130);
        lcd->print("hr : min : sec");
        lcd->setTextSize(4);
        lastBumpTime = bumpObj->getTime();

        lastindex = mindex;
    }

    void showGraphMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print("Graphs");
        lcd->setTextSize(3);
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showSysMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            // lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            // lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_LIGHTGREY);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, TFT_DARKGREY, TFT_LIGHTGREY);
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(67, 24);
        lcd->print("Systems");
        lcd->setTextSize(3);
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showWiFiMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 18, 18, 284, 204, 7, bgColor, TFT_GOLD);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(42, 24);
            lcd->print("WiFi Menu");
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_WIFI_HOSTNAME)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(2);
                spr->print(am->getNetVars()->hostname);
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }
            if (items[i] == MENU_WIFI_IP_ADDR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                IPAddress ip = am->getNetVars()->ip;
                spr->setTextSize(2);
                spr->print(ip[0], 10);
                spr->print(".");
                spr->print(ip[1], 10);
                spr->print(".");
                spr->print(ip[2], 10);
                spr->print(".");
                spr->print(ip[3], 10);
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }
            if (items[i] == MENU_WIFI_MAC_ADDR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                uint8_t *mac = am->getNetVars()->mac;
                spr->setTextSize(2);
                spr->print(mac[0], 16);
                spr->print(":");
                spr->print(mac[1], 16);
                spr->print(":");
                spr->print(mac[2], 16);
                spr->print(":");
                spr->print(mac[3], 16);
                spr->print(":");
                spr->print(mac[4], 16);
                spr->print(":");
                spr->print(mac[5], 16);
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_WIFI_NETWORK_ENABLED)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextSize(3);
                spr->setTextWrap(false);
                // print text
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
                spr->setTextColor(commstor.getWifiEn() ? TFT_GREEN : TFT_RED);
                spr->setCursor(125, 5);
                spr->print(commstor.getWifiEn() ? "ON" : "OFF");
                // push to screen
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        // end printSelections
        lastindex = mindex;
    }

    void showWifiScanMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        NetworkVars *net = am->getNetVars();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_MAROON);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE);
            lcd->setTextSize(4);
            lcd->setCursor(50, 24);
            // gui->printIcon(0, 1, MENU_WIFI_NETWORK_SCAN, true);
            lcd->print("WiFi Scan");
            lcd->setTextSize(3);
        }

        drawSelections(gui, menu, mindex, bgColor);

        TFT_eSprite *spr = gui->getSpriteBuffer(0);

        spr->createSprite(250, 40);
        spr->fillRect(0, 0, 250, 40, TFT_DARKGREY);
        spr->setTextWrap(false);
        spr->setTextColor(net->scanning ? TFT_ORANGE : (net->scanned ? TFT_GREEN : (net->doscan ? TFT_RED : TFT_YELLOW)));
        spr->setTextSize(3);
        spr->setCursor(3, 0);
        spr->print(net->scanned ? "Scan Complete!" : "Scanning WiFi!");
        spr->pushSprite(34, 120);
        spr->deleteSprite();

        spr->createSprite(252, 48);
        spr->fillRect(0, 0, 252, 48, TFT_DARKGREY);
        spr->setCursor(0, 0);
        spr->setTextSize(2);
        spr->setTextWrap(true);
        spr->setTextColor(color565(237, 232, 230));
        spr->print(net->scanned ? "Check 'Find Nearby'  for scan results." : "Please Wait!!  This  will take a moment...");
        spr->pushSprite(34, 170);
        spr->deleteSprite();

        lastindex = mindex;
    }

    void showWifiJoinMenu(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        NetworkVars *net = am->getNetVars();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_MAROON);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE);
            lcd->setTextSize(4);
            lcd->setCursor(50, 24);
            // gui->printIcon(0, 1, MENU_WIFI_NETWORK_SCAN, true);
            lcd->print("WiFi Join");
            lcd->setTextSize(3);
        }

        drawSelections(gui, menu, mindex, bgColor);

        TFT_eSprite *spr = gui->getSpriteBuffer(0);

        spr->createSprite(250, 40);
        spr->fillRect(0, 0, 250, 40, TFT_DARKGREY);
        spr->setTextWrap(false);
        spr->setTextColor(commstor.getSSIDSet() && commstor.hasPSK() ? TFT_GREEN : TFT_RED);
        spr->setTextSize(3);
        spr->setCursor(3, 0);

        spr->print(commstor.getSSIDSet() && commstor.hasPSK() ? "Setup Complete!" : "Config Error!");
        spr->pushSprite(34, 120);
        spr->deleteSprite();

        spr->createSprite(252, 48);
        spr->fillRect(0, 0, 252, 48, TFT_DARKGREY);
        spr->setCursor(0, 0);
        spr->setTextSize(2);
        spr->setTextWrap(true);
        spr->setTextColor(color565(237, 232, 230));
        spr->print(commstor.getSSIDSet() && commstor.hasPSK() ? "Pleaes reboot for settings to take effect." : "Please Check Setup!!");
        spr->pushSprite(34, 170);
        spr->deleteSprite();

        lastindex = mindex;
    }

    void showWifiJoinError(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        NetworkVars *net = am->getNetVars();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_RED);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE);
            lcd->setTextSize(4);
            lcd->setCursor(50, 24);
            lcd->print("WiFi Join");
            lcd->setTextSize(3);
        }

        drawSelections(gui, menu, mindex, bgColor);

        TFT_eSprite *spr = gui->getSpriteBuffer(0);

        spr->createSprite(250, 40);
        spr->fillRect(0, 0, 250, 40, TFT_DARKGREY);
        spr->setTextWrap(false);
        spr->setTextColor(commstor.getSSIDSet() ? TFT_YELLOW : TFT_RED);
        spr->setTextSize(3);
        spr->setCursor(13, 0);

        spr->print("Setup Error!");
        spr->pushSprite(34, 115);
        spr->deleteSprite();

        spr->createSprite(252, 48);
        spr->fillRect(0, 0, 252, 48, TFT_DARKGREY);
        spr->setCursor(0, 0);
        spr->setTextSize(2);
        spr->setTextWrap(true);
        spr->setTextColor(color565(237, 232, 230));
        spr->print("Please Check Device  Setup. SSID and Pass-key must be set!");
        spr->pushSprite(34, 158);
        spr->deleteSprite();

        lastindex = mindex;
    }

    void showWifiDeleteMessage(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        NetworkVars *net = am->getNetVars();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_RED);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE);
            lcd->setTextSize(4);
            lcd->setCursor(50, 24);
            lcd->print("WiFi Leave");
            lcd->setTextSize(3);
        }

        drawSelections(gui, menu, mindex, bgColor);

        TFT_eSprite *spr = gui->getSpriteBuffer(0);

        spr->createSprite(250, 40);
        spr->fillRect(0, 0, 250, 40, TFT_DARKGREY);
        spr->setTextWrap(false);
        spr->setTextColor(commstor.getWifiEn() ? TFT_ORANGE : TFT_RED);
        spr->setTextSize(3);
        spr->setCursor(13, 0);

        spr->print("WiFi Cleared!");
        spr->pushSprite(34, 115);
        spr->deleteSprite();

        spr->createSprite(252, 48);
        spr->fillRect(0, 0, 252, 48, TFT_DARKGREY);
        spr->setCursor(0, 0);
        spr->setTextSize(2);
        spr->setTextWrap(true);
        spr->setTextColor(color565(237, 232, 230));
        spr->print("Pleaes reboot for settings to take effect.");
        spr->pushSprite(34, 167);
        spr->deleteSprite();

        lastindex = mindex;
    }

    void showWifiScanMenuResults(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        int16_t ilvl = (int16_t)props->menuLevelId;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;

        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder1px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_MAROON);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE);
            lcd->setTextSize(4);
            lcd->setCursor(55, 24);
            lcd->print("Networks");
            lcd->setTextSize(3);
        }

        TFT_eSprite *spr = gui->getSpriteBuffer(0);

        // print networks

        std::vector<String> results = am->getNetVars()->scanResults;
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);

        if (results.size() == 0)
        {
            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // push to screen
            spr->pushSprite(xi, yi + (offset * 0), 0xffff);
            spr->deleteSprite();
            // refresh sprite...
            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            gui->printIcon(spr, 0, 1, MENU_MAIN_WIFI, true);
            // push to screen
            spr->pushSprite(xi, yi + (offset * 0), 0xffff);
            spr->deleteSprite();
            // refresh sprite...
            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextColor(MENU_MAIN_WIFI == mindex ? TFT_CYAN : TFT_WHITE);
            spr->setTextSize(3);
            spr->setCursor(0, 5);
            spr->print(gui->getMenuName(MENU_MAIN_WIFI));
            spr->pushSprite(xt, yt + (offset * 0));
            spr->deleteSprite();

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextColor(TFT_YELLOW);
            spr->setTextSize(2);
            spr->setCursor(0, 5);
            spr->print("No Results!");
            spr->pushSprite(xt, yt + (offset * 1));
            spr->deleteSprite();
            lastindex = mindex;
            return;
        }

        for (int i = 0; i < 5; i++)
        {
            if (i > results.size() - ilvl + 2) // ignore entries beyond result size
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            if (i >= items.size()) // ignore entries beyond item size
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            if (items[i] == MENU_MAIN_WIFI)
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(30, 30);
                spr->fillRect(0, 0, 30, 30, bgColor);
                // push to screen
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                // refresh sprite...
                spr->createSprite(30, 30);
                spr->fillRect(0, 0, 30, 30, bgColor);
                gui->printIcon(spr, 0, 1, MENU_MAIN_WIFI, true);
                // push to screen
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();

                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setTextSize(3);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            if (items[i] - 1000 < 0)
            {
                continue;
            }

            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            // push to screen
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();
            // refresh sprite...
            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            gui->printIcon(spr, 0, 1, MENU_WIFI_NETWORK_SCAN, true);
            // push to screen
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();
            // refresh sprite...
            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(2);
            spr->setTextWrap(false);
            // print text
            spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE, bgColor);
            spr->setCursor(0, 8);
            spr->print(results[items[i] - 1000] ? results[items[i] - 1000] : "null");
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        // end print networks

        lastindex = mindex;
    }

    void showWiFiPassMenu(AerManager *am, bool update)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(42, 24);
        lcd->print("WiFi Pass");
        lcd->setTextSize(3);
        printSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showUSBMenu(AerManager *am, bool update, bool change)
    {
        unsigned int bcolor = TFT_DARKGREY;
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bcolor);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_PURPLE);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bcolor);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print("USB Menu");
        lcd->setTextSize(3);

        CommStor *comms = am->getComms();

        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        bool bSelect = false;
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < items.size(); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bcolor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bcolor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bcolor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_USB_ENABLE)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bcolor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(3);
                spr->print(gui->getMenuName(items[i]));
                if (comms->getUSBEn())
                {
                    spr->setTextColor(TFT_GREEN, bcolor);
                    spr->print(" On ");
                }
                else
                {
                    spr->setTextColor(TFT_RED, bcolor);
                    spr->print(" Off");
                }
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_USB_MONITOR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bcolor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(3);
                spr->print(gui->getMenuName(items[i]));
                if (comms->getUSBMon())
                {
                    spr->setTextColor(TFT_GREEN, bcolor);
                    spr->print(" On ");
                }
                else
                {
                    spr->setTextColor(TFT_RED, bcolor);
                    spr->print(" Off");
                }
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_USB_DISCOVER)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bcolor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(3);
                spr->print(gui->getMenuName(items[i]));
                if (comms->getUSBDis())
                {
                    spr->setTextColor(TFT_GREEN, bcolor);
                    spr->print(" On ");
                }
                else
                {
                    spr->setTextColor(TFT_RED, bcolor);
                    spr->print(" Off");
                }
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bcolor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bcolor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bcolor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showBleMenu(AerManager *am, bool update, bool change)
    {
        unsigned int bcolor = TFT_DARKGREY;
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bcolor);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_PURPLE);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bcolor);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print("BLE Menu");
        lcd->setTextSize(3);

        CommStor *comms = am->getComms();

        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        bool bSelect = false;
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < items.size(); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bcolor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bcolor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bcolor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_BLE_ENABLE)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bcolor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(3);
                spr->print(gui->getMenuName(items[i]));
                if (comms->getBTEn())
                {
                    spr->setTextColor(TFT_GREEN, bcolor);
                    spr->print(" On ");
                }
                else
                {
                    spr->setTextColor(TFT_RED, bcolor);
                    spr->print(" Off");
                }
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_BLE_MONITOR)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bcolor);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE);
                spr->setCursor(0, 2);
                spr->setTextSize(3);
                spr->print(gui->getMenuName(items[i]));
                if (comms->getBTMon())
                {
                    spr->setTextColor(TFT_GREEN, bcolor);
                    spr->print(" On ");
                }
                else
                {
                    spr->setTextColor(TFT_RED, bcolor);
                    spr->print(" Off");
                }
                spr->pushSprite(xt, yt + (offset * i) + 4);
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bcolor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bcolor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bcolor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        lastindex = mindex;
    }

    void showKeyboardMenu(AerManager *am, AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        FavsStor *favsObj = am->getFavs();
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
        }
        switch (mlvl)
        {
        case MENU_FAV_1: /* Fav1 Name */
        {
            lcd->print("Favorite #1");
            lcd->fillRect(40, 55, 240, 50, TFT_BLACK);
            lcd->fillRect(42, 57, 236, 46, TFT_DARKGREY);
            lcd->setCursor(55, 67);
            lcd->setTextColor(0xC69D, TFT_DARKGREY);
            lcd->print(favsObj->getName(1));
            lcd->setTextSize(4);
            lcd->fillRect(135, 110, 50, 50, TFT_BLACK);
            lcd->fillRect(137, 112, 46, 46, TFT_DARKGREY);
            lcd->setCursor(150, 117);
            lcd->print('a');
            lcd->setCursor(100, 117);
            lcd->print('.');
            lcd->setCursor(200, 117);
            lcd->print('b');
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            break;
        }
        case MENU_FAV_2: /* Fav2 Name */
        {
            lcd->print("Favorite #2");
            lcd->fillRect(40, 55, 240, 50, TFT_BLACK);
            lcd->fillRect(42, 57, 236, 46, TFT_DARKGREY);
            lcd->setCursor(55, 67);
            lcd->setTextColor(0xC69D, TFT_DARKGREY);
            lcd->print(favsObj->getName(2));
            lcd->setTextSize(4);
            lcd->fillRect(135, 110, 50, 50, TFT_BLACK);
            lcd->fillRect(137, 112, 46, 46, TFT_DARKGREY);
            lcd->setCursor(150, 117);
            lcd->print('a');
            lcd->setCursor(100, 117);
            lcd->print('.');
            lcd->setCursor(200, 117);
            lcd->print('b');
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            break;
        }
        case MENU_FAV_3: /* Fav3 Name */
        {
            lcd->print("Favorite #3");
            lcd->fillRect(40, 55, 240, 50, TFT_BLACK);
            lcd->fillRect(42, 57, 236, 46, TFT_DARKGREY);
            lcd->setCursor(55, 67);
            lcd->setTextColor(0xC69D, TFT_DARKGREY);
            lcd->print(favsObj->getName(3));
            lcd->setTextSize(4);
            lcd->fillRect(135, 110, 50, 50, TFT_BLACK);
            lcd->fillRect(137, 112, 46, 46, TFT_DARKGREY);
            lcd->setCursor(150, 117);
            lcd->print('a');
            lcd->setCursor(100, 117);
            lcd->print('.');
            lcd->setCursor(200, 117);
            lcd->print('b');
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            break;
        }
        case MENU_FAV_4: /* Fav4 Name */
        {
            lcd->print("Favorite #4");
            lcd->fillRect(40, 55, 240, 50, TFT_BLACK);
            lcd->fillRect(42, 57, 236, 46, TFT_DARKGREY);
            lcd->setCursor(55, 67);
            lcd->setTextColor(0xC69D, TFT_DARKGREY);
            lcd->print(favsObj->getName(4));
            lcd->setTextSize(4);
            lcd->fillRect(135, 110, 50, 50, TFT_BLACK);
            lcd->fillRect(137, 112, 46, 46, TFT_DARKGREY);
            lcd->setCursor(150, 117);
            lcd->print('a');
            lcd->setCursor(100, 117);
            lcd->print('.');
            lcd->setCursor(200, 117);
            lcd->print('b');
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            break;
        }
        case MENU_WIFI_PASSWORD: /* code */
        {
            TFT_eSprite *spr = gui->getSpriteBuffer(0);

            spr->createSprite(250, 30);
            spr->fillRect(0, 0, 250, 30, TFT_DARKGREY);
            spr->setTextWrap(false);
            spr->setTextColor(TFT_WHITE, TFT_DARKGREY);
            spr->setTextSize(4);
            spr->setCursor(15, 0);
            spr->print("PASSWORD");
            spr->pushSprite(45, 24);
            spr->deleteSprite();

            if (update && change)
            {
                lcd->fillScreen(0x0841);
                spr->createSprite(250, 76);
                spr->fillRect(0, 0, 250, 76, TFT_BLACK);
                spr->fillRect(2, 2, 246, 70, 0x5aeb);
                spr->pushSprite(35, 64);
                spr->deleteSprite();
            }

            spr->createSprite(236, 60);
            spr->fillRect(0, 0, 236, 60, 0x5aeb);
            spr->setTextSize(3);
            spr->setTextColor(0xC69D);
            spr->setTextWrap(true);
            if (update && change)
            {
                gui->getMenuProps()->menuItemSelStr = std::string(commstor.getPSK());
            }
            spr->print(gui->getMenuProps()->menuItemSelStr.c_str());
            spr->pushSprite(45, 74);
            spr->deleteSprite();

            spr->createSprite(250, 46);
            spr->setTextWrap(false);
            spr->fillRect(0, 0, 250, 46, TFT_BLACK);
            spr->fillRect(2, 2, 246, 42, 0x5aeb);
            printSelectedChar(gui, spr, 1);
            spr->pushSprite(35, 140);
            spr->deleteSprite();

            spr->createSprite(250, 20);
            spr->fillRect(0, 0, 250, 20, TFT_DARKGREY);
            spr->setTextSize(2);
            spr->setCursor(0, 0);
            bool mod = gui->isCursorModify();
            spr->setTextColor(mod ? TFT_LIGHTGREY : (mindex == MENU_MAIN_WIFI ? TFT_CYAN : TFT_WHITE));
            spr->print("Back");
            spr->setCursor(70, 0);
            spr->setTextColor(mindex == MENU_WIFI_HOSTNAME_EDIT ? mod ? TFT_RED : TFT_CYAN : TFT_WHITE);
            spr->print("Edit");
            spr->setCursor(180, 0);
            spr->setTextColor(mod ? TFT_LIGHTGREY : (mindex == MENU_WIFI_HOSTNAME_SAVE ? TFT_RED : TFT_WHITE));
            spr->print("Save");
            spr->pushSprite(40, 197);
            spr->deleteSprite();
            break;
        }
        case MENU_WIFI_HOSTNAME:
        {
            TFT_eSprite *spr = gui->getSpriteBuffer(0);

            spr->createSprite(250, 30);
            spr->fillRect(0, 0, 250, 30, TFT_DARKGREY);
            spr->setTextWrap(false);
            spr->setTextColor(TFT_WHITE, TFT_DARKGREY);
            spr->setTextSize(4);
            spr->setCursor(20, 0);
            spr->print("HOSTNAME");
            spr->pushSprite(45, 24);
            spr->deleteSprite();

            if (update && change)
            {
                lcd->fillScreen(0x0841);
                spr->createSprite(250, 76);
                spr->fillRect(0, 0, 250, 76, TFT_BLACK);
                spr->fillRect(2, 2, 246, 70, 0x5aeb);
                spr->pushSprite(35, 64);
                spr->deleteSprite();
            }

            spr->createSprite(236, 60);
            spr->fillRect(0, 0, 236, 60, 0x5aeb);
            spr->setTextSize(3);
            spr->setTextColor(0xC69D);
            spr->setTextWrap(true);
            if (update && change)
            {
                gui->getMenuProps()->menuItemSelStr = std::string(am->getNetVars()->hostname);
            }
            spr->print(gui->getMenuProps()->menuItemSelStr.c_str());
            spr->pushSprite(45, 74);
            spr->deleteSprite();

            spr->createSprite(250, 46);
            spr->setTextWrap(false);
            spr->fillRect(0, 0, 250, 46, TFT_BLACK);
            spr->fillRect(2, 2, 246, 42, 0x5aeb);
            printSelectedChar(gui, spr);
            spr->pushSprite(35, 140);
            spr->deleteSprite();

            spr->createSprite(250, 20);
            spr->fillRect(0, 0, 250, 20, TFT_DARKGREY);
            spr->setTextSize(2);
            spr->setCursor(0, 0);
            bool mod = gui->isCursorModify();
            spr->setTextColor(mod ? TFT_LIGHTGREY : (mindex == MENU_MAIN_WIFI ? TFT_CYAN : TFT_WHITE));
            spr->print("Back");
            spr->setCursor(70, 0);
            spr->setTextColor(mindex == MENU_WIFI_HOSTNAME_EDIT ? mod ? TFT_RED : TFT_CYAN : TFT_WHITE);
            spr->print("Edit");
            spr->setCursor(180, 0);
            spr->setTextColor(mod ? TFT_LIGHTGREY : (mindex == MENU_WIFI_HOSTNAME_SAVE ? TFT_RED : TFT_WHITE));
            spr->print("Save");
            spr->pushSprite(40, 197);
            spr->deleteSprite();
            break;
        }

        default:
            break;
        }

        lastindex = mindex;
    }

    // -----------------------------------------------------------

    // print selected utf8 character as sprite
    void printSelectedChar(AerGUI *gui, TFT_eSprite *spr, uint type)
    {
        bool mod = gui->isCursorModify();
        int sid = (int)gui->getMenuProps()->menuItemId;
        spr->setCursor(10, 10);
        spr->setTextSize(3);

        spr->setTextColor(0x9d16);
        for (int j = std::max(-3, sid - 3); j < std::max(0, sid); j++)
        {
            if (j < 0)
            {
                spr->print("  ");
                continue;
            }
            char sel = gui->getCharFromIndex(j, type);
            spr->print(sel);
            spr->print(" ");
        }

        if (sid >= 0)
        {
            char sel = gui->getCharFromIndex(sid, type);
            spr->setTextColor(mod ? 0xdffd : 0x9d16);
            spr->print(sel);
        }

        int max = (26 * 2) + 10 + 2 + (type == 1 ? 21 : 0);
        spr->setTextColor(0x9d16);
        for (int j = std::max(0, sid + 1); j < std::max(4, sid + 4); j++)
        {
            if (j < 0)
            {
                continue;
            }
            if (j >= max)
            {
                spr->print("  ");
                continue;
            }
            if (j > 0)
            {
                spr->print(" ");
            }
            char sel = gui->getCharFromIndex(j, type);
            spr->print(sel);
        }

        if (sid <= 2)
        {
            spr->setTextSize(2);
            if (sid <= 0)
            {
                spr->setTextColor(sid == -3 && mod ? 0xdffd : 0x9d16);
                spr->setCursor(10, 16);
                spr->print("bck");
                spr->setTextColor(sid == -2 && mod ? 0xdffd : 0x9d16);
                spr->setCursor(52, 16);
                spr->print("clr");
                spr->setTextColor(sid == -1 && mod ? 0xdffd : 0x9d16);
                spr->setCursor(96, 16);
                spr->print("<");
            }
            else if (sid <= 1)
            {
                spr->setTextColor(0x9d16);
                spr->setCursor(10, 16);
                spr->print("clr");
                spr->setCursor(52, 16);
                spr->print("<");
            }
            else if (sid <= 2)
            {
                spr->setTextColor(0x9d16);
                spr->setCursor(10, 16);
                spr->print("<");
            }
        }
    }

    // -----------------------------------------------------------

    void showPowerMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(40, 24);
        lcd->print("Power Menu");
        lcd->setTextSize(3);
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showFactoryReset(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, TFT_DARKGREY, TFT_RED);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(32, 24);
            lcd->print("Flash Reset");
            lcd->setTextSize(3);
        }

        drawSelections(gui, menu, mindex, TFT_DARKGREY);

        TFT_eSprite *spr = gui->getSpriteBuffer(0);
        spr->createSprite(250, 75);
        spr->setTextWrap(true);
        spr->fillRect(0, 0, 250, 75, TFT_DARKGREY);
        spr->setTextColor(0xf945, TFT_DARKGREY);
        spr->setTextSize(2);
        spr->setCursor(30, 0);
        spr->print("DANGER!  WARNING!");
        spr->setTextColor(0xfb28, TFT_DARKGREY);
        spr->setCursor(3, 20);
        spr->setTextSize(2);
        spr->print("This will erase ALL setup data from internal Flash storage!!");
        spr->pushSprite(35, 130);
        spr->deleteSprite();

        lastindex = mindex;
    }

    void showDisplayMenu(AerManager *am, AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        BumpStor *bumpObj = am->getBump();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            // only draw full background and border when needed
            drawRoundRectWithBorder1px(lcd, 20, 20, 280, 200, 7, TFT_DARKGREY, TFT_BLACK);
            // lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            // lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(55, 24);
        lcd->print("Display");

        std::vector<uint16_t> items;
        bool bSelect = false;
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        uint16_t bgColor = TFT_DARKGREY;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                // fill empty at bottom
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }

            if (!update && !change)
            {
                // don't update everything if no need...
                continue;
            }

            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(34, 30);
            // fill icon background...
            spr->fillRect(0, 0, 34, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            // push to screen
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_DISPLAY_SYS_TRAY)
            {
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
                spr->print(": ");

                if (am->getSettings()->getDispTypeOptions() == 0)
                {
                    spr->setTextColor(TFT_GREEN, TFT_DARKGREY);
                    spr->print("CPU");
                }
                else if (am->getSettings()->getDispTypeOptions() == 1)
                {
                    spr->setTextColor(TFT_GREEN, TFT_DARKGREY);
                    spr->print("INF");
                }
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        lastindex = mindex;
    }

    void showNearbyNetworks(AerGUI *gui, bool update, String json)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(40, 24);
        lcd->print("Nearby Networks");

        lcd->setTextSize(3);
        printSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showSystemTemperature(AerManager *am, AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t bgColor = TFT_DARKGREY;
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            // draw background...
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_MAROON);
            // lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor);
            // lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_MAROON);
            // lcd->drawRoundRect(19, 19, 282, 202, 7, TFT_MAROON);

            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setTextSize(4);
            lcd->setCursor(64, 24);
            lcd->print("Thermals");
        }

        std::vector<uint16_t> items = getSelectionItems(menu, mindex);

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        uint8_t offset = 30;
        lcd->setTextSize(3);
        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                // print empties at bottom
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }

            if (update)
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(34, 30);
                // fill icon background...
                spr->fillRect(0, 0, 34, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();

                spr->createSprite(30, 30);
                spr->fillRect(0, 0, 30, 30, bgColor);
                // print icon
                if (items[i] == menu.getChildren()[0])
                {
                    gui->printIcon(spr, 0, 1, items[i], true);
                }
                else
                {
                    gui->printIcon(spr, 0, 1, items[i], false);
                }
                // push to screen
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
            }

            if (items[i] == MENU_LOCAL_CELSIUS_TOGGLE)
            {
                // units toggle
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextSize(3);
                spr->setTextWrap(false);

                spr->setCursor(0, 5);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE, bgColor);
                spr->print(gui->getMenuName(items[i]));

                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_YELLOW, TFT_DARKGREY);
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    spr->print(" *F");
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    spr->print(" *C");
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    spr->print(" *K");
                }
                else
                {
                    spr->print(" *?");
                }
                // push to screen
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_LOCAL_TEMPERATURE_1)
            {
                // sys temperature measure 1 - cpu
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextSize(3);
                spr->setTextWrap(false);

                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE, TFT_DARKGREY);
                spr->setCursor(0, 5);

#if AERPID_COUNT == 1
                spr->print("CPU: ");
#endif
#if AERPID_COUNT == 2
                spr->print("CPU:  ");
#endif
                float lt = am->getLocalTempB();
                spr->setTextSize(2);
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    spr->print(paddedStr(toStrUnitF(lt, 2, true), 8).c_str());
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    spr->print(paddedStr(toStrUnitC(lt), 8).c_str());
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    spr->print(paddedStr(toStrUnitK(lt, 1, true), 8).c_str());
                }
                else
                {
                    spr->print(" ?");
                }
                // push to screen
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_LOCAL_TEMPERATURE_2)
            {
                // sys temperature measure 2 - ssr
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextSize(3);
                spr->setTextWrap(false);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE, TFT_DARKGREY);
                spr->setCursor(0, 5);

#if AERPID_COUNT == 1
                spr->print("SSR: ");
#endif
#if AERPID_COUNT == 2
                spr->print("SSR1: ");
#endif
                float lt = am->getLocalTempA();
                spr->setTextSize(2);
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    spr->print(paddedStr(toStrUnitF(lt, 2, true), 8).c_str());
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    spr->print(paddedStr(toStrUnitC(lt), 8).c_str());
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    spr->print(paddedStr(toStrUnitK(lt, 1, true), 8).c_str());
                }
                else
                {
                    spr->print(" ?");
                }
                // push to screen
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }
#if AERPID_COUNT == 2
            else if (items[i] == MENU_LOCAL_TEMPERATURE_3)
            {
                // sys temperature measure 2 - ssr
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265 - 30, 30);
                spr->fillRect(0, 0, 265 - 30, 30, bgColor);
                spr->setTextSize(3);
                spr->setTextWrap(false);
                spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE, TFT_DARKGREY);
                spr->setCursor(0, 5);

                spr->print("SSR2: ");
                float lt = am->getLocalTempC();
                spr->setTextSize(2);
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    spr->print(paddedStr(toStrUnitF(lt, 2, true), 8).c_str());
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    spr->print(paddedStr(toStrUnitC(lt), 8).c_str());
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    spr->print(paddedStr(toStrUnitK(lt, 1, true), 8).c_str());
                }
                else
                {
                    spr->print(" ?");
                }
                // push to screen
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }
#endif

            if (!update && !change)
            {
                continue;
            }

            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            spr->setTextColor(items[i] == mindex ? TFT_CYAN : TFT_WHITE, bgColor);
            spr->setCursor(0, 5);
            spr->print(gui->getMenuName(items[i]));
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        lastindex = mindex;
    }

    void showMainClockMenu(AerManager *am, AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        uint16_t bgColor = TFT_DARKGREY;
        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        if (update && change)
        {
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setTextSize(4);
            lcd->setCursor(50, 24);
            lcd->print("RTC Setup");
        }

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        uint8_t offset = 30;
        for (int i = 0; i < 5; i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }

            if (items[i] == MENU_RTC_CURRENT)
            {
                // Display current time on screen
                DateTimeRTC t = am->getDateTime();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(270, 30);
                spr->fillRect(0, 0, 270, 30, bgColor);
                spr->setCursor(16, 7);
                spr->setTextSize(2);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                spr->print("20");
                spr->print(t.year);
                spr->print("-");
                printDigit(spr, t.month);
                spr->print("-");
                printDigit(spr, t.day);
                spr->print(" ");
                printDigit(spr, t.hour);
                spr->print(":");
                printDigit(spr, t.minute);
                spr->print(":");
                printDigit(spr, t.second);
                spr->pushSprite(xi, yi + (offset * i));
                spr->deleteSprite();
                continue;
            }
            if (!update && !change)
            {
                continue;
            }

            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(34, 30);
            // fill icon background...
            spr->fillRect(0, 0, 34, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            // push to screen
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }
        lastindex = mindex;
    }

    void showDateEditMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print("Date Setup");
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showDateEditingMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update /*&& change*/)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        std::vector<uint16_t> items;
        bool bSelect = false;

        int year = 2022;
        std::vector<int> years;
        years.push_back(year++);
        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
                if (i > 0)
                {
                    years.push_back(year);
                    year++;
                }
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
                if (i > 0)
                {
                    years.push_back(year);
                    year++;
                    if (years.size() > 2)
                    {
                        years.erase(years.begin());
                    }
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (i > 0)
                {
                    years.push_back(year);
                    if (years.size() >= 5)
                    {
                        // break;
                    }
                    year++;
                }
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print(gui->getMenuName(mlvl));

        uint8_t offset = 30;
        for (int i = 0; i < items.size(); i++)
        {
            lcd->setTextSize(3);
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setCursor(32, 65 + (offset * i));

            gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], false);

            // lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == mindex)
            {
                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                if (mlvl == MENU_RTC_SET_DATE_YEAR)
                {
                    if (items[i] == MENU_RTC_DATE_SET)
                    {
                        lcd->print(gui->getMenuName(items[i]));
                    }
                    else
                    {
                        lcd->print(years[i]);
                    }
                }
                else
                {
                    lcd->print(gui->getMenuName(items[i]));
                }
            }
            else
            {
                lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                if (mlvl == MENU_RTC_SET_DATE_YEAR)
                {
                    if (items[i] == MENU_RTC_DATE_SET)
                    {
                        lcd->print(gui->getMenuName(items[i]));
                    }
                    else
                    {
                        lcd->print(years[i]);
                    }
                }
                else
                {
                    lcd->print(gui->getMenuName(items[i]));
                }
            }
        }
        lastindex = mindex;
    }

    void showTimeEditMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print("Time Setup");
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showTimeEditingMenu(AerGUI *gui, bool update, bool change)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(50, 24);
        lcd->print(gui->getMenuName(mlvl));
        drawSelections(gui, menu, mindex, TFT_DARKGREY);
        lastindex = mindex;
    }

    void showDefaultMenu(AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        if (update)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, TFT_DARKGREY);
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_BLACK);
        }

        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(40, 24);
        lcd->print(gui->getMenuName(mlvl));
        uint8_t offset = 30;
        lcd->setTextSize(3);
        for (int i = 0; i < items.size(); i++)
        {
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], true);
            }
            else
            {
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], false);
            }

            // lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == mindex)
            {
                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
            else
            {
                lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
        }
        lastindex = mindex;
    }

    void showTimeOutMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        BumpStor *bumpObj = am->getBump();
        TFT_eSPI *lcd = gui->getTFT();
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);

        String timeout = convertBumpSeconds(bumpObj->getTimeout() / 10);

        if (lastindex == mindex && !update)
        {
            if (lastTimeoutTime != bumpObj->getTimeout())
            {
                lcd->setCursor(70, 90);
                lcd->setTextSize(4);
                lcd->print(timeout);
                lastTimeoutTime = bumpObj->getTimeout();
            }
            // Indexes match and update is false; return
            return;
        }

        std::vector<uint16_t> items;
        bool bSelect = false;
        // uint8_t offset = 30;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextSize(4);
        lcd->setCursor(33, 30);
        lcd->print("Screen Timeout");
        lcd->setTextSize(4);
        lcd->setCursor(70, 100);
        lcd->print(timeout);
        lcd->setTextSize(2);
        lcd->setCursor(87, 130);
        lcd->print("hr : min : sec");
        lcd->setTextSize(4);
        lastTimeoutTime = bumpObj->getTimeout();

        lastindex = mindex;
    }

    int convertMenuLvlToFav(int menuindex)
    {
        switch (menuindex)
        {
        case MENU_FAV_1:
        case MENU_FAV_1N:
        case MENU_FAV_1T:
            return 1;
        case MENU_FAV_2:
        case MENU_FAV_2N:
        case MENU_FAV_2T:
            return 2;
        case MENU_FAV_3:
        case MENU_FAV_3N:
        case MENU_FAV_3T:
            return 3;
        case MENU_FAV_4:
        case MENU_FAV_4N:
        case MENU_FAV_4T:
            return 4;
        default:
            break;
        }
        return 0;
    }

    String convertBumpSeconds(double seconds)
    {
        String timeout = "";
        int hr = (int)(seconds / 3600);
        int min = ((int)(seconds / 60)) % 60;
        int sec = (int)(seconds) % 60;
        if (hr < 10)
        {
            timeout += "0" + String(hr) + ":";
        }
        else
        {
            timeout += String(hr) + ":";
        }
        if (min < 10)
        {
            timeout += "0" + String(min) + ":";
        }
        else
        {
            timeout += String(min) + ":";
        }
        if (sec < 10)
        {
            timeout += "0" + String(sec);
        }
        else
        {
            timeout += String(sec);
        }
        return timeout;
    }

    void showKnobMenu(AerManager *am, AerGUI *gui, bool update)
    {
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(56, 24);
        lcd->print("Knob Ctrl");

        uint8_t offset = 30;
        lcd->setTextSize(3);
        for (int i = 0; i < items.size(); i++)
        {
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setCursor(32, 65 + (offset * i));

            gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], false);

            if (items[i] == MENU_KNOB_ADJUST_AMOUNT)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
                lcd->print(" ");
                if (items[i] != mindex)
                {
                    lcd->setTextColor(TFT_YELLOW, TFT_DARKGREY);
                }
                if (gui->getCursorModify() == MENU_KNOB_ADJUST_AMOUNT)
                {
                    lcd->setTextColor(TFT_RED, TFT_DARKGREY);
                }
                lcd->print(am->getAerPID(0)->TEMP_ADJ_AMOUNT, 1);
                continue;
            }
            else if (items[i] == MENU_KNOB_ADJUST_INVERT)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
                lcd->print(" ");
                if (items[i] != mindex)
                {
                    lcd->setTextColor(TFT_YELLOW, TFT_DARKGREY);
                }
                lcd->print(am->getSettings()->getKnobInvert() ? "YES" : "NO");
                continue;
            }
            else if (items[i] == MENU_KNOB_DIAGRAM_SHOWN)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
                lcd->print(" ");
                if (items[i] != mindex)
                {
                    lcd->setTextColor(TFT_YELLOW, TFT_DARKGREY);
                }
                lcd->print(am->getSettings()->getKnobShowUI() ? "SHOW" : "HIDE");
                continue;
            }

            // lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == mindex)
            {
                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
            else
            {
                lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
        }
        lastindex = mindex;
    }

    void showScreenSaverMenu(AerManager *am, bool update)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();

        std::vector<uint16_t> items;
        bool bSelect = false;

        for (int i = 0; i < menu.getChildren().size(); i++)
        {
            uint16_t child = menu.getChildren()[i];
            if (child == mindex)
            {
                bSelect = true;
                items.push_back(child);
            }
            else if (!bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() > 2)
                {
                    items.erase(items.begin());
                }
            }
            else if (bSelect && child != mindex)
            {
                items.push_back(child);
                if (items.size() >= 5)
                {
                    break;
                }
            }
        }

        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
        lcd->setTextSize(4);
        lcd->setCursor(56, 24);
        lcd->print(gui->getMenuName(mlvl));

        uint8_t offset = 30;
        lcd->setTextSize(3);
        for (int i = 0; i < items.size(); i++)
        {
            lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
            lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == menu.getChildren()[0])
            {
                // lcd->drawRect(32 - 2, 65 + (offset * i) - 2, 32 - 4, 32 - 4, TFT_GREEN);
                // lcd->fillRect(32 - 1, 65 + (offset * i) - 1, 32 - 6, 32 - 6, TFT_YELLOW);
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], true);
            }
            else
            {
                // lcd->drawRect(32 - 2, 65 + (offset * i) - 2, 32 - 4, 32 - 4, TFT_LIGHTGREY);
                // lcd->fillRect(32 - 1, 65 + (offset * i) - 1, 32 - 6, 32 - 6, TFT_GREEN);
                gui->printIcon(32 - 2, 65 + (offset * i) - 2, items[i], false);
            }

            if (items[i] == MENU_SCREENSAVER_ENABLED)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
                lcd->print(": ");

                if (am->getSettings()->getScreenSaverEnb())
                {
                    lcd->setTextColor(TFT_GREEN, TFT_DARKGREY);
                    lcd->print("On");
                }
                else
                {
                    lcd->setTextColor(TFT_RED, TFT_DARKGREY);
                    lcd->print("Off");
                }
                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                continue;
            }

            if (items[i] == MENU_SCREENSAVER_TIME)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
                lcd->print(": ");

                if (gui->getCursorModify() == MENU_SCREENSAVER_TIME)
                {
                    lcd->setTextColor(TFT_RED, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_GREEN, TFT_DARKGREY);
                }
                float tLen = am->getSettings()->getTimeTillScrnSaverOn() / 1000 / 60.f;
                lcd->print(tLen, 1);
                lcd->print(" mn");

                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                continue;
            }

            else if (items[i] == MENU_SCREENSAVER_STARS)
            {
                if (items[i] == mindex)
                {
                    lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                }
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
                lcd->print(": ");

                if (gui->getCursorModify() == MENU_SCREENSAVER_STARS)
                {
                    lcd->setTextColor(TFT_RED, TFT_DARKGREY);
                }
                else
                {
                    lcd->setTextColor(TFT_GREEN, TFT_DARKGREY);
                }
                lcd->print(am->getSettings()->getScrnSaverStarCount());

                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                continue;
            }
            else if (items[i] == MENU_SCREENSAVER_TOGGLE)
            {
            }

            // lcd->setCursor(32, 65 + (offset * i));
            if (items[i] == mindex)
            {
                lcd->setTextColor(TFT_CYAN, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
            else
            {
                lcd->setTextColor(TFT_WHITE, TFT_DARKGREY);
                // lcd->print(items[i]);
                lcd->setCursor(64, 65 + (offset * i));
                lcd->print(gui->getMenuName(items[i]));
            }
        }
        lastindex = mindex;
    }

    void showIdleToggle(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_GREY);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setTextSize(4);
            lcd->setCursor(32, 24);
            lcd->print("Idle Toggle");
            lcd->setTextSize(3);
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < std::min(5, (int)menu.getChildren().size()); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_SYSTEM_TOGGLE_IDLE_VAR)
            {
                uint8_t elementIndex = gui->getElementIndex();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(230, 30);
                spr->fillRect(0, 0, 230, 30, bgColor);
                spr->setCursor(3, 7);
                spr->setTextSize(3);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, bgColor);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, bgColor);
                }
                spr->print("Auto-Off: ");
                spr->setTextSize(2);
                gui->isCursorModify() ? spr->setTextColor(TFT_RED, bgColor) : spr->setTextColor(TFT_GREEN, bgColor);
                spr->print(am->getAerPID(elementIndex)->AUTO_OFF_ENB ? "ON" : "OFF");
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }

        lastindex = mindex;
    }
    void showIdleLength(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_GREY);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setTextSize(4);
            lcd->setCursor(32, 24);
            lcd->print("Idle Length");
            lcd->setTextSize(3);
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < std::min(5, (int)menu.getChildren().size()); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_SYSTEM_LENGTH_IDLE_VAR)
            {
                uint8_t elementIndex = gui->getElementIndex();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(230, 30);
                spr->fillRect(0, 0, 230, 30, bgColor);
                spr->setCursor(3, 7);
                spr->setTextSize(2);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, bgColor);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, bgColor);
                }
                spr->print("Idle Timeout: ");
                spr->setTextSize(2);
                gui->isCursorModify() ? spr->setTextColor(TFT_RED, bgColor) : spr->setTextColor(TFT_GREEN, bgColor);
                float aot = ((float)am->getAerPID(elementIndex)->AUTO_OFF_TIME / 60 * 100) / 100;
                spr->print(aot);
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showBacklight(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_GREY);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setTextSize(4);
            lcd->setCursor(46, 24);
            lcd->print("Backlight");
            lcd->setTextSize(3);
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < std::min(5, (int)menu.getChildren().size()); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_DISPLAY_BACKLIGHT_VAR)
            {
                uint8_t elementIndex = gui->getElementIndex();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(230, 30);
                spr->fillRect(0, 0, 230, 30, bgColor);
                spr->setCursor(3, 7);
                spr->setTextSize(2);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, bgColor);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, bgColor);
                }
                spr->print("Brightness Max: ");
                spr->setTextSize(2);
                gui->getCursorModify() == MENU_DISPLAY_BACKLIGHT_VAR ? spr->setTextColor(TFT_RED, bgColor) : spr->setTextColor(TFT_GREEN, bgColor);
                uint bll = ((float)gui->getST7789()->getBacklightLevel() / 255) * 100;
                spr->print(bll);
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }
            else if (items[i] == MENU_DISPLAY_BACKLIGHT_MIN_VAR)
            {
                uint8_t elementIndex = gui->getElementIndex();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(230, 30);
                spr->fillRect(0, 0, 230, 30, bgColor);
                spr->setCursor(3, 7);
                spr->setTextSize(2);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, bgColor);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, bgColor);
                }
                spr->print("Brightness Min: ");
                spr->setTextSize(2);
                gui->getCursorModify() == MENU_DISPLAY_BACKLIGHT_MIN_VAR ? spr->setTextColor(TFT_RED, bgColor) : spr->setTextColor(TFT_GREEN, bgColor);
                uint bll = ((float)gui->getST7789()->getBacklightLevelMin() / 255) * 100;
                spr->print(bll);
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showBacklightDim(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_GREY);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setTextSize(4);
            lcd->setCursor(46, 24);
            lcd->print("Auto Dim");
            lcd->setTextSize(3);
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < std::min(5, (int)menu.getChildren().size()); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_DISPLAY_AUTO_DIM_VAR)
            {
                uint8_t elementIndex = gui->getElementIndex();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(230, 30);
                spr->fillRect(0, 0, 230, 30, bgColor);
                spr->setCursor(0, 7);
                spr->setTextSize(2);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, bgColor);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, bgColor);
                }
                spr->print("Enabled: ");
                spr->setTextSize(2);
                gui->isCursorModify() ? spr->setTextColor(TFT_RED, bgColor) : spr->setTextColor(TFT_GREEN, bgColor);
                spr->print(gui->getST7789()->isBacklightDimOn() ? "YES" : "NO");
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showBacklightDimTimeout(AerManager *am, bool update, bool change)
    {
        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        uint16_t bgColor = TFT_DARKGREY;
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            drawRoundRectWithBorder2px(lcd, 20, 20, 280, 200, 7, bgColor, TFT_GREY);
            lcd->setTextWrap(false);
            lcd->setTextColor(TFT_WHITE, bgColor);
            lcd->setTextSize(4);
            lcd->setCursor(32, 24);
            lcd->print("Dim Timeout");
            lcd->setTextSize(3);
        }

        // printSelections
        std::vector<uint16_t> items = getSelectionItems(menu, mindex);
        uint8_t offset = 30;

        uint xi = 30;
        uint yi = 63;

        uint xt = 64;
        uint yt = 63;

        for (int i = 0; i < std::min(5, (int)menu.getChildren().size()); i++)
        {
            if (i >= items.size())
            {
                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(265, 30);
                spr->fillRect(0, 0, 265, 30, bgColor);
                spr->pushSprite(xi, yi + (offset * i), 0xffff);
                spr->deleteSprite();
                continue;
            }
            TFT_eSprite *spr = gui->getSpriteBuffer(0);
            spr->createSprite(30, 30);
            // fill icon background...
            spr->fillRect(0, 0, 30, 30, bgColor);
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            spr->createSprite(30, 30);
            spr->fillRect(0, 0, 30, 30, bgColor);
            // print icon
            if (items[i] == menu.getChildren()[0])
            {
                gui->printIcon(spr, 0, 1, items[i], true);
            }
            else
            {
                gui->printIcon(spr, 0, 1, items[i], false);
            }
            spr->pushSprite(xi, yi + (offset * i), 0xffff);
            spr->deleteSprite();

            if (items[i] == MENU_DISPLAY_TIMEOUT_DIM_VAR)
            {
                uint8_t elementIndex = gui->getElementIndex();

                TFT_eSprite *spr = gui->getSpriteBuffer(0);
                spr->createSprite(230, 30);
                spr->fillRect(0, 0, 230, 30, bgColor);
                spr->setCursor(0, 7);
                spr->setTextSize(2);
                if (items[i] == mindex)
                {
                    spr->setTextColor(TFT_CYAN, bgColor);
                }
                else
                {
                    spr->setTextColor(TFT_WHITE, bgColor);
                }
                spr->print("Minutes: ");
                spr->setTextSize(2);
                gui->isCursorModify() ? spr->setTextColor(TFT_RED, bgColor) : spr->setTextColor(TFT_GREEN, bgColor);
                float bldt = ((float)gui->getST7789()->getBacklightDimTimeout() / 60 * 100) / 100;
                spr->print(bldt);
                spr->pushSprite(xt, yt + (offset * i));
                spr->deleteSprite();
                continue;
            }

            spr->createSprite(265 - 30, 30);
            spr->fillRect(0, 0, 265 - 30, 30, bgColor);
            spr->setTextSize(3);
            spr->setTextWrap(false);
            // print text
            if (items[i] == mindex)
            {
                spr->setTextColor(TFT_CYAN, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            else
            {
                spr->setTextColor(TFT_WHITE, bgColor);
                spr->setCursor(0, 5);
                spr->print(gui->getMenuName(items[i]));
            }
            // push to screen
            spr->pushSprite(xt, yt + (offset * i));
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    // =======================================================================

    // charting vars
    double l_yhi = 0;
    double l_ylo = 0;

    bool display1 = true;
    bool update1 = true;

    void showGraphTemperature(AerManager *am, bool update, bool change, uint8_t elementIndex)
    {
        const uint16_t bgColor = color565(2, 5, 3);

        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor); // TFT_DARKGREY
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);

            graph = new AerChart();
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bgColor);
        lcd->setTextSize(2);
        lcd->setCursor(64, 22);
        lcd->print("Temperature Graph");
        lcd->setTextSize(1);

        // size of datapoints array...
        uint data_size = 64;

        /* datapoint variables x,y */
        double x, y1, y2 = 0;

        uint8_t dp = 1;   // datapoint flag
        double gx = 60;   // x graph location (lower left)
        double gy = 198;  // y graph location (lower left)
        double w = 220;   // width of graph
        double h = 160;   // height of graph
        double xlo = 0;   // lower bound of x axis
        double xhi = 64;  // upper bound of x asis
        double xinc = 16; // division of x axis (distance not count)
        double ylo = 0;   // lower bound of y axis
        double yhi = 500; // upper bound of y asis
        double yinc = 50; // division of y axis (distance not count)

        // labels are not used!
        const char *title = "T(x)";   // title of graph
        const char *xlabel = "x";     // x asis label
        const char *ylabel = "fn(x)"; // y asis label

        uint16_t seriesColor = color565(8, 255, 32);

        // rescale the y axis based on the measured
        yhi = am->getAerPID(elementIndex)->maxMeasures() + 15;
        ylo = am->getAerPID(elementIndex)->minMeasuresLong() - 25;

        if (ylo < 100)
        {
            ylo = 0;
        }

        // rescale increment for y axis
        if (yhi < 50)
        {
            yinc = 5;
        }
        else if (yhi < 75)
        {
            yinc = 7.5;
        }
        else if (yhi < 125)
        {
            yinc = 10;
        }
        else if (yhi < 200)
        {
            yinc = 20;
            yhi += 20;
        }
        else if (yhi < 250)
        {
            yinc = 30;
            yhi += 25;
        }
        else if (yhi < 300)
        {
            yinc = 40;
            yhi += 30;
        }
        else if (yhi < 400)
        {
            yinc = 50;
            yhi += 35;
        }

        // changed flag for redrawing axises
        bool chngd = l_yhi != yhi;
        l_yhi = yhi;

        // adjust based on reading type
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            // ylo = 30;
            yhi = toFahrenheit(yhi);
            yinc = toFahrenheit(yinc);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            yhi = toKelvin(yhi);
            yinc = toKelvin(yinc);
        }

        // draw the basic graph canvas
        display1 = update || change || chngd;
        graph->Graph(gui, x, y1, dp, gx - 1, gy, w + 5, h, xlo, xhi, xinc, ylo, yhi, yinc, "", "", "", display1, seriesColor, bgColor);

        // sprite buffer
        TFT_eSprite *spr = gui->getSpriteBuffer(1);

        // chunked sprite count
        const uint chnk = 8;

        uint t_x;              // index pointer for x datapoint
        double t_xlo = 0;      // lower bound of x axis
        double t_xhi = 8;      // upper bound of x axis
        double t_xinc = 4;     // division of x axis
        double t_w = w / chnk; // graph chunk width
        double t_gx = 0;       // x graph location (lower left)
        double t_gy = h;       // y graph location (lower left)

        // get the measured data to display
        double *mes = am->getAerPID(elementIndex)->getMeasures();
        double sett = am->getAerPID(elementIndex)->SET_TEMP;

        // iterate over sprite data, build chart series
        for (int z = 0; z < 8; z++)
        {
            spr->createSprite(28, h);
            spr->fillRect(0, 0, 28, h, bgColor);
            update1 = true;

            for (x = 0; x <= chnk; x++)
            {
                t_x = x + chnk * z;
                if (t_x >= data_size)
                {
                    break;
                }
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    y1 = toFahrenheit(mes[t_x]);
                    y2 = toFahrenheit(sett);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    y1 = mes[t_x];
                    y2 = sett;
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    y1 = toKelvin(mes[t_x]);
                    y2 = toKelvin(sett);
                }
                graph->Trace(spr,
                             x /* datapoint x */,
                             y1 /* datapoint y1 */,
                             y2 /* datapoint y2 */,
                             dp,
                             t_gx /* location x */,
                             t_gy /* location y */,
                             t_w /* graph width */,
                             h /* graph height */,
                             t_xlo,
                             t_xhi,
                             t_xinc,
                             ylo,
                             yhi,
                             yinc,
                             title,
                             xlabel,
                             ylabel,
                             update1,
                             seriesColor,
                             bgColor);
            }

            spr->pushSprite(gx + (28 * z), gy - h);
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showGraphTemperatureZoom(AerManager *am, bool update, bool change, uint8_t elementIndex)
    {
        uint16_t bgColor = color565(2, 5, 3);

        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor); // TFT_DARKGREY
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);

            graph = new AerChart();
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bgColor);
        lcd->setTextSize(2);
        lcd->setCursor(64, 22);
        lcd->print("Temp Graph Zoom");
        lcd->setTextSize(1);

        // size of datapoints array...
        uint data_size = 64;

        /* datapoint variables x,y */
        double x, y1, y2 = 0;

        uint8_t dp = 1;     // datapoint flag
        double gx = 60;     // x graph location (lower left)
        double gy = 198;    // y graph location (lower left)
        double w = 220;     // width of graph
        double h = 160;     // height of graph
        double xlo = 0;     // lower bound of x axis
        double xhi = 64;    // upper bound of x asis
        double xinc = 16;   // division of x axis (distance not count)
        double ylo = 0;     // lower bound of y axis
        double yhi = 500;   // upper bound of y asis
        double yinc = 0.05; // division of y axis (distance not count)

        // labels are not used!
        const char *title = "T(x)";   // title of graph
        const char *xlabel = "x";     // x asis label
        const char *ylabel = "fn(x)"; // y asis label

        uint16_t seriesColor = color565(8, 255, 32);

        double avg = am->getAerPID(elementIndex)->avgMeasuresLong();
        double dev = am->getAerPID(elementIndex)->maxMeasuresLong() - max(am->getAerPID(elementIndex)->minMeasuresLong(), avg - 10);
        // rescale the y axis based on the measured
        yhi = am->getAerPID(elementIndex)->maxMeasuresLong() + dev;
        ylo = am->getAerPID(elementIndex)->minMeasuresLong() - dev;
        yhi = min(yhi, avg + dev);
        ylo = max(ylo, avg - dev);

        if (dev > 300)
        {
            yinc = 10;
        }
        else if (dev > 200)
        {
            yinc = 8;
        }
        else if (dev > 120)
        {
            yinc = 4;
        }
        else if (dev > 90)
        {
            yinc = 3;
        }
        else if (dev > 70)
        {
            yinc = 1;
        }
        else if (dev > 40)
        {
            yinc = 0.5;
        }
        else if (dev > 20)
        {
            yinc = 0.2;
        }
        else if (dev > 10)
        {
            yinc = 0.1;
        }

        // changed flag for redrawing axises
        bool chngd = l_yhi != yhi || l_ylo != ylo;
        l_yhi = yhi;
        l_ylo = ylo;

        // adjust based on reading type
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            yhi = toFahrenheit(yhi);
            yinc = toFahrenheit(yinc);
            ylo = toFahrenheit(ylo);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            yhi = toKelvin(yhi);
            yinc = toKelvin(yinc);
            ylo = toKelvin(ylo);
        }

        // draw the basic graph canvas
        display1 = update || change || chngd;
        graph->Graph(gui, x, y1, dp, gx - 1, gy, w + 5, h, xlo, xhi, xinc, ylo, yhi, yinc, "", "", "", display1, seriesColor, bgColor);

        // sprite buffer
        TFT_eSprite *spr = gui->getSpriteBuffer(1);

        // chunked sprite count
        const uint chnk = 8;

        uint t_x;              // index pointer for x datapoint
        double t_xlo = 0;      // lower bound of x axis
        double t_xhi = 8;      // upper bound of x axis
        double t_xinc = 4;     // division of x axis
        double t_w = w / chnk; // graph chunk width
        double t_gx = 0;       // x graph location (lower left)
        double t_gy = h;       // y graph location (lower left)

        // get the measured data to display
        double *mes = am->getAerPID(elementIndex)->getMeasures();
        double sett = am->getAerPID(elementIndex)->SET_TEMP;

        // iterate over sprite data, build chart series
        for (int z = 0; z < 8; z++)
        {
            spr->createSprite(28, h);
            spr->fillRect(0, 0, 28, h, bgColor);
            update1 = true;

            for (x = 0; x <= chnk; x++)
            {
                t_x = x + chnk * z;
                if (t_x >= data_size)
                {
                    break;
                }
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    y1 = toFahrenheit(mes[t_x]);
                    y2 = toFahrenheit(sett);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    y1 = mes[t_x];
                    y2 = sett;
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    y1 = toKelvin(mes[t_x]);
                    y2 = toKelvin(sett);
                }
                graph->Trace(spr,
                             x /* datapoint x */,
                             y1 /* datapoint y1 */,
                             y2 /* datapoint y2 */,
                             dp,
                             t_gx /* location x */,
                             t_gy /* location y */,
                             t_w /* graph width */,
                             h /* graph height */,
                             t_xlo,
                             t_xhi,
                             t_xinc,
                             ylo,
                             yhi,
                             yinc,
                             title,
                             xlabel,
                             ylabel,
                             update1,
                             seriesColor,
                             bgColor);
            }

            spr->pushSprite(gx + (28 * z), gy - h);
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showGraphTemperatureLong(AerManager *am, bool update, bool change, uint8_t elementIndex)
    {
        uint16_t bgColor = color565(2, 5, 3);

        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor); // TFT_DARKGREY
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);

            graph = new AerChart();
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bgColor);
        lcd->setTextSize(2);
        lcd->setCursor(64, 22);
        lcd->print("Temp Graph 3 Sec");
        lcd->setTextSize(1);

        // size of datapoints array...
        uint data_size = 64;

        /* datapoint variables x,y */
        double x, y1, y2 = 0;

        uint8_t dp = 1;   // datapoint flag
        double gx = 60;   // x graph location (lower left)
        double gy = 198;  // y graph location (lower left)
        double w = 220;   // width of graph
        double h = 160;   // height of graph
        double xlo = 0;   // lower bound of x axis
        double xhi = 128; // upper bound of x asis
        double xinc = 16; // division of x axis (distance not count)
        double ylo = 0;   // lower bound of y axis
        double yhi = 500; // upper bound of y asis
        double yinc = 50; // division of y axis (distance not count)

        // labels are not used!
        const char *title = "T(x)";   // title of graph
        const char *xlabel = "x";     // x asis label
        const char *ylabel = "fn(x)"; // y asis label

        uint16_t seriesColor = color565(8, 255, 32);

        // rescale the y axis based on the measured
        yhi = am->getAerPID(elementIndex)->maxMeasures() + 25;
        ylo = am->getAerPID(elementIndex)->minMeasuresLong() - 25;

        if (ylo < 100)
        {
            ylo = 0;
        }

        // rescale increment for y axis
        if (yhi < 50)
        {
            yinc = 5;
        }
        else if (yhi < 75)
        {
            yinc = 7.5;
        }
        else if (yhi < 125)
        {
            yinc = 10;
        }
        else if (yhi < 200)
        {
            yinc = 20;
            yhi += 20;
        }
        else if (yhi < 250)
        {
            yinc = 30;
            yhi += 25;
        }
        else if (yhi < 300)
        {
            yinc = 40;
            yhi += 30;
        }
        else if (yhi < 400)
        {
            yinc = 50;
            yhi += 35;
        }

        // changed flag for redrawing axises
        bool chngd = l_yhi != yhi;
        l_yhi = yhi;

        // adjust based on reading type
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            // ylo = 30;
            yhi = toFahrenheit(yhi);
            yinc = toFahrenheit(yinc);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            yhi = toKelvin(yhi);
            yinc = toKelvin(yinc);
        }

        // draw the basic graph canvas
        display1 = update || change || chngd;
        graph->Graph(gui, x, y1, dp, gx - 1, gy, w + 5, h, xlo, xhi, xinc, ylo, yhi, yinc, "", "", "", display1, seriesColor, bgColor);

        // sprite buffer
        TFT_eSprite *spr = gui->getSpriteBuffer(1);

        // chunked sprite count
        const uint chnk = 8;

        uint t_x;           // index pointer for x datapoint
        double t_xlo = 0;   // lower bound of x axis
        double t_xhi = 8;   // upper bound of x axis
        double t_xinc = 4;  // division of x axis
        double t_w = w / 8; // graph chunk width
        double t_gx = 0;    // x graph location (lower left)
        double t_gy = h;    // y graph location (lower left)

        // get the measured data to display
        double *mes = am->getAerPID(elementIndex)->getMeasuresLong();
        double sett = am->getAerPID(elementIndex)->SET_TEMP;

        // iterate over sprite data, build chart series
        for (int z = 0; z < 8; z++)
        {
            spr->createSprite(28, h);
            spr->fillRect(0, 0, 28, h, bgColor);
            update1 = true;

            for (x = 0; x <= 8; x++)
            {
                t_x = x + chnk * z;
                if (t_x >= data_size)
                {
                    break;
                }
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    y1 = toFahrenheit(mes[t_x]);
                    y2 = toFahrenheit(sett);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    y1 = mes[t_x];
                    y2 = sett;
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    y1 = toKelvin(mes[t_x]);
                    y2 = toKelvin(sett);
                }
                graph->Trace(spr,
                             x /* datapoint x */,
                             y1 /* datapoint y1 */,
                             y2 /* datapoint y2 */,
                             dp,
                             t_gx /* location x */,
                             t_gy /* location y */,
                             t_w /* graph width */,
                             h /* graph height */,
                             t_xlo,
                             t_xhi,
                             t_xinc,
                             ylo,
                             yhi,
                             yinc,
                             title,
                             xlabel,
                             ylabel,
                             update1,
                             seriesColor,
                             bgColor);
            }

            spr->pushSprite(gx + ((28) * z), gy - h);
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showGraphTemperature(AerManager *am, bool update, bool change)
    {
        const uint16_t bgColor = color565(2, 5, 3);

        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor); // TFT_DARKGREY
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);

            graph = new AerChart();
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bgColor);
        lcd->setTextSize(2);
        lcd->setCursor(64, 22);
        lcd->print("Temperature Graph");
        lcd->setTextSize(1);

        // size of datapoints array...
        uint data_size = 64;

        /* datapoint variables x,y */
        double x, y1, y2, y3, y4 = 0;

        uint8_t dp = 1;   // datapoint flag
        double gx = 60;   // x graph location (lower left)
        double gy = 198;  // y graph location (lower left)
        double w = 220;   // width of graph
        double h = 160;   // height of graph
        double xlo = 0;   // lower bound of x axis
        double xhi = 64;  // upper bound of x asis
        double xinc = 16; // division of x axis (distance not count)
        double ylo = 0;   // lower bound of y axis
        double yhi = 500; // upper bound of y asis
        double yinc = 50; // division of y axis (distance not count)

        // labels are not used!
        const char *title = "T(x)";   // title of graph
        const char *xlabel = "x";     // x asis label
        const char *ylabel = "fn(x)"; // y asis label

        // rescale the y axis based on the measured
        yhi = max(am->getAerPID(0)->maxMeasures() + 15, am->getAerPID(1)->maxMeasures() + 15);
        ylo = min(am->getAerPID(0)->minMeasuresLong() - 25, am->getAerPID(1)->minMeasuresLong() - 25);

        if (ylo < 100)
        {
            ylo = 0;
        }

        // rescale increment for y axis
        if (yhi < 50)
        {
            yinc = 5;
        }
        else if (yhi < 75)
        {
            yinc = 7.5;
        }
        else if (yhi < 125)
        {
            yinc = 10;
        }
        else if (yhi < 200)
        {
            yinc = 20;
            yhi += 20;
        }
        else if (yhi < 250)
        {
            yinc = 30;
            yhi += 25;
        }
        else if (yhi < 300)
        {
            yinc = 40;
            yhi += 30;
        }
        else if (yhi < 400)
        {
            yinc = 50;
            yhi += 35;
        }

        // changed flag for redrawing axises
        bool chngd = l_yhi != yhi;
        l_yhi = yhi;

        // adjust based on reading type
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            // ylo = 30;
            yhi = toFahrenheit(yhi);
            yinc = toFahrenheit(yinc);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            yhi = toKelvin(yhi);
            yinc = toKelvin(yinc);
        }

        // draw the basic graph canvas
        display1 = update || change || chngd;
        graph->Graph(gui, x, y1, dp, gx - 1, gy, w + 5, h, xlo, xhi, xinc, ylo, yhi, yinc, "", "", "", display1, 0x0000, bgColor);

        // sprite buffer
        TFT_eSprite *spr = gui->getSpriteBuffer(1);

        // chunked sprite count
        const uint chnk = 8;

        uint t_x;              // index pointer for x datapoint
        double t_xlo = 0;      // lower bound of x axis
        double t_xhi = 8;      // upper bound of x axis
        double t_xinc = 4;     // division of x axis
        double t_w = w / chnk; // graph chunk width
        double t_gx = 0;       // x graph location (lower left)
        double t_gy = h;       // y graph location (lower left)

        // get the measured data to display
        double *mes1 = am->getAerPID(0)->getMeasures();
        double *mes2 = am->getAerPID(1)->getMeasures();
        double sett1 = am->getAerPID(0)->SET_TEMP;
        double sett2 = am->getAerPID(1)->SET_TEMP;

        // iterate over sprite data, build chart series
        for (int z = 0; z < 8; z++)
        {
            spr->createSprite(28, h);
            spr->fillRect(0, 0, 28, h, bgColor);
            update1 = true;

            for (x = 0; x <= chnk; x++)
            {
                t_x = x + chnk * z;
                if (t_x >= data_size)
                {
                    break;
                }
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    y1 = toFahrenheit(mes1[t_x]);
                    y2 = toFahrenheit(sett1);
                    y3 = toFahrenheit(mes2[t_x]);
                    y4 = toFahrenheit(sett2);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    y1 = mes1[t_x];
                    y2 = sett1;
                    y3 = mes2[t_x];
                    y4 = sett2;
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    y1 = toKelvin(mes1[t_x]);
                    y2 = toKelvin(sett1);
                    y3 = toKelvin(mes2[t_x]);
                    y4 = toKelvin(sett2);
                }
                graph->Trace(spr,
                             x /* datapoint x */,
                             y1 /* datapoint y1 */,
                             y2 /* datapoint y2 */,
                             y3 /* datapoint y3 */,
                             y4 /* datapoint y4 */,
                             dp,
                             t_gx /* location x */,
                             t_gy /* location y */,
                             t_w /* graph width */,
                             h /* graph height */,
                             t_xlo,
                             t_xhi,
                             t_xinc,
                             ylo,
                             yhi,
                             yinc,
                             title,
                             xlabel,
                             ylabel,
                             update1,
                             bgColor);
            }

            spr->pushSprite(gx + (28 * z), gy - h);
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showGraphTemperatureZoom(AerManager *am, bool update, bool change)
    {
        uint16_t bgColor = color565(2, 5, 3);

        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor); // TFT_DARKGREY
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);

            graph = new AerChart();
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bgColor);
        lcd->setTextSize(2);
        lcd->setCursor(64, 22);
        lcd->print("Temp Graph Zoom");
        lcd->setTextSize(1);

        // size of datapoints array...
        uint data_size = 64;

        /* datapoint variables x,y */
        double x, y1, y2, y3, y4 = 0;

        uint8_t dp = 1;     // datapoint flag
        double gx = 60;     // x graph location (lower left)
        double gy = 198;    // y graph location (lower left)
        double w = 220;     // width of graph
        double h = 160;     // height of graph
        double xlo = 0;     // lower bound of x axis
        double xhi = 64;    // upper bound of x asis
        double xinc = 16;   // division of x axis (distance not count)
        double ylo = 0;     // lower bound of y axis
        double yhi = 500;   // upper bound of y asis
        double yinc = 0.05; // division of y axis (distance not count)

        // labels are not used!
        const char *title = "T(x)";   // title of graph
        const char *xlabel = "x";     // x asis label
        const char *ylabel = "fn(x)"; // y asis label

        uint16_t seriesColor = color565(8, 255, 32);

        double avg1 = am->getAerPID(0)->avgMeasuresLong();
        double avg2 = am->getAerPID(1)->avgMeasuresLong();
        double dev1 = am->getAerPID(0)->maxMeasuresLong() - max(am->getAerPID(0)->minMeasuresLong(), avg1 - 10);
        double dev2 = am->getAerPID(1)->maxMeasuresLong() - max(am->getAerPID(1)->minMeasuresLong(), avg2 - 10);
        double dev = max(dev1, dev2);
        // rescale the y axis based on the measured
        yhi = max(am->getAerPID(0)->maxMeasuresLong() + dev1, am->getAerPID(1)->maxMeasuresLong() + dev2);
        ylo = max(am->getAerPID(0)->minMeasuresLong() - dev1, am->getAerPID(1)->minMeasuresLong() - dev2);
        yhi = min(yhi, max(avg1 + dev1, avg2 + dev2)) + 1;
        ylo = max(ylo, min(avg1 - dev1, avg2 - dev2)) - 1;

        if (dev > 300)
        {
            yinc = 10;
        }
        else if (dev > 200)
        {
            yinc = 8;
        }
        else if (dev > 120)
        {
            yinc = 4;
        }
        else if (dev > 90)
        {
            yinc = 3;
        }
        else if (dev > 70)
        {
            yinc = 1;
        }
        else if (dev > 40)
        {
            yinc = 0.5;
        }
        else if (dev > 20)
        {
            yinc = 0.2;
        }
        else if (dev > 10)
        {
            yinc = 0.1;
        }

        // changed flag for redrawing axises
        bool chngd = l_yhi != yhi || l_ylo != ylo;
        l_yhi = yhi;
        l_ylo = ylo;

        // adjust based on reading type
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            yhi = toFahrenheit(yhi);
            yinc = toFahrenheit(yinc);
            ylo = toFahrenheit(ylo);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            yhi = toKelvin(yhi);
            yinc = toKelvin(yinc);
            ylo = toKelvin(ylo);
        }

        // draw the basic graph canvas
        display1 = update || change || chngd;
        graph->Graph(gui, x, y1, dp, gx - 1, gy, w + 5, h, xlo, xhi, xinc, ylo, yhi, yinc, "", "", "", display1, seriesColor, bgColor);

        // sprite buffer
        TFT_eSprite *spr = gui->getSpriteBuffer(1);

        // chunked sprite count
        const uint chnk = 8;

        uint t_x;              // index pointer for x datapoint
        double t_xlo = 0;      // lower bound of x axis
        double t_xhi = 8;      // upper bound of x axis
        double t_xinc = 4;     // division of x axis
        double t_w = w / chnk; // graph chunk width
        double t_gx = 0;       // x graph location (lower left)
        double t_gy = h;       // y graph location (lower left)

        // get the measured data to display
        double *mes1 = am->getAerPID(0)->getMeasures();
        double *mes2 = am->getAerPID(1)->getMeasures();
        double sett1 = am->getAerPID(0)->SET_TEMP;
        double sett2 = am->getAerPID(1)->SET_TEMP;

        // iterate over sprite data, build chart series
        for (int z = 0; z < 8; z++)
        {
            spr->createSprite(28, h);
            spr->fillRect(0, 0, 28, h, bgColor);
            update1 = true;

            for (x = 0; x <= chnk; x++)
            {
                t_x = x + chnk * z;
                if (t_x >= data_size)
                {
                    break;
                }
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    y1 = toFahrenheit(mes1[t_x]);
                    y2 = toFahrenheit(sett1);
                    y3 = toFahrenheit(mes2[t_x]);
                    y4 = toFahrenheit(sett2);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    y1 = mes1[t_x];
                    y2 = sett1;
                    y3 = mes2[t_x];
                    y4 = sett2;
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    y1 = toKelvin(mes1[t_x]);
                    y2 = toKelvin(sett1);
                    y3 = toKelvin(mes2[t_x]);
                    y4 = toKelvin(sett2);
                }
                graph->Trace(spr,
                             x /* datapoint x */,
                             y1 /* datapoint y1 */,
                             y2 /* datapoint y2 */,
                             y3 /* datapoint y3 */,
                             y4 /* datapoint y4 */,
                             dp,
                             t_gx /* location x */,
                             t_gy /* location y */,
                             t_w /* graph width */,
                             h /* graph height */,
                             t_xlo,
                             t_xhi,
                             t_xinc,
                             ylo,
                             yhi,
                             yinc,
                             title,
                             xlabel,
                             ylabel,
                             update1,
                             bgColor);
            }

            spr->pushSprite(gx + (28 * z), gy - h);
            spr->deleteSprite();
        }

        lastindex = mindex;
    }

    void showGraphTemperatureLong(AerManager *am, bool update, bool change)
    {
        uint16_t bgColor = color565(2, 5, 3);

        AerGUI *gui = am->getGUI();
        PropsMenu *props = gui->getMenuProps();
        uint16_t mindex = props->menuLevelVal;
        if (lastindex == mindex && !update)
        {
            // Indexes match and update is false; return
            // return;
        }
        uint16_t mlvl = props->menuIndex;
        AerMenu menu = gui->getSelectedMenu(mlvl);
        TFT_eSPI *lcd = gui->getTFT();
        if (update && change)
        {
            lcd->fillScreen(0x0841);
            lcd->fillRoundRect(20, 20, 280, 200, 7, bgColor); // TFT_DARKGREY
            lcd->drawRoundRect(18, 18, 284, 204, 7, TFT_CYAN);

            graph = new AerChart();
        }
        lcd->setTextWrap(false);
        lcd->setTextColor(TFT_WHITE, bgColor);
        lcd->setTextSize(2);
        lcd->setCursor(64, 22);
        lcd->print("Temp Graph 3 Sec");
        lcd->setTextSize(1);

        // size of datapoints array...
        uint data_size = 64;

        /* datapoint variables x,y */
        double x, y1, y2, y3, y4 = 0;

        uint8_t dp = 1;   // datapoint flag
        double gx = 60;   // x graph location (lower left)
        double gy = 198;  // y graph location (lower left)
        double w = 220;   // width of graph
        double h = 160;   // height of graph
        double xlo = 0;   // lower bound of x axis
        double xhi = 128; // upper bound of x asis
        double xinc = 16; // division of x axis (distance not count)
        double ylo = 0;   // lower bound of y axis
        double yhi = 500; // upper bound of y asis
        double yinc = 50; // division of y axis (distance not count)

        // labels are not used!
        const char *title = "T(x)";   // title of graph
        const char *xlabel = "x";     // x asis label
        const char *ylabel = "fn(x)"; // y asis label

        uint16_t seriesColor = color565(8, 255, 32);

        // rescale the y axis based on the measured
        yhi = max(am->getAerPID(0)->maxMeasures() + 25, am->getAerPID(1)->maxMeasures() + 25);
        ylo = min(am->getAerPID(0)->minMeasuresLong() - 25, am->getAerPID(1)->minMeasuresLong() - 25);

        if (ylo < 100)
        {
            ylo = 0;
        }

        // rescale increment for y axis
        if (yhi < 50)
        {
            yinc = 5;
        }
        else if (yhi < 75)
        {
            yinc = 7.5;
        }
        else if (yhi < 125)
        {
            yinc = 10;
        }
        else if (yhi < 200)
        {
            yinc = 20;
            yhi += 20;
        }
        else if (yhi < 250)
        {
            yinc = 30;
            yhi += 25;
        }
        else if (yhi < 300)
        {
            yinc = 40;
            yhi += 30;
        }
        else if (yhi < 400)
        {
            yinc = 50;
            yhi += 35;
        }

        // changed flag for redrawing axises
        bool chngd = l_yhi != yhi;
        l_yhi = yhi;

        // adjust based on reading type
        if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
        {
            // ylo = 30;
            yhi = toFahrenheit(yhi);
            yinc = toFahrenheit(yinc);
        }
        else if (am->getReadingType() == ThermalUnitsType::KELVIN)
        {
            yhi = toKelvin(yhi);
            yinc = toKelvin(yinc);
        }

        // draw the basic graph canvas
        display1 = update || change || chngd;
        graph->Graph(gui, x, y1, dp, gx - 1, gy, w + 5, h, xlo, xhi, xinc, ylo, yhi, yinc, "", "", "", display1, seriesColor, bgColor);

        // sprite buffer
        TFT_eSprite *spr = gui->getSpriteBuffer(1);

        // chunked sprite count
        const uint chnk = 8;

        uint t_x;           // index pointer for x datapoint
        double t_xlo = 0;   // lower bound of x axis
        double t_xhi = 8;   // upper bound of x axis
        double t_xinc = 4;  // division of x axis
        double t_w = w / 8; // graph chunk width
        double t_gx = 0;    // x graph location (lower left)
        double t_gy = h;    // y graph location (lower left)

        // get the measured data to display
        double *mes1 = am->getAerPID(0)->getMeasuresLong();
        double sett1 = am->getAerPID(0)->SET_TEMP;
        double *mes2 = am->getAerPID(1)->getMeasuresLong();
        double sett2 = am->getAerPID(1)->SET_TEMP;

        // iterate over sprite data, build chart series
        for (int z = 0; z < 8; z++)
        {
            spr->createSprite(28, h);
            spr->fillRect(0, 0, 28, h, bgColor);
            update1 = true;

            for (x = 0; x <= 8; x++)
            {
                t_x = x + chnk * z;
                if (t_x >= data_size)
                {
                    break;
                }
                if (am->getReadingType() == ThermalUnitsType::FAHRENHEIT)
                {
                    y1 = toFahrenheit(mes1[t_x]);
                    y2 = toFahrenheit(sett1);
                    y3 = toFahrenheit(mes2[t_x]);
                    y4 = toFahrenheit(sett2);
                }
                else if (am->getReadingType() == ThermalUnitsType::CELSIUS)
                {
                    y1 = mes1[t_x];
                    y2 = sett1;
                    y3 = mes2[t_x];
                    y4 = sett2;
                }
                else if (am->getReadingType() == ThermalUnitsType::KELVIN)
                {
                    y1 = toKelvin(mes1[t_x]);
                    y2 = toKelvin(sett1);
                    y3 = toKelvin(mes2[t_x]);
                    y4 = toKelvin(sett2);
                }
                graph->Trace(spr,
                             x /* datapoint x */,
                             y1 /* datapoint y1 */,
                             y2 /* datapoint y2 */,
                             y3 /* datapoint y3 */,
                             y4 /* datapoint y4 */,
                             dp,
                             t_gx /* location x */,
                             t_gy /* location y */,
                             t_w /* graph width */,
                             h /* graph height */,
                             t_xlo,
                             t_xhi,
                             t_xinc,
                             ylo,
                             yhi,
                             yinc,
                             title,
                             xlabel,
                             ylabel,
                             update1,
                             bgColor);
            }

            spr->pushSprite(gx + ((28) * z), gy - h);
            spr->deleteSprite();
        }

        lastindex = mindex;
    }
}