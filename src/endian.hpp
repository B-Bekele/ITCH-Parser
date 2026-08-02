#pragma once

#include <cstdint>
#include <cstring>

#ifdef _MSC_VER
#include <stdlib.h>
inline uint16_t to_host(uint16_t be) { return _byteswap_ushort(be); }
inline uint32_t to_host(uint32_t be) { return _byteswap_ulong(be); }
inline uint64_t to_host(uint64_t be) { return _byteswap_uint64(be); }
#else
inline uint16_t to_host(uint16_t be) { return __builtin_bswap16(be); }
inline uint32_t to_host(uint32_t be) { return __builtin_bswap32(be); }
inline uint64_t to_host(uint64_t be) { return __builtin_bswap64(be); }
#endif

// ITCH timestamps are 6 bytes big-endian, nanoseconds since midnight
inline uint64_t read_timestamp(const uint8_t ts[6]) {
    return (static_cast<uint64_t>(ts[0]) << 40) | (static_cast<uint64_t>(ts[1]) << 32) |
           (static_cast<uint64_t>(ts[2]) << 24) | (static_cast<uint64_t>(ts[3]) << 16) |
           (static_cast<uint64_t>(ts[4]) << 8) | static_cast<uint64_t>(ts[5]);
}
