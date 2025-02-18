
#ifndef ST7789_V_H
#define ST7789_V_H

// User setup loaded
#ifndef USER_SETUP_LOADED

// Pins setup
#define TFT_RST 14
#define TFT_CS 27
#define TFT_DC 26
#define TFT_MOSI 23 // for hardware SPI data pin
#define TFT_SCLK 18 // for hardware SPI sclk pin
#define TFT_BL 16

#define ST7789_DRIVER
#define TFT_RGB_ORDER TFT_BGR

#endif // User setup loaded

// #define TFT_BL 16
// PWM channel for backlight
#define TFT_BL_PWM_CHAN 2
#define TFT_BL_PWM_FREQ 2000
#define TFT_BL_PWM_RES 8

// SPI setup
#define SPICONFIG SPISettings(52000000, MSBFIRST, SPI_MODE0)

#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library

#include "img/ad_icon.h"
#include "img/splash.h"
#include "img/logo.h"

#define TFT_GREY 0x5AEB

// tft screen object
static TFT_eSPI __tft = TFT_eSPI();

// lets pre-define five sprites for now.
static TFT_eSprite __screenbuffer[1] = {TFT_eSprite(&__tft)};

class AerST7789
{
public:
    AerST7789()
    {
        this->tft = &__tft;
        this->screenbuffer = &*__screenbuffer;

        if (Serial)
        {
            Serial.println("ST7789 Initialized");
        }
    }

    TFT_eSPI *getTFT()
    {
        return tft;
    }
    TFT_eSprite *getSpriteBuffer()
    {
        return &screenbuffer[0];
    }
    TFT_eSprite *getSpriteBuffer(uint i)
    {
        return &screenbuffer[i];
    }

    void setupISP();

    void showVersion(String v);

    void showSplashInit();
    void showSplashPost();
    void showLoadingStorMessage();
    void showLoadingAppMessage();

    // setup backlight PWM
    void initBacklight();

    // set backlight level
    void setBacklight(uint8_t v, bool apply = true)
    {
        backlight_level = v;
        if (apply)
        {
            ledcWrite(TFT_BL_PWM_CHAN, v);
        }
    }
    void setBacklight() { ledcWrite(TFT_BL_PWM_CHAN, backlight_level); }
    uint8_t getBacklightLevel() { return backlight_level; }

    void setBacklightMin(uint8_t v) { backlight_level_min = v; }
    uint8_t getBacklightLevelMin() { return backlight_level_min; }

    bool isBacklightDimOn() { return backlight_dim; }
    void setBacklightDim(bool on) { backlight_dim = on; }

    bool isBacklightDimmed() { return backlight_dimmed; }
    void setBacklightDimmed(bool dimmed) { backlight_dimmed = dimmed; }

    uint16_t getBacklightDimTimeout() { return backlight_dim_timeout; }
    void setBacklightDimTimeout(uint16_t seconds) { backlight_dim_timeout = seconds; }

private:
    TFT_eSPI *tft;
    TFT_eSprite *screenbuffer;

    bool backlight_dim = false;
    bool backlight_dimmed = false;
    uint8_t backlight_level = 0;
    uint8_t backlight_level_min = 128;
    uint16_t backlight_dim_timeout = 90;

    void testdrawtext(char *text, uint16_t color);
    void testdrawvers(char *text, uint16_t color);
};

#endif