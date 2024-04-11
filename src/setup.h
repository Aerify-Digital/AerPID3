/* =============================================================================
 * Application setup defines for 'AerPID-Lite' or the 'AerTiny' device module.
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

#ifndef AER_SETUP_H
#define AER_SETUP_H

#include <Arduino.h>
#include <version.h>

// =========================
// This is for an ESP32 chip
#ifndef ESP32
#define ESP32
#endif

// =========================
// AerPID module type setup
#ifndef AERPID_DEVICE_TYPE
// default to single element control
#define AERPID_DEVICE_TYPE 1
#endif

#if !defined(AERPID_COUNT) && AERPID_DEVICE_TYPE == 1
#define AERPID_COUNT 1
#endif
#if !defined(AERPID_COUNT) && AERPID_DEVICE_TYPE == 2
#define AERPID_COUNT 2
#endif

// =========================
// Verbose debug mode toggle
#define DEBUG_VERBOSE true

// Adafruit BusIO serial debug
#define DEBUG_SERIAL 0

// =========================
// Addressable LED setup
#define NUM_LEDS 6      // number of addressable LEDs
#define CHIPSET WS2812B // addressable LED chipset
#define COLOR_ORDER GRB // addressable LED color order

// =========================
// port for http web server
#define HTTP_ADDRESS 80 // http server port

// ===============================================================
/**
 * @brief AerPID setup flags
 */
class AerSetup
{
public:
    // Verbose debug
    static const bool verbose_d = DEBUG_VERBOSE;
};

#endif