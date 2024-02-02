#pragma once

#include "types.hpp"

namespace world {

namespace entity {

struct Placement {
    TerrainOffset3 position;
    uint8_t rotation;
};

} // namespace entity

} // namespace world
