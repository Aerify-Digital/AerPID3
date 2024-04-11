#ifndef MUTEX_H
#define MUTEX_H

#include "freertos/semphr.h"

static SemaphoreHandle_t i2c1_mutex;
static SemaphoreHandle_t spi1_mutex;
static SemaphoreHandle_t sys1_mutex;

#endif