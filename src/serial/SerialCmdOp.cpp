#include "SerialCmdOp.h"

SerialCmdOp SerialCmdOp::Cmd(uint8_t cmd)
{
    this->cmd = cmd;
    if (!this->hasCmd)
    {
        this->hasCmd = true;
        this->size++;
    }
    return *this;
}

SerialCmdOp SerialCmdOp::Param(uint8_t p)
{
    this->param = p;
    if (!this->hasParam)
    {
        this->hasParam = true;
        this->size++;
    }
    return *this;
}

SerialCmdOp SerialCmdOp::Op(uint8_t op)
{
    this->op = op;
    if (!this->hasOp)
    {
        this->hasOp = true;
        this->size++;
    }
    return *this;
}

SerialCmdOp SerialCmdOp::Val(uint8_t *v)
{
    for (uint i = 0; i < sizeof(v); i++)
    {
        this->val.push_back(v[i]);
        this->size++;
    }
    this->hasVal = true;
    return *this;
}

SerialCmdOp SerialCmdOp::Val(char *v)
{
    this->Val((uint8_t *)v);
    return *this;
}

SerialCmdOp SerialCmdOp::Val(uint8_t v)
{
    this->val.push_back(v);
    this->hasVal = true;
    this->size++;
    return *this;
}

SerialCmdOp SerialCmdOp::Val(char v)
{
    this->Val((uint8_t)v);
    return *this;
}

SerialCmdOp SerialCmdOp::Val(bool b)
{
    this->val.push_back((uint8_t)(b ? 1 : 0));
    this->hasVal = true;
    this->size++;
    return *this;
}

SerialCmdOp SerialCmdOp::Val(uint16_t w)
{
    this->val.push_back(w >> 8);
    this->val.push_back(w & 0xff);
    this->hasVal = true;
    this->size += 2;
    return *this;
}

SerialCmdOp SerialCmdOp::Val(uint32_t n)
{
    this->val.push_back((n >> 16) & 0xff);
    this->val.push_back((n >> 8) & 0xff);
    this->val.push_back((n >> 0) & 0xff);
    this->hasVal = true;
    this->size += 3;
    return *this;
}

SerialCmdOp SerialCmdOp::Val(double d)
{
    union byteDouble
    {
        double value;
        char bytes[sizeof(double)];
    };
    byteDouble bd;
    bd.value = d;
    for (uint j = 0; j < 8; j++)
    {
        this->val.push_back(bd.bytes[j]);
    }
    this->hasVal = true;
    this->size += 8;
    return *this;
}

uint SerialCmdOp::Size()
{
    return this->size;
}

bool SerialCmdOp::didEmit()
{
    return this->emitted;
}

bool SerialCmdOp::build()
{
    if (isBuilt)
    {
        return true;
    }
    if (size == 0 || isDisposed)
    {
        return false;
    }
    if (hasCmd)
    {
        data.push_back(cmd);
    }
    if (hasParam)
    {
        data.push_back(param);
    }
    if (hasOp)
    {
        data.push_back(op);
    }
    if (hasVal)
    {
        for (uint i = 0; i < val.size(); i++)
        {
            data.push_back(val[i]);
        }
    }
    return isBuilt = true;
}

cmd_op_reply_t SerialCmdOp::emit()
{
    if (!isBuilt)
    {
        return cmd_op_reply_t::ERROR_NOT_BUILT;
    }
    if (emitted || isDisposed)
    {
        return cmd_op_reply_t::ERROR_EMITTED_DONE;
    }
    if (this->size != data.size())
    {
        return cmd_op_reply_t::ERROR_SIZE;
    }
    cmd_op_reply_t ret = ERROR_NOT_CONNECTED;
    if (Serial)
    {
        const uint len = data.size();
        char *msg = new char[len];
        std::copy(data.begin(), data.end(), msg);
        std::vector<char> encoded = encodeData(msg, len);
        delete[] msg;
        if (!encoded.empty())
        {
            delayMicroseconds(64 * 1000);
            for (int i = 0; i < encoded.size(); i++)
            {
                Serial.print(encoded.at(i));
            }
            // delayMicroseconds(64 * 1000);
            emitted = true;
            ret = cmd_op_reply_t::OKAY_EMITTED;
        }
        encoded.clear();
        encoded.shrink_to_fit();
        ret = cmd_op_reply_t::ERROR_ENCODED_EMPTY;
    }
    dispose();
    return ret;
}

cmd_op_reply_t SerialCmdOp::buildThenEmit()
{
    if (build())
    {
        return emit();
    }
    return cmd_op_reply_t::ERROR_BUILD_FAIL;
}

void SerialCmdOp::dispose()
{
    if (isDisposed)
    {
        return;
    }
    if (this->val.size() > 0)
    {
        this->val.clear();
        this->val.shrink_to_fit();
    }
    if (this->data.size() > 0)
    {
        this->data.clear();
        this->data.shrink_to_fit();
    }
    this->size = 0;
    this->isDisposed = true;
}