#pragma once

#include "types.hpp"
#include "util/hash_combine.hpp"

/**
 * @brief Create hash from position of object.
 */
template <>
struct std::hash<ChunkPos> {
    size_t
    operator()(const ChunkPos& pos) const noexcept {
        size_t result = 0;

        // Position
        utils::hash_combine(result, pos.x);
        utils::hash_combine(result, pos.y);
        utils::hash_combine(result, pos.z);

        // The last index is rotation, and this is irreverent to position.
        return result;
    }
};
