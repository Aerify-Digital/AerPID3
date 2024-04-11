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

#ifndef _SERIAL_COMMAND_OPERATION_H
#define _SERIAL_COMMAND_OPERATION_H

#include "common/enum/serial_commands.h"
#include "common/enum/CommandOperationReply.h"
#include "encoder.h"

#include "HardwareSerial.h"
#include <stdio.h>
#include <iostream>
#include <map>
#include <vector>

class SerialCmdOp
{
public:
    SerialCmdOp() {}
    SerialCmdOp(SerialCommand cmd)
    {
        this->Cmd(cmd);
    }
    SerialCmdOp(char *dat, uint len)
    {
        this->Cmd(dat[0]); // first byte must be command!
        for (uint i = 1; i < len; i++)
        {
            this->val.push_back(dat[i]);
            this->size++;
        }
        this->hasVal = true;
    }
    SerialCmdOp(SerialCommand cmd, char *dat, uint len)
    {
        this->Cmd(cmd).Val(dat);
    }
    ~SerialCmdOp() { dispose(); }

    /* Command */
    SerialCmdOp Cmd(uint8_t cmd);
    /* Parameter */
    SerialCmdOp Param(uint8_t p);
    /* Operation */
    SerialCmdOp Op(uint8_t op);
    /* Value */
    SerialCmdOp Val(uint8_t *v);
    SerialCmdOp Val(char *v);
    SerialCmdOp Val(uint8_t v);
    SerialCmdOp Val(char v);
    SerialCmdOp Val(bool b);
    SerialCmdOp Val(uint16_t w);
    SerialCmdOp Val(uint32_t n);
    SerialCmdOp Val(double d);

    uint Size();

    bool build();
    cmd_op_reply_t emit();
    cmd_op_reply_t buildThenEmit();

protected:
    bool didEmit();
    void dispose();

private:
    uint8_t cmd;
    uint8_t param;
    uint8_t op;

    std::vector<uint8_t> val = {};
    std::vector<uint8_t> data = {};

    uint size = 0;

    bool hasCmd = false;
    bool hasParam = false;
    bool hasOp = false;
    bool hasVal = false;
    bool isBuilt = false;
    bool emitted = false;
    bool isDisposed = false;
};

#endif