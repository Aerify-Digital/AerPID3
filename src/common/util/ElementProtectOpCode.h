#ifndef _ELM_PROT_OP_CODE_H
#define _ELM_PROT_OP_CODE_H

#include "common/enum/ElementProtectOpCodes.h"
#include "WString.h"

static String OperationToString(_elm_prot_op_code_t op)
{
    String str = "NONE_SETUP";
    switch (op)
    {
    case E_PROT_NONE:
        str = "NONE";
        break;
    case E_PROT_INIT:
        str = "INIT";
        break;
    case E_PROT_NEXT:
        str = "NEXT";
        break;
    case E_PROT_WAIT:
        str = "WAIT";
        break;
    case E_PROT_OKAY:
        str = "OKAY";
        break;
    case E_PROT_RESET:
        str = "RESET";
        break;
    case E_PROT_ERROR:
        str = "ERROR";
        break;
    case E_PROT_NACK:
        str = "NACK";
        break;
    case E_PROT_ACK:
        str = "ACK";
        break;
    case E_PROT_ERROR_NONE:
        str = "ERROR NO DATA"; // ERROR_NONE
        break;
    case E_PROT_INIT_READY:
        str = "INIT READY"; // INIT_READY
        break;
    case E_PROT_ERROR_NULL:
        str = "ERROR ON BUS"; // ERROR_NULL
        break;
    default:
        break;
    }
    return str;
}

#endif