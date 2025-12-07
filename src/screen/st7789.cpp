#include "st7789.h"

void AerST7789::setupISP()
{
    pinMode(TFT_CS, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, LOW);
    delay(2);

    tft->init();
    tft->setRotation(1);

    tft->fillScreen(TFT_BLACK);
    delay(50);
    testdrawtext((char *)"Starting AerPID Controls for ST7789V...", (uint16_t)TFT_BLUE);
    digitalWrite(TFT_BL, HIGH);
    delay(200);
}

void AerST7789::initBacklight()
{
    pinMode(TFT_BL, OUTPUT);
    double freq_PWN = ledcSetup(TFT_BL_PWM_CHAN, TFT_BL_PWM_FREQ, TFT_BL_PWM_RES);
    ledcAttachPin(TFT_BL, TFT_BL_PWM_CHAN);

    if (Serial)
    {
        Serial.print(F(">> "));
        Serial.print(F("Attached PWM control to Pin "));
        Serial.print(F("'"));
        Serial.print(TFT_BL);
        Serial.print(F("'"));
        Serial.print(F(" on Channel "));
        Serial.print(F("'"));
        Serial.print(TFT_BL_PWM_CHAN);
        Serial.print(F("'"));
        Serial.print(F(" at frequency of: "));
        Serial.print(freq_PWN, 0);
        Serial.print(F(" hz"));
        Serial.print(F("  with: "));
        Serial.print(TFT_BL_PWM_RES);
        Serial.print(F(" resolution bits"));
        Serial.println(F("!"));
    }

    if (backlight_level <= 0)
    {
        // set initial brightness...
        setBacklight(255, true);
        setBacklightMin(32);
    }
    else if (backlight_level <= 4)
    {
        setBacklight(255, true);
        setBacklightMin(12);
    }

    if (Serial)
    {
        Serial.print(F(">> Enabled TFT Backlight at "));
        Serial.print(((float)getBacklightLevel() / 255) * 100, 0);
        Serial.println(F("%"));
    }
}

void AerST7789::testdrawtext(char *text, uint16_t color)
{
    tft->setCursor(5, 5);
    tft->setTextColor(color);
    tft->setTextWrap(true);
    tft->print(text);
}

void AerST7789::testdrawvers(char *text, uint16_t color)
{
    tft->setTextColor(color);
    tft->print(text);
}

void AerST7789::showVersion(String version)
{
    String v = String(" v") + version;
    testdrawvers((char *)v.c_str(), TFT_GREEN);
}

void AerST7789::showLoadingStorMessage()
{
    tft->setTextColor(TFT_GREY, TFT_WHITE);
    tft->setTextSize(2);
    tft->setCursor(75, 152);
    tft->print("Loading Setup...");
    delay(300);
}

void AerST7789::showLoadingAppMessage()
{
    tft->setTextColor(TFT_GREY, TFT_WHITE);
    tft->setTextSize(2);
    tft->setCursor(75, 172);
    tft->print("Loading App...");
    delay(300);
}

void AerST7789::showSplashInit()
{
    tft->pushImage(0, 0, 320, 240, image_data_aeri_splash);
    delay(200);
}

void AerST7789::showSplashPost()
{
    tft->fillScreen(TFT_BLACK);
    tft->setTextColor(TFT_WHITE, TFT_BLACK);
    tft->setTextSize(3);
    int xt = (TFT_HEIGHT - 13 * 18) / 2, yt = (TFT_WIDTH - 24) / 2;
    tft->pushImage((TFT_HEIGHT - 64) / 2, yt - 72, 64, 64, image_data_aeri_logo);
    tft->setCursor(xt, yt);
    tft->println("AerifyDigital");

    delay(500); // animation delay

    tft->setTextColor(TFT_GREY, TFT_BLACK);
    xt = (TFT_HEIGHT - 8 * 12) / 2;
    yt = (TFT_WIDTH - 16) / 2;
    tft->setTextSize(2);
    tft->setCursor(xt, yt + 28);
    tft->print("(C)");
    const char *year = (const char *)VER_BUILD_YEAR;
    tft->println(year);

    delay(1000);
}