#ifndef _AER_THERMALS_HELPER_H_
#define _AER_THERMALS_HELPER_H_

#include <stdio.h>
#include "stringHelper.hpp"

static double xround(double value)
{
    value = round(value * 100.0) / 100.0;
    return value;
}

// Celsius to Fahrenheit
static double toFahrenheit(double c)
{
    return ((double)c * ((double)9 / 5)) + 32;
}

// Celsius to Kelvin
static double toKelvin(double c)
{
    return c + 273.15;
}

// Kelvin to Celsius
static double fromKelvin(double k)
{
    return k - 273.15;
}

// Fahrenheit to Celsius
static double toCelsius(double f)
{
    return (f - 32) * ((double)5 / 9);
}

static std::string toStrUnitF(double t, uint precision = 2, bool convert = false)
{
    double f = t;
    if (convert)
    {
        f = toFahrenheit(t);
    }
    return _toFixed(to_string(xround(f)), precision) + " F";
}

static std::string toStrF(double t, uint precision = 2, bool convert = false)
{
    double f = t;
    if (convert)
    {
        f = toFahrenheit(t);
    }
    return _toFixed(to_string(xround(f)), precision);
}

static std::string toStrUnitC(double t, uint precision = 2, bool convert = false)
{
    double c = t;
    if (convert)
    {
        c = toCelsius(t);
    }
    return _toFixed(to_string(xround(c)), precision) + " C";
}

static std::string toStrC(double t, uint precision = 2, bool convert = false)
{
    double c = t;
    if (convert)
    {
        c = toCelsius(t);
    }
    return _toFixed(to_string(xround(c)), precision);
}

static std::string toStrUnitK(double t, uint precision = 2, bool convert = false)
{
    double k = t;
    if (convert)
    {
        k = toKelvin(t);
    }
    return _toFixed(to_string(xround(k)), precision) + " K";
}

static std::string toStrK(double t, uint precision = 2, bool convert = false)
{
    double k = t;
    if (convert)
    {
        k = toKelvin(t);
    }
    return _toFixed(to_string(xround(k)), precision);
}

#endif