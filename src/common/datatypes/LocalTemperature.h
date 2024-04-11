#ifndef _LOCAL_TEMPERATURE_H_
#define _LOCAL_TEMPERATURE_H_

#include "../enum/ThermalUnitsType.h"

#include <stdio.h>

typedef struct
{
    // Kelvin, Celsius or Fahrenheit
    ThermalUnitsType thermalUnit;

    double tempA;
    double tempB;
    double tempC;
} LocalTemperature;

#endif