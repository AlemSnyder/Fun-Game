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
 * @file tile_iterators.cpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines tile iterators classes
 *
 * @ingroup terrain::path
 *
 */

#include "../terrain.hpp"
#include "types.hpp"
#include "unit_path.hpp"

#include <cstdint>
#include <utility>

namespace terrain {

namespace path {

/* This is a look up table for possible offsets.
 * We have 26 offsets because 3^3-1 = 26.
 * The total number of adjacent cubes is the number cubes in a three by three
 * area minus one for the central cube.
 */
static uint8_t offsets[26] = {
    0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000101, 0b00000110, 0b00001000,
    0b00001001, 0b00001010, 0b00010000, 0b00010001, 0b00010010, 0b00010100, 0b00010110,
    0b00011000, 0b00011001, 0b00011010, 0b00100000, 0b00100001, 0b00100010, 0b00100100,
    0b00100101, 0b00100110, 0b00101000, 0b00101001, 0b00101010,
};

glm::i8vec3
get_indexed_offsets(uint8_t index) {
    uint8_t offset_type = offsets[index];
    glm::i8vec3 out(
        offset_type & int8_t(3), offset_type >> 2 & int8_t(3),
        offset_type >> 4 & int8_t(3)
    );
    return out - glm::i8vec3(1, 1, 1);
}

AdjacentIterator::AdjacentIterator(
    const Terrain& parent, TerrainOffset3 xyz, UnitPath path_type
) :
    parent_(parent),
    path_type_constraint_(path_type), pos_(xyz), path_type_(0), dpos_(0) {
    update_path();
    if (!path_type_.compatible(path_type_constraint_) || !is_valid_end_position()) {
        iterate_to_next_available();
    }
}

int
AdjacentIterator::operator++() {
    iterate_to_next_available();
    return dpos_;
}

int
AdjacentIterator::operator++(int) {
    iterate_to_next_available();
    return dpos_;
}

void
AdjacentIterator::update_path() {
    auto offset = get_relative_position();

    path_type_ = parent_.get_path_type(pos_, pos_ + TerrainOffset3(offset));
}

bool
AdjacentIterator::is_valid_end_position() const {
    auto offset = get_relative_position();

    return parent_.in_range(pos_ + TerrainOffset3(offset));
}

void
AdjacentIterator::iterate_to_next_available() {
    do {
        dpos_++;
        if (end())
            return;
        update_path();
    } while (!path_type_.compatible(path_type_constraint_) || !is_valid_end_position());
    // If the path is not compatible with the path type or
    // the final position is not in bounds then continue iterating
}

} // namespace path

} // namespace terrain
