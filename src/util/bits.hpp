#pragma once

#include <cstdint>

namespace bits {

// Favor compiler builtins as they are CPU-specific
#if defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)

static inline uint32_t
swap(uint32_t x) noexcept
{
    return __builtin_bswap32(x);
}

static inline uint64_t
swap(uint64_t x) noexcept
{
    return __builtin_bswap64(x);
}

#elif defined(_WIN32)
#  include <intrin.h>

static inline uint32_t
swap(uint32_t x) noexcept
{
    return _byteswap_ulong(x);
}

static inline uint64_t
swap(uint64_t x) noexcept
{
    return _byteswap_uint64(x);
}

#elif defined(__FreeBSD__) || defined(__DragonFly__) || defined(__NetBSD__)
#  include <sys/endian.h>

static inline uint32_t
swap(uint32_t x) noexcept
{
    return bswap32(x);
}

static inline uint64_t
swap(uint64_t x) noexcept
{
    return bswap64(x);
}

#elif defined(__APPLE__)
#  include <libkern/OSByteOrder.h>

static inline uint32_t
swap(uint32_t x) noexcept
{
    return OSSwapInt32(x);
}

static inline uint64_t
swap(uint64_t x) noexcept
{
    return OSSwapInt64(x);
}

#elif defined(__linux__)
#  include <byteswap.h>

static inline uint32_t
swap(uint32_t x) noexcept
{
    return bswap_32(x);
}

static inline uint64_t
swap(uint64_t x) noexcept
{
    return bswap_64(x);
}

#else

static inline uint32_t
swap(uint32_t x) noexcept
{
    // clang-format off
    return ((x & 0xff000000) >> 24) |
           ((x & 0x00ff0000) >> 8)  |
           ((x & 0x0000ff00) << 8)  |
           ((x & 0x000000ff) << 24);
    // clang-format on
}

static inline uint64_t
swap(uint64_t x) noexcept
{
    // https://github.com/Cyan4973/xxHash/issues/227
    // clang-format off
    return ((x << 56) & 0xff00000000000000ULL) |
           ((x << 40) & 0x00ff000000000000ULL) |
           ((x << 24) & 0x0000ff0000000000ULL) |
           ((x << 8)  & 0x000000ff00000000ULL) |
           ((x >> 8)  & 0x00000000ff000000ULL) |
           ((x >> 24) & 0x0000000000ff0000ULL) |
           ((x >> 40) & 0x000000000000ff00ULL) |
           ((x >> 56) & 0x00000000000000ffULL);
    // clang-format on
}

#endif

} // namespace bits
