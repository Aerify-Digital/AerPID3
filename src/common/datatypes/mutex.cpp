#include "freertos/FreeRTOS.h" // must precede semphr.h (pulled in by mutex.h)
#include "mutex.h"

// Single definition of the shared mutex handles declared `extern` in mutex.h.
// Created (xSemaphoreCreateMutex) in main.cpp setup(); referenced by every TU that includes mutex.h.
SemaphoreHandle_t i2c1_mutex;
SemaphoreHandle_t spi1_mutex;
SemaphoreHandle_t sys1_mutex;
