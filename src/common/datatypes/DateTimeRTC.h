#ifndef _RTC_DATE_TIME_H_
#define _RTC_DATE_TIME_H_

#include <stdint.h>

typedef struct
{
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t year;
    uint8_t month;
    uint8_t day;
} DateTimeRTC;

#endif