/* =============================================================================
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

#ifndef SERIALCOM_H
#define SERIALCOM_H

#include "HardwareSerial.h"
#include "driver/uart.h"
#include "common/base64/base64.h"
#include "storage/crypt.h"
#include "storage/auth.h"
#include "storage/config.h"

#include "core/controllers/AerManager.h"
#include "core/controllers/AerPID.h"
#include "common/datatypes/NetworkVars.h"
#include "common/enum/serial_commands.h"
#include "encoder.h"
#include "messagePack.h"
#include "SerialCmdOp.h"
#include "util/splitter.h"
#include "util/thermals.hpp"

#include "IPAddress.h"

// #include "esp_heap_trace.h"
// #include "esp_heap_caps.h"

class SerialCom
{
public:
  SerialCom() {}

  void init(uint waitTimeSeconds = 10);
  void begin(AerManager *params);

  void serialEvent();
  void serialReceiveEvent(HardwareSerial ser);
  void handleEventBuffer();
  void handleEventBuffer(char *buffer);

  bool hasMessage();
  char *getDecoded();

  int isLoaded();
  int isConnected();

  void updateClients();

protected:
  void enPackFill(MessagePack *messagePack);

private:
  int loaded = 0;
  int USB_CONNECTED = 0;

  AerManager *aerManager;

  bool _hasMessage = false;
  bool messageStarted = false;
  int bytesRead = 0;
  int bufferSize = 0;
  char *buffer = (char *)malloc(bufferSize);
  char *decoded = (char *)malloc(bufferSize);

  void dispose();

  void handleInitCmd(char op);
  void handleLedCmd(char param, char op, char dat);
  void handleCoilCmd(char param, char op);

  union byteDouble
  {
    double value;
    char bytes[sizeof(double)];
  };
};

#endif
