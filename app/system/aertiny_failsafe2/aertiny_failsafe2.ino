/* =============================================================================
   Element Software for (ATTiny88) 'AerPID-Lite' or the 'AerTiny' device module.
   Copyright (c) 2015-2024 AerifyDigital LLC.  All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
   =============================================================================
*/

#include <Wire.h>
#include <Arduino.h>
#include "ledEffects.h"
#include "fanPWM.h"

using namespace LED_EFFECTS;

// avrdude -c usbasp -p t88 -B 5 -U lfuse:w:0xE2:m -U hfuse:w:0xDD:m
// avrdude -c usbasp -p t88 -U flash:w:aertiny_failsafe.ino.hex:i

// ==============================
// Pin setups

#define SSR_OV_1 0
#define SSR_OV_2 4

#define SSR_CONTROL_1 2
#define SSR_CONTROL_2 3

#define PIN_FAN 8

#define LED_A 17 // RED
#define LED_B 18 // GREEN
#define LED_C 19 // BLUE
#define LED_D 1  // RED

#define LED_E 20 // External

#define DTR_CONTROL 6

#define INVERT_DVR false

// ==============================

// i2c Address
#define I2C_SLAVE_ADDRESS 0x2D

uint8_t response = 0xff;

uint8_t state = LOW;

long element_timeout = 0;
bool element_enb = false;
bool system_init = false;
bool system_error = false;
bool dtr_enabled = false;
bool dtr_mode_on = false;

typedef enum ELM_PROT_OP_CODE : uint8_t
{
  E_PROT_NONE = 0,
  E_PROT_INIT = 1,
  E_PROT_NEXT = 2,
  E_PROT_WAIT = 3,
  E_PROT_OKAY = 4,
  E_PROT_RESET = 5,
  E_PROT_ERROR = 6,
  E_PROT_NACK = 7,
  E_PROT_ACK = 8,
  E_PROT_ERROR_NONE = 9,
  E_PROT_INIT_READY = 10,
  E_PROT_ERROR_NULL = 255,
  E_PROT_FAN_CONTROL = 50,
} _elm_prot_op_code_t;

// ===============================================================
// Main Setup

void setup()
{
  pinMode(LED_A, OUTPUT);
  pinMode(LED_B, OUTPUT);
  pinMode(LED_C, OUTPUT);
  pinMode(LED_D, OUTPUT);
  pinMode(LED_E, OUTPUT);

  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_B, LOW);
  digitalWrite(LED_C, LOW);
  digitalWrite(LED_D, LOW);

  pinMode(SSR_OV_1, OUTPUT);
  digitalWrite(SSR_OV_1, HIGH);
  pinMode(SSR_OV_2, OUTPUT);
  digitalWrite(SSR_OV_2, HIGH);

  pinMode(SSR_CONTROL_1, INPUT);
  pinMode(SSR_CONTROL_2, INPUT);

  pinMode(DTR_CONTROL, OUTPUT);
  digitalWrite(DTR_CONTROL, INVERT_DVR ? HIGH : LOW);

  pinMode(PIN_FAN, OUTPUT);
  digitalWrite(PIN_FAN, HIGH);
  
  delay(100);
  // init i2c slave
  Wire.begin(I2C_SLAVE_ADDRESS);
  // Wire.setClock(104000);
  Wire.onRequest(requestEvent); // register request event
  Wire.onReceive(receiveEvent); // register receive listener

  delay(300);
  digitalWrite(LED_A, HIGH);
  digitalWrite(LED_B, HIGH);
  digitalWrite(LED_C, HIGH);
  digitalWrite(LED_D, HIGH);

  // attachInterrupt(digitalPinToInterrupt(SSR_CONTROL_1), updateStatus, CHANGE);
}

// ===============================================================
// functions

long time()
{
  return millis() / 1000;
}

void updateStatus()
{
  element_timeout = time();
}

void disableElement()
{
  element_enb = false;
  digitalWrite(SSR_OV_1, HIGH);
  digitalWrite(SSR_OV_2, HIGH);
}

// ===============================================================
// Main Loop
// ===============================================================

void loop()
{
  unsigned long usec = micros();
  softPWM(usec, PIN_FAN, duty_cycle, &fstate, &period, &usecLst);

  tickFadePin();
  // set rx status led off
  digitalWrite(LED_C, HIGH);

  if (dtr_mode_on)
  {
    fadePin(LED_C);
  }

  // check for sys init...
  if (!system_init)
  {
    disableElement();
    if (millis() % 1000 == 0)
    {
      state = !state;
    }
    fadePin(LED_A);
    //digitalWrite(LED_B, !state);
    digitalWrite(LED_B, HIGH);
    digitalWrite(DTR_CONTROL, INVERT_DVR ? LOW : HIGH);
    dtr_mode_on = true;
    delay(1);
    return;
  }

  if (!dtr_enabled)
  {
    dtr_mode_on = false;
    digitalWrite(DTR_CONTROL, INVERT_DVR ? HIGH : LOW);
  }

  // status led
  if (element_enb)
  {
    fadePin(LED_B);
  }
  else
  {
    fadePin(LED_A);
  }

  // timeout error
  if (system_init && time() - element_timeout > 30)
  {
    system_init = false;
    system_error = true;
    digitalWrite(LED_D, HIGH);
  }
  else if (!system_error)
  {
    digitalWrite(LED_D, LOW);
  }

  // check for error and handle
  if (time() - element_timeout > 5)
  {
    disableElement();
    // digitalWrite(LED_A, HIGH);
    digitalWrite(LED_B, HIGH);
  }
  else if (!element_enb)
  {
    element_enb = true;
    digitalWrite(SSR_OV_1, LOW);
    digitalWrite(SSR_OV_2, LOW);
    digitalWrite(LED_A, HIGH);
  }
  else
  {
    digitalWrite(LED_A, HIGH);
  }

  delay(2);
}

// ===============================================================
// ===============================================================

/**
   @brief i2c Request Event

*/
void requestEvent()
{
  // noInterrupts();
  Wire.write(response); // sendData
  response = E_PROT_ERROR_NONE;
  digitalWrite(LED_C, HIGH);
  // interrupts();
}

// ===============================================================

/**
   @brief i2c Receive Event

   @param howMany
*/
void receiveEvent(int howMany)
{
  if (howMany <= 0)
  {
    return;
  }

  if (howMany > 2) {
    while (Wire.available() > 0 && howMany-- > 0)
    {
      uint8_t val = (uint8_t)Wire.read();
    }
    return;
  }

  noInterrupts();
  digitalWrite(LED_C, LOW);
  digitalWrite(LED_E, HIGH);
  // read one byte of data on the wire
  response = (uint8_t)Wire.read();
  interrupts();

  // fan control
  if (response == E_PROT_FAN_CONTROL && Wire.available() > 0)
  {
    uint8_t val = (uint8_t)Wire.read();
    duty_cycle = val;
    response = 8;
    return;
  }

  // DTR prog control
  if (response == 99)
  {
    response = dtr_enabled ? 1 : 0;
    return;
  }
  else if (response == 100)
  {
    digitalWrite(DTR_CONTROL, INVERT_DVR ? LOW : HIGH);
    dtr_enabled = true;
    dtr_mode_on = true;
    response = E_PROT_OKAY;
    return;
  }
  else if (response == 200)
  {
    digitalWrite(DTR_CONTROL, INVERT_DVR ? HIGH : LOW);
    dtr_enabled = false;
    response = E_PROT_OKAY;
    return;
  }

  // heartbeat state machine
  if (system_error && response != E_PROT_RESET)
  {
    response = E_PROT_RESET;
    updateStatus();
  }
  else if (response == E_PROT_INIT)
  {
    digitalWrite(LED_A, LOW);
    // digitalWrite(LED_B, LOW);
    digitalWrite(LED_C, LOW);
    system_init = true;
    system_error = false;
    response = E_PROT_INIT_READY;
  }
  else if (response == E_PROT_WAIT)
  {
    digitalWrite(LED_B, LOW);
    response = E_PROT_WAIT;
  }
  else if (response == E_PROT_RESET)
  {
    digitalWrite(LED_A, LOW);
    system_init = false;
    system_error = false;
    response = E_PROT_NEXT;
  }
  else if (response == E_PROT_ACK || response == E_PROT_OKAY)
  {
    if (system_init && !system_error)
    {
      updateStatus();
      if (response == E_PROT_OKAY)
      {
        response = E_PROT_OKAY;
      }
      else
      {
        response = E_PROT_ACK;
      }
    }
    else
    {
      response = E_PROT_NACK;
    }
  }
  else
  {
    response = E_PROT_ERROR;
  }
}
