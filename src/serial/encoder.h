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

#ifndef _SERIAL_ENCODE_H
#define _SERIAL_ENCODE_H

#include "common/enum/serial_commands.h"

#include "HardwareSerial.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

static int encodeData(char *data, uint len, char *encoded)
{
  std::vector<char> vct = {START_BYTE};
  for (int i = 0; i < len; i++)
  {
    char byte = data[i];
    switch (byte)
    {
    case START_BYTE:
    case END_BYTE:
    case ESCAPE_BYTE:
      vct.push_back(ESCAPE_BYTE);
      vct.push_back(byte ^ ESCAPE_BYTE);
      break;
    default:
      vct.push_back(byte);
      break;
    }
  }
  vct.push_back(END_BYTE);
  // convert vector to char array
  std::copy(vct.begin(), vct.end(), encoded);
  uint size = vct.size();
  vct.clear();
  vct.shrink_to_fit();
  return size;
}

static std::vector<char> encodeData(char *data, const uint len)
{
  std::vector<char> vct = {};
  // std::vector<char> vct = *(std::vector<char> *)malloc(len * sizeof(std::vector<char>));
  vct.push_back(START_BYTE);
  for (int i = 0; i < len; i++)
  {
    char byte = data[i];
    switch (byte)
    {
    case START_BYTE:
    case END_BYTE:
    case ESCAPE_BYTE:
      vct.push_back(ESCAPE_BYTE);
      vct.push_back(byte ^ ESCAPE_BYTE);
      break;
    default:
      vct.push_back(byte);
      break;
    }
  }
  vct.push_back(END_BYTE);
  return vct;
}

static bool decodeData(char *data, uint len, char *decoded)
{
  if (data[0] != START_BYTE)
  {
    return false;
  }
  if (data[len - 1] != END_BYTE)
  {
    return false;
  }
  std::vector<char> vct = {};
  bool endOfMessage = false;
  for (int i = 0; i < len; i++)
  {
    if (endOfMessage)
      break;
    switch (data[i])
    {
    case START_BYTE:
      break;
    case END_BYTE:
      endOfMessage = true;
      break;
    case ESCAPE_BYTE:
      ++i;
      vct.push_back(data[i] ^ ESCAPE_BYTE);
      break;
    default:
      vct.push_back(data[i]);
      break;
    }
  }
  std::copy(vct.begin(), vct.end(), decoded);
  vct.clear();
  vct.shrink_to_fit();
  return true;
}

static int decodeDataSerial(HardwareSerial ser, char *decoded)
{
  bool messageStarted = false;
  int bytesRead = 0;
  int bufferSize = 0;
  char *buffer = (char *)malloc(bufferSize);

  int c = 0;
  while (ser.available() && c++ < MAX_MESSAGE_SIZE)
  {
    /*uint8_t b[1];
    size_t sizr = ser.read(b, 1);
    if (sizr == 0)
    {
      break;
    }

    char c = b[0];*/
    
    char c = ser.read();

    if (c == START_BYTE)
    {
      // Start of message
      messageStarted = true;
      bytesRead = 0;
      bufferSize = 0; // Initial buffer size
      buffer = (char *)realloc(buffer, bufferSize);
      continue;
    }

    if (!messageStarted)
    {
      // Ignore data until message starts
      continue;
    }

    if (c == END_BYTE)
    {
      // End of message
      messageStarted = false;

      // Resize the buffer to the correct size
      buffer = (char *)realloc(buffer, bytesRead + 1);

      // Process the message here
      buffer[bytesRead] = '\0';
      // ser.println(bytesRead);
      for (int i = 0; i < bytesRead; i++)
      {
        // Serial.print(buffer[i], 10);
        // ser.print(" ");
        decoded[i] = buffer[i];
      }
      // ser.println("");

      // Free the buffer memory
      free(buffer);
      bufferSize = 0;
      buffer = (char *)malloc(bufferSize);

      continue;
    }
    // Add data to buffer
    // If buffer is full, allocate more memory
    if (bytesRead == bufferSize)
    {
      (bufferSize > 0 ? bufferSize : ++bufferSize) += 1; // Double the buffer size
      buffer = (char *)realloc(buffer, bufferSize);
    }

    if (c == ESCAPE_BYTE)
    {
      char d = ser.read();
      buffer[bytesRead] = (d ^ ESCAPE_BYTE);
      bytesRead++;
    }
    else
    {
      buffer[bytesRead] = c;
      bytesRead++;
    }

    if (bytesRead >= MAX_MESSAGE_SIZE)
    {
      // Buffer overflow, reset buffer
      bytesRead = 0;
    }
  }
  return bytesRead;
}

#endif