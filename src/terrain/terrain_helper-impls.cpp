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

//! This is an implementations file. Anything not implemented below will lead
//! to linker errors.

#include "terrain.hpp"
#include "terrain_helper.cpp"
#include "terrain_helper.hpp"
#include "tile.hpp"

#include <set>

namespace terrain {

// implementations of start of iteration
template void
helper::grow_grass_recursive<
    helper::edge_detector_low, helper::getter_low,
    helper::setter_low>(Terrain&, std::set<Tile*>);

template void
helper::grow_grass_recursive<
    helper::edge_detector_high, helper::getter_high,
    helper::setter_high>(Terrain&, std::set<Tile*>);

// implementations of inner recursive loop
template void
helper::grow_grass_inner<helper::getter_low, helper::setter_low>(
    Terrain&, std::set<Tile*>, int
);

template void
helper::grow_grass_inner<helper::getter_high, helper::setter_high>(
    Terrain&, std::set<Tile*>, int
);

} // namespace terrain
