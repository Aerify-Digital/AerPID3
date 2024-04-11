#ifndef _CHART_ENTRY_H_
#define _CHART_ENTRY_H_

#include <stdio.h>
#include <iostream>

// ============================
// enums

typedef enum eChartType : uint8_t
{
    EMPTY = 0,
    INT8 = 7,
    INT16 = 1,
    INT32 = 2,
    INT64 = 3,
    DOUBLE = 4,
    FLOAT = 5,
    LONG = 6,
    NONE = 255
} _e_chart_type_t;

// ============================
// unions

union cEntryInt8
{
    uint8_t hex[sizeof(int8_t)];
    int8_t num;
};

union cEntryInt16
{
    uint8_t hex[sizeof(int16_t)];
    int16_t num;
};

union cEntryInt32
{
    uint8_t hex[sizeof(int32_t)];
    int32_t num;
};

union cEntryInt64
{
    uint8_t hex[sizeof(int64_t)];
    int64_t num;
};

union cEntryDouble
{
    uint8_t hex[sizeof(double)];
    double num;
};

union cEntryFloat
{
    uint8_t hex[sizeof(float)];
    float num;
};

union cEntryLong
{
    uint8_t hex[sizeof(long)];
    long num;
};

// ============================
// structs

typedef struct
{
    std::string key;
    cEntryInt8 value;
} ChartEntryInt8;

typedef struct
{
    std::string key;
    cEntryInt16 value;
} ChartEntryInt16;

typedef struct
{
    std::string key;
    cEntryInt32 value;
} ChartEntryInt32;

typedef struct
{
    std::string key;
    cEntryInt64 value;
} ChartEntryInt64;

typedef struct
{
    std::string key;
    cEntryDouble value;
} ChartEntryDouble;

typedef struct
{
    std::string key;
    cEntryFloat value;
} ChartEntryFloat;

typedef struct
{
    std::string key;
    cEntryLong value;
} ChartEntryLong;

// ============================
// Class

class ChartEntry
{
public:
    ChartEntry(int32_t i, std::string key, int8_t val)
    {
        cType = eChartType::INT8;
        index = i;
        valInt8.key = key;
        valInt8.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }
    ChartEntry(int32_t i, std::string key, int16_t val)
    {
        cType = eChartType::INT16;
        index = i;
        valInt16.key = key;
        valInt16.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }
    ChartEntry(int32_t i, std::string key, int32_t val)
    {
        cType = eChartType::INT32;
        index = i;
        valInt32.key = key;
        valInt32.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }
    ChartEntry(int32_t i, std::string key, int64_t val)
    {
        cType = eChartType::INT64;
        index = i;
        valInt64.key = key;
        valInt64.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }
    ChartEntry(int32_t i, std::string key, double val)
    {
        cType = eChartType::DOUBLE;
        index = i;
        valDouble.key = key;
        valDouble.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }
    ChartEntry(int32_t i, std::string key, float val)
    {
        cType = eChartType::FLOAT;
        index = i;
        valFloat.key = key;
        valFloat.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }
    ChartEntry(int32_t i, std::string key, long val)
    {
        cType = eChartType::LONG;
        index = i;
        valLong.key = key;
        valLong.value.num = val;
        built = cType != 0xff && cType != 0x00;
    }

    // ChartEntry from hex
    ChartEntry(int32_t i, std::string key, eChartType t, uint8_t *hex)
    {
        cType = t;
        index = i;
        fromHex(key, hex);
    }

    // Entry object built/loaded flag
    bool isBuilt() { return built; }

    // Entry index
    uint32_t eIndex() { return index; }

    // Entry type
    eChartType eType() { return cType; }

    // Entry key
    std::string key();

    // Entry value as hex
    uint8_t *toHexVal();

    // Entry values typed
    int8_t vInt8();
    int16_t vInt16();
    int32_t vInt32();
    int64_t vInt64();
    double vDouble();
    float vFloat();
    long vLong();

    // get entry type hex length
    uint8_t size();

    // get entry type hex length
    static uint8_t size(eChartType t);

private:
    bool built = false;

    int32_t index = 0;
    eChartType cType = eChartType::NONE;

    ChartEntryInt16 valInt8;
    ChartEntryInt16 valInt16;
    ChartEntryInt32 valInt32;
    ChartEntryInt64 valInt64;
    ChartEntryDouble valDouble;
    ChartEntryFloat valFloat;
    ChartEntryLong valLong;

    // Parse hex by cType flag
    void fromHex(std::string key, uint8_t *hex);
};

#endif