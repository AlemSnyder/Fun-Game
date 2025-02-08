// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file terrain_helper.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Terrain adjacent helper functions
 *
 * @ingroup terrain::helper
 *
 */
#pragma once

#include "terrain.hpp"
#include "tile.hpp"

#include <unordered_set>

namespace terrain {
namespace helper {

/**
 * @brief Recursive function that sets the grass gradient growth color. This
 * part is inside the iteration
 *
 * @warning This function should not be called.
 * @tparam getter(Tile*) get the paramater that will be set (grow data high/low)
 * @tparam setter(Tile*, int) set the paramater (grow data high/low)
 * @param ter terrain to do operation on
 * @param in_grass set of all grass at the next height level
 * @param height level of grass gradient being set during this iteration.
 */
template <int getter(Tile*), void setter(Tile*, int)>
void
grow_grass_inner(Terrain& ter, std::unordered_set<TerrainOffset3> in_grass, int height);

/**
 * @brief Recursive function that sets the grass gradient growth color.
 *
 * @tparam edge_detector(Tile*)
 * @tparam getter(Tile*) get the paramater that will be set (grow data high/low)
 * @tparam setter(Tile*, int) set the paramater (grow data high/low)
 * @param ter terrain to do operation on
 * @param all_grass set of all grass in terrain
 */
template <bool edge_detector(Tile*), int getter(Tile*), void setter(Tile*, int)>
void grow_grass_recursive(Terrain& ter, std::unordered_set<TerrainOffset3> all_grass);

} // namespace helper

} // namespace terrain
