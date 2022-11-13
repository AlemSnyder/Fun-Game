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

#include <set>


namespace terrain {
namespace helper {

/**
 * @brief  * @brief Recursive function that sets the grass gradient growth color. This 
 * part is inside the iteration
 * 
 * @warning This function should not be called.
 *
 * @tparam getter(Tile*) get the paramater that will be set (grow data high/low)
 * @tparam setter(Tile*, int) set the paramater (grow data high/low)
 * @param ter terrain to do operation on
 * @param in_grass set of all grass at the next height level
 * @param height level of grass gradient being set during this iteration.
 */
template <int getter(Tile*), void setter(Tile*, int)>
void
grow_grass_inner(Terrain& ter, std::set<Tile*> in_grass, int height); /*{
    // height == 1 this is the end of recursion. Tile is default set to 0
    if (height == 1) {
        return;
    }
    // set of tiles that are of height - 2
    // the next level down
    std::set<Tile*> next_grass_tiles;
    for (Tile* tile : in_grass) {
        auto it = ter.get_tile_adjacent_iterator(
            ter.pos(tile), DirectionFlags::HORIZONTAL1 | DirectionFlags::HORIZONTAL2
        );
        for (; !it.end(); it++) {
            uint16_t pos = it.get_pos();
            Tile* adjacent_tile = ter.get_tile(pos);
            // instead of height used maxheight +1
            if (adjacent_tile->is_grass() && (getter(adjacent_tile) < height)) {
                // if adjacent tile is on the level below
                // it should be added to the next iteration
                next_grass_tiles.insert(adjacent_tile);
                // and the tiles with height less should have their height set
                setter(adjacent_tile, height);
            }
        }
    }
    grow_grass_inner<getter, setter>(ter, next_grass_tiles, height - 1);
}*/

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
void
grow_grass_recursive(Terrain& ter, std::set<Tile*> all_grass);/* {
    // set of tiles that are of adjacent to an edge
    // the first level
    std::set<Tile*> next_grass_tiles;
    // maximum value for grass gradient
    int max_grass = ter.get_grass_grad_length() - 1;
    for (Tile* tile : all_grass) {
        // is the tile and edge
        bool is_source = false;
        auto it = ter.get_tile_adjacent_iterator(
            ter.pos(tile), DirectionFlags::HORIZONTAL1 | DirectionFlags::HORIZONTAL2
        );
        for (; !it.end(); it++) {
            Tile* adjacent_tile = ter.get_tile(it.get_pos());
            // (in some cases: not solid, and others: solid and not grass)
            if (edge_detector(adjacent_tile)) {
                is_source = true;
                break;
            }
        }
        if (is_source) {
            // set the tile grass index to max_grass
            setter(tile, max_grass);
            // tile->grow_sink should be set to true
            // add it to the next iteration
            next_grass_tiles.insert(tile);
        }
    }
    grow_grass_inner<getter, setter>(ter, next_grass_tiles, max_grass - 1);
}*/

/*inline static void
grow_grass_recursive_low(Terrain& ter, std::set<Tile*> all_grass) {
    helper::grow_grass_recursive<
        helper::edge_detector_low, helper::getter_low, helper::setter_low>(
        ter, all_grass
    );
}

inline static void
grow_grass_recursive_high(Terrain& ter, std::set<Tile*> all_grass) {
    helper::grow_grass_recursive<
        helper::edge_detector_high, helper::getter_high, helper::setter_high>(
        ter, all_grass
    );
}*/

} // namespace helper

} // namespace terrain
