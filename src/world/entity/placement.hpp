#pragma once

#include "types.hpp"

namespace world {

namespace entity {

using Placement = glm::ivec4;

/*struct Placement {
    TerrainOffset3 position;
    uint8_t rotation;
};*/

struct PlacementOrder {
    size_t
    operator()(const Placement& pos) const noexcept {
        size_t result = 0;

        // Position
        utils::hash_combine(result, pos.x);
        utils::hash_combine(result, pos.y);
        utils::hash_combine(result, pos.z);

        // The last index is rotation, and this is irreverent to position.
        return result;
    }
};

} // namespace entity

} // namespace world
