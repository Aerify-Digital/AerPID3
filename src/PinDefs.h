/* =============================================================================
 * Application Pin definitions for 'AerPID-Lite' or the 'AerTiny' device module.
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

#ifndef PIN_DEFS_H
#define PIN_DEFS_H

// ======================
// Device Addresses
// ======================
#define SCREEN_ADDRESS 0x3C
#define EEPROM_ADDRESS 0x50 // or 0x54 ?
#define LM75_ADDRESS_A 0x48
#define LM75_ADDRESS_B 0x49
#define RCLOCK_ADDRESS 0x51

// ======================
// Pin Defines
#define PIN_LED_C 17
#define PIN_LED_B 12
// Fnacy LED Control PIN
#define PIN_xLED_A 4
#define PIN_xLED_B 16
// EEPROM Enable PIN
#define PIN_EEPROM_EN 13
// Flash Select PIN
#define PIN_FLSH_CS 5

// One Wire Bus Pin
#define PIN_ONE_WIRE 15

// SSR Enable Pin
#define PIN_SSR_EN 2
#define PIN_SSR_CHAN 0

// Interface Pins
#define ENC_A 33  // Encoder Knob
#define ENC_B 25  // Encoder Knob
#define ENC_SW 32 // encoder button
#define BTN_A 34  // Button 1
#define BTN_B 35  // Button 2

#endif