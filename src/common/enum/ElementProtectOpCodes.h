#ifndef _ELM_PROT_OP_CODES_H
#define _ELM_PROT_OP_CODES_H

#include <stdint.h>
#include "WString.h"

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
} _elm_prot_op_code_t;

#endif