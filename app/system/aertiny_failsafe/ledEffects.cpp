#include "ledEffects.h"

namespace LED_EFFECTS
{
    void fadeOut(uint8_t pin, uint16_t length)
    {
        fadeOut(pin, 1, length, 1);
    }

    void fadeOut(uint8_t pin, uint16_t span, uint16_t length, uint16_t step)
    {
        int range = span * 1000;
        int limit = length * 100;
        int steps = (range / limit) * step;
        /*if (range - (steps * limit) < 0) {
          while (range - (steps * limit) < 0) {
            steps--;
          }
        }*/
        for (int i = limit; i > 0; i--)
        {
            digitalWrite(pin, LOW);
            delayMicroseconds(steps + (steps * i));
            digitalWrite(pin, HIGH);
            if (range - (steps * i) > 0)
            {
                delayMicroseconds(range - (steps * i));
            }
        }
    }

    void fadeIn(uint8_t pin, uint16_t length)
    {
        fadeIn(pin, 1, length, 1);
    }

    void fadeIn(uint8_t pin, uint16_t span, uint16_t length, uint16_t step)
    {
        int range = span * 1000;
        int limit = length * 100;
        int steps = (range / limit) * step;
        /*if (range - (steps * limit) < 0) {
          while (range - (steps * limit) < 0) {
            steps--;
          }
        }*/
        for (int i = 0; i < limit; i++)
        {
            digitalWrite(pin, LOW);
            delayMicroseconds(steps + (steps * i));
            digitalWrite(pin, HIGH);
            if (range - (steps * i) > 0)
            {
                delayMicroseconds(range - (steps * i));
            }
        }
        digitalWrite(pin, HIGH);
    }

    // ===============================================================

    void fadePin(uint8_t pin)
    {
        uint16_t span = 1;
        uint16_t length = 3;
        uint16_t step = 2;
        int range = span * 1000;
        int limit = length * 100;
        int steps = (range / limit) * step;
        {
            digitalWrite(pin, LOW);
            delayMicroseconds(steps + (steps * led_index));
            digitalWrite(pin, HIGH);
            if (range - (steps * led_index) > 0)
            {
                delayMicroseconds(range - (steps * led_index));
            }
        }
        if (!led_dir_out)
        {
            digitalWrite(pin, HIGH);
        }
    }

    void tickFadePin()
    {
        if (!led_dir_out)
        {
            led_index++;
        }
        else
        {
            led_index--;
        }
        if (led_index >= 255)
        {
            led_dir_out = true;
        }
        else if (led_index <= 0)
        {
            led_dir_out = false;
        }
    }
}