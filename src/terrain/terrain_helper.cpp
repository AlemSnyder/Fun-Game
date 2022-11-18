#include "path/unit_path.hpp"
#include "terrain.hpp"
#include "tile.hpp"

#include <set>

namespace terrain {
namespace helper {

// grow_grass_recursive(set, int height)
// for tile in set
//      if tile grow data high < height
//          for adjacent tile (only if height != 0)
//              if this tile is grass, and grow_data_high < height-1
//                  add adjacent tiles to new set
//          set tile grass height to height
// if height then run grow_grass_recursive(new_set, height -1)

/*
used the grow the (grass gradient length - height)th iteration of grass
given a set of grass tiles gets the adjacent grass tiles that have height
equal to given hight.
Then next to those tiles set the grass height to hight-1 if this is higher
than the saved height.
*/
template <int getter(Tile*), void setter(Tile*, int)>
void
grow_grass_inner(Terrain& ter, std::set<Tile*> in_grass, int height) {
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
}

// grow_grass recursive(set)
// for tile in set
//      if tile is a source
//          add adjacent tiles to the new set if they are grass and something about
//          height set tile.grass height/source status
// run grow_grass recursive(new_set, grass_grad_length)
/*
used to grow the first iteration of of grass
given a set of all grass tiles gets the grass tiles that adjacent to an
edge.
Then next to those tiles set the grass height to max_hight-1.
*/
template <bool edge_detector(Tile*), int getter(Tile*), void setter(Tile*, int)>
void
grow_grass_recursive(Terrain& ter, std::set<Tile*> all_grass) {
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
}

} // namespace helper

} // namespace terrain