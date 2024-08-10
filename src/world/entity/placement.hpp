// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file placement.hpp
 *
 * @brief Defines Placement class.
 *
 * @ingroup World Entity
 *
 */

#pragma once

#include "types.hpp"

namespace world {

namespace entity {

/**
 * @brief Position and rotation of object.
 */
class Placement {
 public:
    Dim x;
    Dim y;
    Dim z;

    uint8_t rotation;

    uint8_t texture_id; // texture used the render object

    [[nodiscard]] inline glm::ivec4
    as_vec() const {
        return glm::ivec4(x, y, z, rotation);
    }

    [[nodiscard]] inline bool
    operator==(const Placement& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }
};

/**
 * @brief Create has from position of object.
 */
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
