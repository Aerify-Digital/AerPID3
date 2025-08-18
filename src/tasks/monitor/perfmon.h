#ifndef COMPONENTS_PERFMON_INCLUDE_PERFMON_H_
#define COMPONENTS_PERFMON_INCLUDE_PERFMON_H_

#include "core/controllers/AerManager.h"
#include "common/util/ArrayManipulator.h"

// #define DEGUG_PERFORMANCE_MONITOR 1

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_err.h"

    /// @brief Starts the performance monitor task
    /// @param am AerManager object with storage variables
    /// @param stackSize The size of the task stack specified as the number of bytes
    /// @return pdPASS if the task was successfully created and added to a ready list,
    /// otherwise an error code defined in the file projdefs.h
    esp_err_t perfmon_start(AerManager *am, uint32_t stackSize, TaskHandle_t *pxCreatedTask);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_PERFMON_INCLUDE_PERFMON_H_ */