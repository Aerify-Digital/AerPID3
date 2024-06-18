#ifndef _AER_TASK_HANDLES_H
#define _AER_TASK_HANDLES_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// =======================
// Task Handles

TaskHandle_t btTask;
TaskHandle_t tftTask;
TaskHandle_t wrkTask;
TaskHandle_t pidTask;
TaskHandle_t pidTask1;
TaskHandle_t pidTask2;
TaskHandle_t serialTask;
TaskHandle_t encTask;
TaskHandle_t demoTask = NULL;
TaskHandle_t linkTask;
TaskHandle_t ledTask;
TaskHandle_t webTask;
TaskHandle_t saveTask;
TaskHandle_t elementTask;
TaskHandle_t webFetchTask;

QueueHandle_t usbQueue;

#endif