#ifndef _CMD_OP_REPLY_TYPE_H
#define _CMD_OP_REPLY_TYPE_H

typedef enum
{
    OKAY_EMITTED = 0,
    ERROR_NOT_CONNECTED = 1,
    ERROR_SIZE = 2,
    ERROR_NOT_BUILT = 3,
    ERROR_BUILD_FAIL = 4,
    ERROR_ENCODED_EMPTY = 5,
    ERROR_EMITTED_DONE = 6,
} cmd_op_reply_t;

#endif