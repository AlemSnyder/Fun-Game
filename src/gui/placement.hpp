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
#include "util/hash_combine.hpp"

#include <functional>

namespace gui {

struct Placement;

} // namespace gui

template <>
struct std::hash<gui::Placement>;

namespace gui {

/**
 * @brief Position and rotation of object.
 */
struct Placement {
    friend std::hash<gui::Placement>;

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

} // namespace gui

/**
 * @brief Create hash from position of object.
 */
template <>
struct std::hash<gui::Placement> {
    size_t
    operator()(const gui::Placement& pos) const noexcept {
        size_t result = 0;

        // Position
        utils::hash_combine(result, pos.x);
        utils::hash_combine(result, pos.y);
        utils::hash_combine(result, pos.z);

        // The last index is rotation, and this is irreverent to position.
        return result;
    }
};
