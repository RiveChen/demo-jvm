/**
 * @file endian.hpp
 * @brief Endianness conversion functions for reading big-endian class files.
 *
 * The JVM class file format uses big-endian (network byte order).
 * This module provides platform-independent byte swapping via
 * compiler intrinsics.
 *
 * @note Most modern machines are little-endian (x86, ARM). This module
 * assumes we are reading from a big-endian stream into a little-endian
 * host. For this learning project the assumption is safe.
 *
 */
#pragma once

#include "types.hpp"

// Using byte order swap macros for different compilers
#if defined(_MSC_VER)
#include <cstdlib>
#define BSWAP16(x) _byteswap_ushort(x)
#define BSWAP32(x) _byteswap_ulong(x)
#define BSWAP64(x) _byteswap_uint64(x)
#elif defined(__GNUC__) || defined(__clang__)
#define BSWAP16(x) __builtin_bswap16(x)
#define BSWAP32(x) __builtin_bswap32(x)
#define BSWAP64(x) __builtin_bswap64(x)
#else
// A fallback implementation for other compilers
inline U2 BSWAP16(U2 val) { return (val << 8) | (val >> 8); }
inline U4 BSWAP32(U4 val) {
  val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
  return (val << 16) | (val >> 16);
}
inline U8 BSWAP64(U8 val) {
  val = ((val << 8) & 0xFF00FF00FF00FF00ULL) | ((val >> 8) & 0x00FF00FF00FF00FFULL);
  val = ((val << 16) & 0xFFFF0000FFFF0000ULL) | ((val >> 16) & 0x0000FFFF0000FFFFULL);
  return (val << 32) | (val >> 32);
}
#endif

namespace jvm {

/**
 * @brief Swap byte order of a multi-byte value, or leave 1-byte values
 * unchanged.
 *
 * Uses C++17 `if constexpr` to dispatch to the correct BSWAP intrinsic
 * at compile time based on the type size.
 *
 * @tparam T A scalar type (typically U2, U4, U8, or their signed equivalents).
 * @param value The value to convert from big-endian to host endianness (or vice
 * versa).
 * @return The byte-swapped value, or the original unchanged for single-byte
 * types.
 */
template <typename T>
T swapEndian(T value) {
  if constexpr (sizeof(T) == sizeof(U1)) {
    return value;  // 1-byte values don't need swapping
  } else if constexpr (sizeof(T) == sizeof(U2)) {
    return BSWAP16(value);
  } else if constexpr (sizeof(T) == sizeof(U4)) {
    return BSWAP32(value);
  } else if constexpr (sizeof(T) == sizeof(U8)) {
    return BSWAP64(value);
  }
  // For any other size, just return the value unchanged.
  return value;
}

}  // namespace jvm