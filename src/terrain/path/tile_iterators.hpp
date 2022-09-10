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

#include <cstdint>
#include <utility>
#include "../terrain.hpp"
#include "unit_path.hpp"

#pragma once

namespace terrain{

class Terrain;

namespace path{

//static uint8_t offsets[26];

std::array<int8_t, 3> get_indexed_offsets(uint8_t index);

class AdjacentIterator {
private:
    const size_t pos_;
    uint8_t dpos_;
    const UnitPath path_type_constraint_;
    UnitPath path_type_;
    const Terrain& parent_;

    void update_path();
    void iterate_to_next_available();

public:
    AdjacentIterator(const Terrain& parent, unsigned int xyz, UnitPath path_type);
    //AdjacentIterator();
    int operator++();
    int operator++(int);
    inline bool end(){
        return dpos_ > 26U;
    }
    size_t get_pos();
    UnitPath get_path_type();
    inline std::array<int8_t, 3> get_relative_position(){
        return get_indexed_offsets(dpos_);
    }
    
};

} // namespace path

} // namespace terrain
