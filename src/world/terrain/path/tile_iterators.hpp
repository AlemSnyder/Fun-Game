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
 * @file tile_iterators.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines tile iterators classes
 *
 * @ingroup terrain::path
 *
 */

#pragma once

#include "types.hpp"
#include "unit_path.hpp"

#include <cstdint>
#include <utility>

#define NUMBER_OF_BORDER_CUBES 26U

// 3^3 - 1 = 26
// Because Nino keeps forgetting, If you have a cube in 3D space, and want to
// find the number of cubes at are near it. Not just touching (6), but all of
// of them

namespace terrain {

class Terrain;

namespace path {

// static uint8_t offsets[26];

glm::i8vec3 get_indexed_offsets(uint8_t index);

class AdjacentIterator {
 private:
    const Terrain& parent_;
    const UnitPath path_type_constraint_;
    const TerrainOffset3 pos_;
    UnitPath path_type_;
    uint8_t dpos_;

    void update_path();
    void iterate_to_next_available();
    // Is the adjacent position in the bounds of the terrain
    [[nodiscard]] bool is_valid_end_position() const;

 public:
    AdjacentIterator(const Terrain& parent, TerrainOffset3 xyz, UnitPath path_type);
    int operator++();
    int operator++(int);

    [[nodiscard]] inline bool
    end() const {
        return dpos_ > NUMBER_OF_BORDER_CUBES;
    }

    [[nodiscard]] inline TerrainOffset3
    get_pos() {
        return pos_ + TerrainOffset3(dpos_);
    }


    [[nodiscard]] inline UnitPath
    get_path_type() const {
        return path_type_;
    }

    [[nodiscard]] inline glm::i8vec3
    get_relative_position() const {
        return get_indexed_offsets(dpos_);
    }
};

} // namespace path

} // namespace terrain
