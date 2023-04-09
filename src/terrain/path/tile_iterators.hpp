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

#include "../../types.hpp"
#include "unit_path.hpp"

#include <cstdint>
#include <utility>

namespace terrain {

class Terrain;

namespace path {

// static uint8_t offsets[26];

TerrainDim3 get_indexed_offsets(uint8_t index);

class AdjacentIterator {
 private:
    const Terrain& parent_;
    const UnitPath path_type_constraint_;
    const size_t pos_;
    UnitPath path_type_;
    uint8_t dpos_;

    void update_path();
    void iterate_to_next_available();
    // Is the adjacent position in the bounds of the terrain
    bool is_valid_end_position();

 public:
    AdjacentIterator(const Terrain& parent, unsigned int xyz, UnitPath path_type);
    int operator++();
    int operator++(int);

    inline bool end() { return dpos_ > 26U; }

    size_t get_pos();
    UnitPath get_path_type();

    inline TerrainDim3 get_relative_position() {
        return get_indexed_offsets(dpos_);
    }
};

} // namespace path

} // namespace terrain
