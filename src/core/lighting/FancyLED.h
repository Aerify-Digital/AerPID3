#ifndef _AER_FANCY_LED_H_
#define _AER_FANCY_LED_H_

#include <Adafruit_GFX.h>
#include <FastLED.h>

#include <PinDefs.h>
#include <setup.h>

#include "core/controllers/AerManager.h"
#include "storage/lights.h"

// =======================

class AerManager;

class FancyLED
{
public:
    FancyLED(LightsStor *s) { this->lights = s; };

    uint init()
    {
        // ----------------------------
        // Setup the fancy LED
        FastLED.addLeds<CHIPSET, PIN_xLED_A, COLOR_ORDER>(xleds, NUM_LEDS);
        return NUM_LEDS;
    }

    CRGB *getPixels()
    {
        return xleds;
    }

    void tickFancyLED(AerManager *am, LightsStor *lights);

    void rainbow(CRGB *pixel);
    void rainbow();
    void rainbowWave();
    uint32_t Wheel(CRGB *pixel, byte WheelPos);
    void setPixel(CRGB &pixel, const String color);
    // void setPixels(CRGB *pixel, const String color);
    // void pulsePixel(CRGB &pixel, const String color, bool increment);
    void pulsePixels(const String color, bool increment);
    void pulseRGB(uint8_t r, uint8_t g, uint8_t b, bool increment);
    void brightenPixels(CRGB *pixel, uint8_t value);
    void brightenPixels(uint8_t value);
    void colorPixels(CRGB pixel);
    void colorPixel(CRGB pixel, uint32_t color);
    void disableAll();

    uint16_t setRGB(uint8_t r, uint8_t g, uint8_t b);
    void rgbWheel(byte WheelPos, uint8_t *r, uint8_t *g, uint8_t *b);

protected:
    friend class AerManager;

private:
    // CRGB *pixels;
    LightsStor *lights;

    // Initialize the fancy LED
    CRGB xleds[NUM_LEDS];

    // hue for color
    uint8_t hue = 0;
    // color wheel pos for rainbow
    int wheel_pos = 0;

    // LED task/tick Variables
    uint16_t led_tick = 0;
    uint8_t LIGHT_STYLE_CYCLE = 0;
    int pulse_t = 0;
    int pulse_dir = 0;

    // Lighting vars for control setting
    bool LEDstate = true;
    uint8_t LED_R = 255; // Used in the lighting thread for random colors
    uint8_t LED_G = 0;   // Used in the lighting thread for random colors
    uint8_t LED_B = 255; // Used in the lighting thread for random colors
};

extern FancyLED fancyLED;

#endif