#ifndef MUTEX_H
#define MUTEX_H

#include "freertos/semphr.h"

// extern (not static): one shared instance across all translation units. `static` here gave each
// including .cpp its own NULL copy — only main.cpp's set got initialized, so other TUs (e.g.
// serial_com.cpp) saw NULL handles and their locking was non-functional. Definitions live in mutex.cpp.
extern SemaphoreHandle_t i2c1_mutex;
extern SemaphoreHandle_t spi1_mutex;
extern SemaphoreHandle_t sys1_mutex;

#endif