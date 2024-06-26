#ifndef COMPONENTS_PERFMON_INCLUDE_PERFMON_H_
#define COMPONENTS_PERFMON_INCLUDE_PERFMON_H_

#include "core/controllers/AerManager.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_err.h"

    esp_err_t perfmon_start(AerManager *am, uint32_t stackSize);

#ifdef __cplusplus
}
#endif

#endif /* COMPONENTS_PERFMON_INCLUDE_PERFMON_H_ */