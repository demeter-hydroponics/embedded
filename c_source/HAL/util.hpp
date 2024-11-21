#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdint.h>

// Function to swap endianness of uint16_t
inline uint16_t swap_endianness(uint16_t value) { return (value >> 8) | (value << 8); }

// Function to swap endianness of uint32_t
inline uint32_t swap_endianness(uint32_t value) {
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00) | ((value << 8) & 0x00FF0000) |
           ((value << 24) & 0xFF000000);
}

// Function to swap endianness of uint64_t
inline uint64_t swap_endianness(uint64_t value) {
    return ((value >> 56) & 0x00000000000000FF) | ((value >> 40) & 0x000000000000FF00) | ((value >> 24) & 0x0000000000FF0000) |
           ((value >> 8) & 0x00000000FF000000) | ((value << 8) & 0x000000FF00000000) | ((value << 24) & 0x0000FF0000000000) |
           ((value << 40) & 0x00FF000000000000) | ((value << 56) & 0xFF00000000000000);
}

// add an IGNORE macro to ignore a variable
#define IGNORE(x) (void)(x)

#endif  // UTIL_HPP