#ifndef _AER_ARRAY_MANIPULATOR_H_
#define _AER_ARRAY_MANIPULATOR_H_

#include <stdio.h>

/// @brief Calculate the average value of an array of uint8_t values.
/// @param calls The array of values to average
/// @param count Count of the items in the array
/// @return The average value
uint8_t calcAverage(uint8_t *calls, int count);

/// @brief Calculate the average value of an array of uint64_t values.
/// @param calls The array of values to average
/// @param count Count of the items in the array
/// @return The average value
uint64_t calcAverage(uint64_t *calls, int count);

/// @brief Shifts elements in array down by one index, leaving index 0 empty.
/// @param arr The array to resize in place
/// @param size The size of the array for resizing
void shiftArrDown(uint8_t *arr, size_t size);

/// @brief Shifts elements in array down by one index, leaving index 0 empty.
/// @param arr The array to resize in place
/// @param size The size of the array for resizing
void shiftArrDown(uint64_t *arr, size_t size);

/// @brief Shifts elements in an array down by one index and adds given value to index 0.
/// @param arr The array to resize in place
/// @param size The size of the array for resizing
/// @param val The value to add to the array at index 0
void shiftArrDownThenAdd(uint8_t *arr, uint8_t size, uint8_t val);

/// @brief Shifts elements in an array down by one index and adds given value to index 0.
/// @param arr The array to resize in place
/// @param size The size of the array for resizing
/// @param val The value to add to the array at index 0
void shiftArrDownThenAdd(uint64_t *arr, uint8_t size, uint64_t val);

#endif