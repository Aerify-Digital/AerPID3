#ifndef _AER_HEX_HELPER_H_
#define _AER_HEX_HELPER_H_

#include <iostream>
#include <sstream>
#include <iomanip>

// Function to convert uint8_t array to hex string
static std::string uint8ArrayToHexString(const uint8_t* byteArray) {
    std::ostringstream oss;
    
    // Set fixed width to 2 for each hexadecimal digit, and uppercase letters
    oss << std::hex << std::uppercase << std::setfill('0');
    
    for (size_t i = 0; i < 32; ++i) {
        // Convert each byte to its 2-digit hex representation
        oss << std::setw(2) << static_cast<int>(byteArray[i]);

        if (i < 32 - 1) {
            oss << "-"; // Add dash between bytes, but not after the last one
        }
    }
    
    return oss.str();
}

#endif