#ifndef _LINK_TASK_H
#define _LINK_TASK_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "common/enum/ElementProtectOpCodes.h"
#include "common/util/ElementProtectOpCode.h"
#include "common/datatypes/mutex.h"
#include "screen/gui/gui.h"
#include "core/controllers/AerManager.h"

#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 104000
#ifndef PID_MONITOR_ADDR
#define PID_MONITOR_ADDR 0x2D
#endif

_elm_prot_op_code_t doOpStateCmd(_elm_prot_op_code_t op)
{
    _elm_prot_op_code_t new_op = E_PROT_NONE;
    switch (op)
    {
    case E_PROT_NONE:
    case E_PROT_INIT:
    case E_PROT_WAIT:
        new_op = E_PROT_INIT;
        break;
    case E_PROT_INIT_READY:
        new_op = E_PROT_OKAY;
        break;
    case E_PROT_NEXT:
        new_op = E_PROT_WAIT;
        break;
    case E_PROT_RESET:
    case E_PROT_ERROR:
    case E_PROT_NACK:
        new_op = E_PROT_RESET;
        break;
    case E_PROT_OKAY:
    case E_PROT_ACK:
        new_op = E_PROT_ACK;
        break;
    case E_PROT_ERROR_NONE:
        new_op = E_PROT_NONE;
        break;
    case E_PROT_ERROR_NULL:
        new_op = E_PROT_ERROR;
        break;
    default:
        new_op = E_PROT_ERROR_NONE;
        break;
    }
    return new_op;
}

void link_task(void *pvParameters)
{
    AerManager *_am = (AerManager *)pvParameters; // task parameters
    vTaskDelay(3200);

    _elm_prot_op_code_t _op_state = E_PROT_NONE;

    /*if (xSemaphoreTake(i2c1_mutex, 100) == pdTRUE)
    {
        Wire.beginTransmission(PID_MONITOR_ADDR);
        Wire.write(100);
        Wire.endTransmission();
        xSemaphoreGive(i2c1_mutex);
    }*/

    uint tick = 0;

    printf("\nStarting Element-Link task...\n");
    while (1)
    {
        if (xSemaphoreTake(sys1_mutex, 100) == pdTRUE)
        {
            if (xSemaphoreTake(i2c1_mutex, 100) == pdTRUE)
            {
                uint8_t ret = 0xff;

                // do op state command
                _elm_prot_op_code_t op_state_n = doOpStateCmd(_op_state);
                _op_state = E_PROT_NEXT;

                Wire.beginTransmission(PID_MONITOR_ADDR);
                Wire.write(op_state_n);
                ret = Wire.endTransmission();
                delayMicroseconds(5000);

                if (ret == 0)
                {
                    _op_state = E_PROT_ERROR_NONE;
                    ret = Wire.requestFrom(PID_MONITOR_ADDR, 1);
                    if (ret == 1)
                    {
                        _op_state = (_elm_prot_op_code_t)Wire.read();
                    }
                }
                else
                {
                    _op_state = E_PROT_ERROR;
                }

                /*String op_state_n_str = OperationToString(op_state_n);
                String op_state_str = OperationToString(_op_state);

                Serial.print("op_state_n: ");
                Serial.print(op_state_n_str);
                Serial.print("  ");

                Serial.print("_op_state: ");
                Serial.print(op_state_str);
                Serial.print(" \n");*/

                if (_am->hasUpdateDTR())
                {
                    delayMicroseconds(1000);
                    Wire.beginTransmission(PID_MONITOR_ADDR);
                    if (!_am->isEnabledDTR())
                    {
                        Wire.write(200);
                    }
                    else
                    {
                        Wire.write(100);
                    }
                    Wire.endTransmission();
                    _am->hasUpdateDTR(false);
                    delayMicroseconds(500);
                }

                if (tick++ >= 5)
                {
                    tick = 0;
                    delayMicroseconds(8000);
                    Wire.beginTransmission(PID_MONITOR_ADDR);
                    Wire.write(99);
                    ret = Wire.endTransmission();
                    delayMicroseconds(5000);
                    if (ret == 0)
                    {
                        ret = Wire.requestFrom(PID_MONITOR_ADDR, 1);
                        if (ret == 1 && Wire.read() == 1)
                        {
                            _am->setEnabledDTR(true);
                        }
                        else
                        {
                            _am->setEnabledDTR(false);
                        }
                    }
                }

                // release i2c lock
                xSemaphoreGive(i2c1_mutex);
            }

            // Set global current op state
            _am->setOpEP(&_op_state);

            // release system lock
            xSemaphoreGive(sys1_mutex);
        }
        vTaskDelay(1650);
    }
}

#endif