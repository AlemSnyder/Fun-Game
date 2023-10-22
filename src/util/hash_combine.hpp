#pragma once

#include <functional>

namespace utils {

/**
 * @brief Boost hash combine hash algorithm.
 * 
 * @details Modifies seed depending on the value v given.
*/
template <class T>
inline void
hash_combine(std::size_t& seed, const T& v) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
};

} // namespace utils
