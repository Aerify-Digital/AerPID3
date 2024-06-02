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

#ifndef _AER_FAN_PWM_H
#define _AER_FAN_PWM_H

#include <Arduino.h>

#define Period  10000

unsigned long usecLst;
unsigned long period;
byte          fstate;

uint8_t duty_cycle = 0;

// -----------------------------------------------------------------------------
void softPWM (
  unsigned long   usec,
  byte            pin,
  byte            dutyCycle,
  byte           *pState,
  unsigned long  *pPeriod,
  unsigned long  *pUsecLst )
{
  if (dutyCycle >= 254 && (usec - *pUsecLst) >= *pPeriod) {
    *pUsecLst = usec;
    digitalWrite (pin, LOW);
  } else if (dutyCycle <= 1 && (usec - *pUsecLst) >= *pPeriod) {
    *pUsecLst = usec;
    digitalWrite (pin, HIGH);
  } else if ((usec - *pUsecLst) >= *pPeriod)  {
    *pUsecLst = usec;
    if (HIGH == *pState) {
      *pPeriod = Period * dutyCycle / 100;
    } else {
      *pPeriod = Period * (255 - dutyCycle) / 100;
    }
    *pState = ! *pState;
    digitalWrite (pin, fstate);
  }
}

#endif
