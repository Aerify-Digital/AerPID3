#ifndef _SERIAL_COMMANDS_H
#define _SERIAL_COMMANDS_H

// Max message size in bytes
// This was 16384! It was lowered due to heap issues...  -squid
#define MAX_MESSAGE_SIZE 4096

// Encoded message flags
#define START_BYTE 0x02
#define END_BYTE 0x03
#define ESCAPE_BYTE 0x10

enum SerialCommand
{
    CMD_INIT = 0x20,
    CMD_INIT_ADV = 0x2A,
    CMD_STATUS = 0x21,
    CMD_INIT2 = 0x22,
    CMD_STATUS2 = 0x23,
    AUTO_OFF_LENGTH = 0x35,
    AUTO_OFF_TOGGLE = 0x32,
    ADJUST_AMOUNT = 0x24,
    CMD_BLE = 0x50,
    BUMP_UPDATE = 0x43,
    BUMP_AMOUNT = 0x44,
    BUMP_LENGTH = 0x45,
    BUMP_TOGGLE = 0x42,
    COIL_TOGGLE = 0x52,
    COIL_TOGGLE2 = 0x53,
    CMD_ESP = 0x00,
    FAV_1 = 0x86,
    FAV_2 = 0x87,
    FAV_3 = 0x88,
    FAV_4 = 0x89,
    CMD_LED = 0x70,
    CMD_PID = 0x90,
    CMD_PID2 = 0x91,
    CMD_ADV1_PWM = 0x92,
    CMD_ADV2_PWM = 0x93,
    CMD_ADV1_PID = 0x94,
    CMD_ADV2_PID = 0x95,
    CMD_TEMP = 0x10,
    CMD_UNIT = 0x11,
    CMD_TEMP2 = 0x13,
    CMD_WIFI = 0x60,
    CMD_DEBUG = 0xff,
};

enum Operation
{
    OP_GET = 0x00,
    OP_SET = 0x01,
};

enum LED
{
    PARAM_ENABLE = 0x00,
    PARAM_MODE = 0x01,
    PARAM_COLOR = 0x02,
    PARAM_BRIGHTNESS = 0x03,
    PARAM_STAT = 0x04,
};

enum PARAM_PID
{
    PARAM_PID_P,
    PARAM_PID_I,
    PARAM_PID_D,
    PARAM_PID,
};

enum PARAM_ADV
{
    PARAM_ADV_PWM_FACTOR,
    PARAM_ADV_PWM_FREQ,
    PARAM_ADV_PWM_RES,
    PARAM_ADV_PID_BIAS,
    PARAM_ADV_PID_TIME,
    PARAM_ADV_PID_RES,
};

enum LEDMode
{
    PARAM_AMBIENT,
    PARAM_BLINK,
    PARAM_PULSE,
    PARAM_PRESET,
    PARAM_RAINBOW,
    PARAM_RAINBOW_PULSE,
    PARAM_RAINBOW_WAVE,
    PARAM_SHIFT,
    PARAM_PREPULSE,
    PARAM_STATIC,
    PARAM_STATUS,
};

enum ESP
{
    PARAM_RESET,
    PARAM_DEVICE_RESET,
};

enum BLE
{
    PARAM_ENABLE_BLE,
    PARAM_MONITOR_BLE,
};

enum WIFI
{
    PARAM_ENABLE_WIFI,
    PARAM_JOIN_WIFI,
    PARAM_RESET_WIFI,
    PARAM_SCAN_WIFI,
};

enum Favorite
{
    FAV_NAME,
    FAV_TEMP,
    FAV_BUMP,
};

#endif