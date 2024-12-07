#include "SocketCmdOp.h"

SocketCmdOp SocketCmdOp::AddClient(uint32_t clnt)
{
    this->clnts.push_back(clnt);
    this->hasClnt = true;
    return *this;
}

SocketCmdOp SocketCmdOp::Cmd(uint8_t cmd)
{
    this->cmd = cmd;
    if (!this->hasCmd)
    {
        this->hasCmd = true;
        this->size++;
    }
    return *this;
}

SocketCmdOp SocketCmdOp::Param(uint8_t p)
{
    this->param = p;
    if (!this->hasParam)
    {
        this->hasParam = true;
        this->size++;
    }
    return *this;
}

SocketCmdOp SocketCmdOp::Op(uint8_t op)
{
    this->op = op;
    if (!this->hasOp)
    {
        this->hasOp = true;
        this->size++;
    }
    return *this;
}

SocketCmdOp SocketCmdOp::Val(uint8_t *v)
{
    for (uint i = 0; i < sizeof(v); i++)
    {
        this->val.push_back(v[i]);
        this->size++;
    }
    this->hasVal = true;
    return *this;
}

SocketCmdOp SocketCmdOp::Val(char *v)
{
    this->Val((uint8_t *)v);
    return *this;
}

SocketCmdOp SocketCmdOp::Val(uint8_t v)
{
    this->val.push_back(v);
    this->hasVal = true;
    this->size++;
    return *this;
}

SocketCmdOp SocketCmdOp::Val(char v)
{
    this->Val((uint8_t)v);
    return *this;
}

SocketCmdOp SocketCmdOp::Val(bool b)
{
    this->val.push_back((uint8_t)(b ? 1 : 0));
    this->hasVal = true;
    this->size++;
    return *this;
}

SocketCmdOp SocketCmdOp::Val(uint16_t w)
{
    this->val.push_back(w >> 8);
    this->val.push_back(w & 0xff);
    this->hasVal = true;
    this->size += 2;
    return *this;
}

SocketCmdOp SocketCmdOp::Val(uint32_t n)
{
    this->val.push_back((n >> 16) & 0xff);
    this->val.push_back((n >> 8) & 0xff);
    this->val.push_back((n >> 0) & 0xff);
    this->hasVal = true;
    this->size += 3;
    return *this;
}

SocketCmdOp SocketCmdOp::Val(double d)
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

SocketCmdOp SocketCmdOp::Val(int n)
{
    this->val.push_back((n >> 24) & 0xff);
    this->val.push_back((n >> 16) & 0xff);
    this->val.push_back((n >> 8) & 0xff);
    this->val.push_back((n >> 0) & 0xff);
    this->hasVal = true;
    this->size += 4;
    return *this;
}

uint SocketCmdOp::Size()
{
    return this->size;
}

bool SocketCmdOp::didEmit()
{
    return this->emitted;
}

bool SocketCmdOp::build()
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

cmd_op_reply_t SocketCmdOp::emit(AsyncWebSocket *sock)
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
    if (hasClnt && sock->enabled())
    {
        const uint len = data.size();
        if (len > 0)
        {
            char *msg = new char[len];
            std::copy(data.begin(), data.end(), msg);
            std::vector<char> encoded = encodeData(msg, len);
            delete[] msg;
            if (!encoded.empty())
            {
                uint elen = encoded.size();
                char *enc = new char[elen];
                std::copy(encoded.begin(), encoded.end(), enc);
                for (int t = 0; t < clnts.size(); t++)
                {
                    sock->binary(clnts.at(t), enc, elen);
                }
                delete[] enc;
                emitted = true;
                ret = cmd_op_reply_t::OKAY_EMITTED;
            }
            encoded.clear();
            encoded.shrink_to_fit();
        }
        ret = cmd_op_reply_t::ERROR_ENCODED_EMPTY;
    }
    dispose();
    return ret;
}

cmd_op_reply_t SocketCmdOp::emitAll(AsyncWebSocket *sock)
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
    this->hasClnt = true;
    cmd_op_reply_t ret = ERROR_NOT_CONNECTED;
    if (hasClnt && sock->enabled())
    {
        const uint len = data.size();
        if (len > 0)
        {
            char *msg = new char[len];
            std::copy(data.begin(), data.end(), msg);
            std::vector<char> encoded = encodeData(msg, len);
            delete[] msg;
            if (!encoded.empty())
            {
                uint elen = encoded.size();
                char *enc = new char[elen];
                std::copy(encoded.begin(), encoded.end(), enc);
                sock->binaryAll(enc, elen);
                delete[] enc;
                emitted = true;
                ret = cmd_op_reply_t::OKAY_EMITTED;
            }
            encoded.clear();
            encoded.shrink_to_fit();
        }
        ret = cmd_op_reply_t::ERROR_ENCODED_EMPTY;
    }
    dispose();
    return ret;
}

cmd_op_reply_t SocketCmdOp::buildThenEmit(AsyncWebSocket *sock)
{
    if (build())
    {
        return emit(sock);
    }
    return cmd_op_reply_t::ERROR_BUILD_FAIL;
}

void SocketCmdOp::dispose()
{
    if (isDisposed)
    {
        return;
    }
    if (this->clnts.size() > 0)
    {
        this->clnts.clear();
        this->clnts.shrink_to_fit();
        this->clnts = {};
    }
    if (this->val.size() > 0)
    {
        this->val.clear();
        this->val.shrink_to_fit();
        this->val = {};
    }
    if (this->data.size() > 0)
    {
        this->data.clear();
        this->data.shrink_to_fit();
        this->data = {};
    }
    this->op = 0;
    this->cmd = 0;
    this->param = 0;
    this->size = 0;
    this->isDisposed = true;
}