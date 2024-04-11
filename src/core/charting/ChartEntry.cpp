#include "ChartEntry.h"

uint8_t *ChartEntry::toHexVal()
{
    switch (cType)
    {
    case eChartType::INT8:
        return valInt8.value.hex;
    case eChartType::INT16:
        return valInt16.value.hex;
    case eChartType::INT32:
        return valInt32.value.hex;
    case eChartType::INT64:
        return valInt64.value.hex;
    case eChartType::DOUBLE:
        return valDouble.value.hex;
    case eChartType::FLOAT:
        return valFloat.value.hex;
    case eChartType::LONG:
        return valLong.value.hex;

    default:
        return {0};
    }
}

std::string ChartEntry::key()
{
    switch (cType)
    {
    case eChartType::INT8:
        return valInt8.key;
    case eChartType::INT16:
        return valInt16.key;
    case eChartType::INT32:
        return valInt32.key;
    case eChartType::INT64:
        return valInt64.key;
    case eChartType::DOUBLE:
        return valDouble.key;
    case eChartType::FLOAT:
        return valFloat.key;
    case eChartType::LONG:
        return valLong.key;

    default:
        return {0};
    }
}

int8_t ChartEntry::vInt8()
{
    return valInt8.value.num;
}

int16_t ChartEntry::vInt16()
{
    return valInt16.value.num;
}

int32_t ChartEntry::vInt32()
{
    return valInt32.value.num;
}

int64_t ChartEntry::vInt64()
{
    return valInt64.value.num;
}

double ChartEntry::vDouble()
{
    return valDouble.value.num;
}

float ChartEntry::vFloat()
{
    return valFloat.value.num;
}

long ChartEntry::vLong()
{
    return valLong.value.num;
}

uint8_t ChartEntry::size()
{
    return size(cType);
}

uint8_t ChartEntry::size(eChartType t)
{
    switch (t)
    {
    case eChartType::INT8:
        return sizeof(int8_t);
    case eChartType::INT16:
        return sizeof(int16_t);
    case eChartType::INT32:
        return sizeof(int32_t);
    case eChartType::INT64:
        return sizeof(int64_t);
    case eChartType::DOUBLE:
        return sizeof(double);
    case eChartType::FLOAT:
        return sizeof(float);
    case eChartType::LONG:
        return sizeof(long);

    default:
        return 0;
    }
}

// ===========================================================
// ===========================================================
// private functions

void ChartEntry::fromHex(std::string key, uint8_t *hex)
{
    built = cType != 0xff && cType != 0x00;
    switch (cType)
    {
    case eChartType::INT8:
        valInt8.key = key;
        for (uint8_t i = 0; i < sizeof(int8_t); i++)
        {
            valInt8.value.hex[i] = hex[i];
        }
        break;
    case eChartType::INT16:
        valInt16.key = key;
        for (uint8_t i = 0; i < sizeof(int16_t); i++)
        {
            valInt16.value.hex[i] = hex[i];
        }
        break;
    case eChartType::INT32:
        valInt32.key = key;
        for (uint8_t i = 0; i < sizeof(int32_t); i++)
        {
            valInt32.value.hex[i] = hex[i];
        }
        break;
    case eChartType::INT64:
        valInt64.key = key;
        for (uint8_t i = 0; i < sizeof(int64_t); i++)
        {
            valInt64.value.hex[i] = hex[i];
        }
        break;
    case eChartType::DOUBLE:
        valDouble.key = key;
        for (uint8_t i = 0; i < sizeof(double); i++)
        {
            valDouble.value.hex[i] = hex[i];
        }
        break;
    case eChartType::FLOAT:
        valFloat.key = key;
        for (uint8_t i = 0; i < sizeof(float); i++)
        {
            valFloat.value.hex[i] = hex[i];
        }
        break;
    case eChartType::LONG:
        valLong.key = key;
        for (uint8_t i = 0; i < sizeof(long); i++)
        {
            valLong.value.hex[i] = hex[i];
        }
        break;

    default:
        built = false;
        break;
    }
}
