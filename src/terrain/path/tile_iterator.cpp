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

#include <cstdint>
#include <utility>
#include "../terrain.hpp"
#include "unit_path.hpp"

namespace terrain{

namespace path{

uint8_t offsets[26] = {0b00000000,
                       0b00000001,
                       0b00000010,
                       0b00000100,
                       0b00000101,
                       0b00000110,
                       0b00001000,
                       0b00001001,
                       0b00001010,
                       0b00010000,
                       0b00010001,
                       0b00010010,
                       0b00010100,
                       0b00010110,
                       0b00011000,
                       0b00011001,
                       0b00011010,
                       0b00100000,
                       0b00100001,
                       0b00100010,
                       0b00100100,
                       0b00100101,
                       0b00100110,
                       0b00101000,
                       0b00101001,
                       0b00101010 };

std::array<int8_t, 3> get_indexed_offsets(uint8_t index){
    std::array<int8_t, 3> out;
    uint8_t offset_type = offsets[index];
    out[0] = (offset_type      & int8_t(3)) - 1;
    out[1] = (offset_type >> 2 & int8_t(3)) - 1;
    out[2] = (offset_type >> 4 & int8_t(3)) - 1;
    return out;
}


AdjacentIterator::AdjacentIterator(const Terrain& parent, unsigned int xyz, UnitPath path_type) : parent_(parent){
    pos_ = xyz;
    path_type_constraint_ = path_type;
    //parent_ = parent;

}

int AdjacentIterator::operator++(){
    dpos_++;
    update_path();
    return dpos_;
}

int AdjacentIterator::operator++(int){
    dpos_++;
    update_path();
    return dpos_;
}

bool AdjacentIterator::end(){
    return true; // not implemented
}

inline std::array<int8_t, 3> AdjacentIterator::get_relative_position(){
    return get_indexed_offsets(dpos_);
}

void AdjacentIterator::update_path(){
    auto [xs, ys, zs] = parent_.sop(pos_);
    auto [dx, dy, dz] = get_relative_position();

    path_type_ = parent_.get_path_type(xs, ys, zs, xs+dx, ys+dy, zs+dz);
}

size_t AdjacentIterator::get_pos(){
    auto [xs, ys, zs] = parent_.sop(pos_);
    auto [dx, dy, dz] = get_relative_position();
    size_t pos = parent_.pos(xs+dx, ys+dy, zs+dz);
    return pos;
}

} // namespace path

} // namespace terrain