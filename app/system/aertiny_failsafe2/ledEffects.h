/* =============================================================================
 * Element Software for (ATTiny88) 'AerPID-Lite' or the 'AerTiny' device module.
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

#ifndef _AER_LED_EFFECTS_H
#define _AER_LED_EFFECTS_H

#include <Arduino.h>

namespace LED_EFFECTS
{
    void fadeOut(uint8_t pin, uint16_t length);
    void fadeOut(uint8_t pin, uint16_t span, uint16_t length, uint16_t step);

    void fadeIn(uint8_t pin, uint16_t length);
    void fadeIn(uint8_t pin, uint16_t span, uint16_t length, uint16_t step);

    void fadePin(uint8_t pin);
    void tickFadePin();
};

#endif
