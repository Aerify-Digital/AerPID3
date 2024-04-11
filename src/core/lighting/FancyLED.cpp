#include "FancyLED.h"

// tick the fancy leds
void FancyLED::tickFancyLED(AerManager *am, LightsStor *lights)
{
    /*
     * bool LIGHT_STAT_EN = true;                 lights->statusEnabled()
     * bool LIGHT_AMBI_EN = true;                 lights->ambientEnabled()
     * bool LIGHT_PULSE_EN = false;               lights->amPulseEnabled()
     * bool LIGHT_BLINK_EN = false;               lights->amBlinkEnabled()
     * bool LIGHT_RAIN_EN = false;                lights->preRainEnabled()
     * bool LIGHT_COLPULSE_EN = false;            lights->prePulseEnabled()
     * bool LIGHT_COLSHIFT_EN = false;            lights->preShiftEnabled()
     */
    if (am->updateLightState())
    {
        if (!lights->isEnabled())
        {
            brightenPixels(xleds, 0);
            setPixel(xleds[0], "OFF");
        }
        else
        {
            brightenPixels(xleds, 90);
            xleds->setRGB(lights->getRgbVal('r'), lights->getRgbVal('g'), lights->getRgbVal('b'));
            FastLED.show();
        }
        am->updateLightState(false);
    }

    if (lights->isEnabled()) //(LEDstate)
    {

        if (lights->statusEnabled() && am->getAerPID(0)->PID_ON)
        {
            // Serial.println("Status!");
            if (led_tick > 1)
            {
                // Serial.println("Status in tick!");
                led_tick = 0;
                if (am->getAerPID(0)->AVG_TEMP < am->getAerPID(0)->SET_TEMP - 250)
                {
                    pulsePixels("BLUE", true);
                }
                else if (am->getAerPID(0)->AVG_TEMP < am->getAerPID(0)->SET_TEMP - 10)
                {
                    pulsePixels("YELLOW", true);
                }
                else if (am->getAerPID(0)->AVG_TEMP < am->getAerPID(0)->SET_TEMP - 3)
                {
                    pulsePixels("CYAN", true);
                }
                else if (am->getAerPID(0)->AVG_TEMP < am->getAerPID(0)->SET_TEMP + 3)
                {
                    pulsePixels("GREEN", true);
                }
                else if (am->getAerPID(0)->AVG_TEMP < am->getAerPID(0)->SET_TEMP + 6)
                {
                    pulsePixels("ORANGE", true);
                }
                else if (am->getAerPID(0)->AVG_TEMP >= am->getAerPID(0)->SET_TEMP + 6)
                {
                    pulsePixels("RED", true);
                }
            }
            else
            {
                led_tick++;
            }
        }
        else
        {
            //
            switch (lights->getMode())
            {
            case 1:
            { /* Ambient Blink */
                EVERY_N_SECONDS(5)
                {
                    if (led_tick >= 1)
                    {
                        led_tick = 0;
                        colorPixels(xleds->setRGB(lights->getRgbVal('r'), lights->getRgbVal('g'), lights->getRgbVal('b')));
                        brightenPixels(0);
                    }
                    else
                    {
                        led_tick = 1;
                        colorPixels(xleds->setRGB(lights->getRgbVal('r'), lights->getRgbVal('g'), lights->getRgbVal('b')));
                        // brightenPixels(255);
                        brightenPixels(lights->getBright());
                    }
                }
                break;
            }
            case 2:
            { /* Ambient Pulse */
                if (led_tick >= 1)
                {
                    pulsePixels("AMBIENT", true);
                    led_tick = 0;
                }
                else
                {
                    led_tick++;
                }
                break;
            }
            case 4:
            { /* Preset Rainbow2 */
                rainbow();
                EVERY_N_MILLISECONDS(36)
                { // You can change the pattern speed here
                    hue++;
                }
                brightenPixels(lights->getBright());
                break;
            }
            case 5:
            { /* Preset Rainbow3 */
                switch (LIGHT_STYLE_CYCLE)
                {
                case 0:
                    pulseRGB(148, 0, 211, true);
                    break;
                case 1:
                    pulseRGB(75, 0, 130, true);
                    break;
                case 2:
                    pulseRGB(0, 0, 255, true);
                    break;
                case 3:
                    pulseRGB(0, 255, 0, true);
                    break;
                case 4:
                    pulseRGB(255, 255, 0, true);
                    break;
                case 5:
                    pulseRGB(255, 127, 0, true);
                    break;
                case 6:
                    pulseRGB(255, 0, 0, true);
                    break;
                case 7:
                    /* code */
                    break;

                default:
                    break;
                }
                // led_tick and LIGHT_STYLE_CYCLE handling
                if (led_tick >= 108)
                {
                    pulse_t = 0;
                    pulse_dir = 0;
                    led_tick = 0;
                    if (LIGHT_STYLE_CYCLE < 6)
                    {
                        LIGHT_STYLE_CYCLE++;
                    }
                    else
                    {
                        LIGHT_STYLE_CYCLE = 0;
                    }
                }
                else
                {
                    led_tick++;
                }
                break;
            }
            case 6:
            { /* Preset Rainbow */
                rainbowWave();
                EVERY_N_MILLISECONDS(36) // You can change the pattern speed here  TODO: Make pattern_speed VAR
                {
                    hue++;
                }
                brightenPixels(lights->getBright());
                break;
            }
            case 7:
            { /* Preset Shift */
                colorPixels(xleds->setRGB(LED_R, LED_G, LED_B));
                brightenPixels(lights->getBright());
                if (led_tick >= 300)
                {
                    led_tick = 0;
                    LED_R = random(256);
                    LED_G = random(256);
                    LED_B = random(256);
                }
                else
                {
                    led_tick++;
                }
                break;
            }
            case 8:
            { /* Preset Pulse */
                pulseRGB(LED_R, LED_G, LED_B, true);
                // led_tick and LIGHT_STYLE_CYCLE handling
                if (led_tick >= 108)
                {
                    pulse_t = 0;
                    pulse_dir = 0;
                    led_tick = 0;
                    LED_R = random(256);
                    LED_G = random(256);
                    LED_B = random(256);
                }
                else
                {
                    led_tick++;
                }
                break;
            }
            case 9:
            default:
            {
                colorPixels(xleds->setRGB(lights->getRgbVal('r'), lights->getRgbVal('g'), lights->getRgbVal('b')));
                // brightenPixels(255);
                brightenPixels(lights->getBright());
                break;
            }
            }
        }
    }
}

// ========================================================
// Fancy LED Functions
// ========================================================

uint16_t FancyLED::setRGB(uint8_t r, uint8_t g, uint8_t b)
{
    return (r << 16 | g << 8 | b);
}

void FancyLED::rgbWheel(byte WheelPos, uint8_t *r, uint8_t *g, uint8_t *b)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        *r = 255 - WheelPos * 3;
        *g = 0;
        *b = WheelPos * 3;
        return;
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        *r = 0;
        *g = WheelPos * 3;
        *b = 255 - WheelPos * 3;
        return;
    }
    WheelPos -= 170;
    *r = WheelPos * 3;
    *g = 255 - WheelPos * 3;
    *b = 0;
}

void FancyLED::rainbow(CRGB *pixel)
{
    uint16_t i;

    // for (i = 0; i < pixel.numPixels(); i++)
    for (i = 0; i < NUM_LEDS; i++)
    {
        pixel[i] = Wheel(pixel, (i + wheel_pos) & 255);
        // pixel.setPixelColor(i, Wheel(pixel, (i + wheel_pos) & 255));
    }

    FastLED.show();
    // pixel.show();

    wheel_pos++;

    if (wheel_pos > 255)
    {
        wheel_pos = 0;
    }
}

void FancyLED::rainbow()
{
    for (int i = 0; i < NUM_LEDS; ++i)
    {
        xleds[i] = CHSV(hue, 255, 255);
    }
}

void FancyLED::rainbowWave()
{
    for (int i = 0; i < NUM_LEDS; ++i)
    {
        xleds[i] = CHSV(hue + (i * 10), 255, 255);
    }
}

void FancyLED::setPixel(CRGB &pixel, const String color)
{
    if (color == "OFF" || color == "BLACK")
    {
        colorPixel(pixel, setRGB(0, 0, 0));
    }
    if (color == "WHITE")
    {
        colorPixel(pixel, setRGB(255, 255, 255));
    }
    if (color == "RED")
    {
        colorPixel(pixel, setRGB(255, 0, 0));
    }
    if (color == "GREEN")
    {
        colorPixel(pixel, setRGB(0, 255, 0));
    }
    if (color == "BLUE")
    {
        colorPixel(pixel, setRGB(0, 0, 255));
    }
    if (color == "YELLOW")
    {
        colorPixel(pixel, setRGB(255, 246, 0));
    }
    if (color == "ORANGE")
    {
        colorPixel(pixel, setRGB(255, 141, 0));
    }
    if (color == "ORANGE_2")
    {
        colorPixel(pixel, setRGB(255, 80, 0));
    }
    if (color == "ORANGE_3")
    {
        colorPixel(pixel, setRGB(255, 208, 0));
    }
    if (color == "CYAN")
    {
        colorPixel(pixel, setRGB(0, 255, 255));
    }
    if (color == "MAGENTA")
    {
        colorPixel(pixel, setRGB(255, 0, 255));
    }
    if (color == "PURPLE")
    {
        colorPixel(pixel, setRGB(212, 0, 255));
    }
    if (color == "GREEN-YELLOW")
    {
        colorPixel(pixel, setRGB(225, 255, 0));
    }
    if (color == "GREENISH")
    {
        colorPixel(pixel, setRGB(225, 255, 16));
    }
}

void FancyLED::brightenPixels(CRGB *pixel, uint8_t value)
{
    // pixel.setBrightness(value);
    // pixel.show();
    FastLED.setBrightness(value);
    FastLED.show();
}

void FancyLED::brightenPixels(uint8_t value)
{
    // pixel.setBrightness(value);
    // pixel.show();
    FastLED.setBrightness(value);
    FastLED.show();
}

void FancyLED::colorPixels(CRGB pixel)
{
    uint16_t i;
    for (i = 0; i < NUM_LEDS; i++)
    {
        // pixel.setPixelColor(i, color);
        xleds[i] = pixel;
    }

    // pixel.show();
    FastLED.show();
}

void FancyLED::colorPixel(CRGB pixel, uint32_t color)
{
    pixel = pixel.setColorCode(color);
    // pixel.show();
    FastLED.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t FancyLED::Wheel(CRGB *pixel, byte WheelPos)
{
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85)
    {
        return setRGB(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170)
    {
        WheelPos -= 85;
        return setRGB(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return setRGB(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void FancyLED::pulsePixels(const String color, bool increment)
{
    if (color == "WHITE")
    {
        colorPixels(xleds->setRGB(255, 255, 255));
    }
    if (color == "RED")
    {
        colorPixels(xleds->setRGB(255, 0, 0));
    }
    if (color == "GREEN")
    {
        colorPixels(xleds->setRGB(0, 255, 0));
    }
    if (color == "BLUE")
    {
        colorPixels(xleds->setRGB(0, 0, 255));
    }
    if (color == "YELLOW")
    {
        colorPixels(xleds->setRGB(255, 255, 0));
    }
    if (color == "ORANGE")
    {
        colorPixels(xleds->setRGB(255, 128, 0));
    }
    if (color == "MAGENTA")
    {
        colorPixels(xleds->setRGB(255, 0, 255));
    }
    if (color == "PURPLE")
    {
        colorPixels(xleds->setRGB(212, 0, 255));
    }
    if (color == "CYAN")
    {
        colorPixels(xleds->setRGB(0, 255, 255));
    }
    if (color == "AMBIENT")
    {
        colorPixels(xleds->setRGB(lights->getRgbVal('r'), lights->getRgbVal('g'), lights->getRgbVal('b')));
    }
    if (!increment)
    {
        return;
    }
    brightenPixels(pulse_t);
    if (pulse_dir == 0)
    {
        if (pulse_t < 42)
        {
            pulse_t += 2;
        }
        else
        {
            pulse_t += 5;
        }
    }
    else
    {
        if (pulse_t < 42)
        {
            pulse_t -= 2;
        }
        else
        {
            pulse_t -= 5;
        }
    }
    if (pulse_t >= 255)
    {
        pulse_dir = 1;
        pulse_t = 255;
    }
    else if (pulse_t <= 0)
    {
        pulse_dir = 0;
        pulse_t = 0;
    }
}

void FancyLED::pulseRGB(uint8_t r, uint8_t g, uint8_t b, bool increment)
{
    colorPixels(xleds->setRGB(r, g, b));
    if (!increment)
    {
        return;
    }
    brightenPixels(pulse_t);
    if (pulse_dir == 0)
    {
        if (pulse_t < 42)
        {
            pulse_t += 2;
        }
        else
        {
            pulse_t += 5;
        }
    }
    else
    {
        if (pulse_t < 42)
        {
            pulse_t -= 2;
        }
        else
        {
            pulse_t -= 5;
        }
    }
    if (pulse_t >= 255)
    {
        pulse_dir = 1;
        pulse_t = 255;
    }
    else if (pulse_t <= 0)
    {
        pulse_dir = 0;
        pulse_t = 0;
    }
}

void FancyLED::disableAll()
{
    uint16_t i;
    for (i = 0; i < NUM_LEDS; i++)
    {
        xleds[i] = xleds->setRGB(0, 0, 0);
    }
    FastLED.show();
}
