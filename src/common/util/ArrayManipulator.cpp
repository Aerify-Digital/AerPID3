#include "ArrayManipulator.h"

uint8_t calcAverage(uint8_t *calls, int count)
{
    uint total = 0;
    for (uint i = 0; i < count; i++)
    {
        total += calls[i];
    }
    return total / count;
}

uint64_t calcAverage(uint64_t *calls, int count)
{
    uint64_t total = 0;
    for (uint i = 0; i < count; i++)
    {
        total += calls[i];
    }
    return total / count;
}

void shiftArrDown(uint8_t *arr, size_t size)
{
    uint8_t *new_arr = (uint8_t *)malloc(sizeof(uint8_t) * size);
    uint siz = size - 1;
    uint j = 0;
    new_arr[j++] = 0;
    for (uint i = 0; i < siz; i++)
    {
        new_arr[j++] = arr[i];
    }
    for (j = 0; j < size; j++)
    {
        arr[j] = new_arr[j];
    }
    free(new_arr);
}

void shiftArrDownThenAdd(uint8_t *arr, uint8_t size, uint8_t val)
{
    shiftArrDown(arr, size);
    arr[0] = val;
}

void shiftArrDown(uint64_t *arr, size_t size)
{
    uint64_t *new_arr = (uint64_t *)malloc(sizeof(uint64_t) * size);
    uint siz = size - 1;
    uint j = 0;
    new_arr[j++] = 0;
    for (uint i = 0; i < siz; i++)
    {
        new_arr[j++] = arr[i];
    }
    for (j = 0; j < size; j++)
    {
        arr[j] = new_arr[j];
    }
    free(new_arr);
}

void shiftArrDownThenAdd(uint64_t *arr, uint8_t size, uint64_t val)
{
    shiftArrDown(arr, size);
    arr[0] = val;
}