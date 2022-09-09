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
 * @ingroup Terrain
 *
 */

#include <cstdint>
#include <utility>
#include "terrain.hpp"
#include "unit_path.hpp"

#pragma once

namespace terrain{

class AdjacentIterator {
private:
    size_t pos_;
    uint8_t dpos_;
    UnitPath path_type_;
    Terrain& parent_;

    void update_path();

    static int8_t offsets[26];

public:
    AdjacentIterator(Terrain& parent, unsigned int xyz, UnitPath path_type);
    //AdjacentIterator();
    int operator++();
    std::pair<size_t, UnitPath> get();
    std::array<int8_t, 3> get_offsets(uint8_t index);
};

AdjacentIterator::AdjacentIterator(Terrain& parent, unsigned int xyz, UnitPath path_type) : parent_(parent){
    pos_ = xyz;
    path_type_ = path_type;
    //parent_ = parent;

}

int AdjacentIterator::operator++(){
    dpos_++;
    update_path();
}

std::array<int8_t, 3> AdjacentIterator::get_offsets(uint8_t index){
    std::array<int8_t, 3> out;
    auto x = int8_t(3);
    out[0] = offsets[index] & x - 1;
    out[1] = offsets[index] >> 2 & x - 1;
    out[2] = offsets[index] >> 4 & x - 1;
    return out;
}

void AdjacentIterator::update_path(){
    auto [xs, ys, zs] = parent_.sop(pos_);
    auto [dx, dy, dz] = get_offsets(dpos_);

    path_type_ = parent_.get_path_type(xs, ys, zs, xs+dx, ys+dy, zs+dz);
}

//AdjacentIterator::AdjacentIterator() {

//}

}
