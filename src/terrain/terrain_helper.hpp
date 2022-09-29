// TODO comments

#include "tile.hpp"
#include "terrain.hpp"
#include <set>

namespace terrain
{
namespace helper{

// grow_grass recursive(set)
// for tile in set
//      if tile is a source
//          add adjacent tiles to the new set if they are grass and something about height
//          set tile.grass height/source status
// run grow_grass recursive(new_set, grass_grad_length)
template<bool edge_detector(Tile*), int getter(Tile*), void setter(Tile*, int)>
void grow_grass_recursive(Terrain& ter, std::set<Tile*> all_grass){
    std::set<Tile*> next_grass_tiles;
    for (Tile* tile : all_grass){
        bool is_source = false;
        for (int x = -1; x < 2; x++)
        for (int y = -1; y < 2; y++) {
            if (x == 0 && y == 0) { continue; }
            // safety function to test if you xyz is in range;
            if (ter.in_range(tile->get_x() + x, tile->get_y() + y, tile->get_z())) {
                Tile* adjacent_tile =
                    ter.get_tile(tile->get_x() + x, tile->get_y() + y, tile->get_z());
                if (edge_detector(adjacent_tile)) {
                    is_source = true;
                    break;
                }
            }
        }
        if (is_source){
            for (int x = -1; x < 2; x++)
            for (int y = -1; y < 2; y++) {
                if (x == 0 && y == 0) { continue; }
                // safety function to test if you xyz is in range;
                if (ter.in_range(tile->get_x() + x, tile->get_y() + y, tile->get_z())) {
                    Tile* adjacent_tile =
                        ter.get_tile(tile->get_x() + x, tile->get_y() + y, tile->get_z());
                    if (adjacent_tile->is_grass()
                            & (getter(adjacent_tile)
                                < ter.get_grass_grad_length() - 1)) {
                        setter(adjacent_tile, ter.get_grass_grad_length() - 1);
                    }
                }
            }
            setter(tile, ter.get_grass_grad_length());
            // tile->grow_sink should be set to true
            next_grass_tiles.insert(tile);
        }
    }
    grow_grass_recursive<getter, setter>(ter, next_grass_tiles, ter.get_grass_grad_length() - 1);
}

// grow_grass_recursive(set, int height)
// for tile in set
//      if tile grow data high < height
//          for adjacent tile (only if height != 0)
//              if this tile is grass, and grow_data_high < height-1
//                  add adjacent tiles to new set
//          set tile grass height to height
// if height then run grow_grass_recursive(new_set, height -1)
template<int getter(Tile*), void setter(Tile*, int)>
void grow_grass_recursive(Terrain& ter, std::set<Tile*> in_grass, int height){
    if (height == 1) { return; }
    std::set<Tile*> next_grass_tiles;
    for (Tile* tile : in_grass){
        for (int x = -1; x < 2; x++)
        for (int y = -1; y < 2; y++) {
            if (x == 0 && y == 0) { continue; }
            // safety function to test if you xyz is in range;
            if (ter.in_range(tile->get_x() + x, tile->get_y() + y, tile->get_z())) {
                Tile* adjacent_tile = ter.get_tile(tile->get_x() + x, tile->get_y() + y, tile->get_z());
                //! depends on/low
                if (adjacent_tile->is_grass() & (getter(adjacent_tile) == height)) {
                    next_grass_tiles.insert(adjacent_tile);
                    for (int xn = -1; xn < 2; xn++)
                    for (int yn = -1; yn < 2; yn++) {
                        if (xn == 0 && yn == 0) { continue; }
                        // safety function to test if you xyz is in range;
                        if (ter.in_range(adjacent_tile->get_x() + xn, adjacent_tile->get_y() + yn, adjacent_tile->get_z())) {
                            Tile* adjacent_tile_second = ter.get_tile(adjacent_tile->get_x() + xn, adjacent_tile->get_y() + yn, adjacent_tile->get_z());
                            //! depends on/low
                            if (adjacent_tile_second->is_grass() & (getter(adjacent_tile_second) < height-1)) {
                                //! depends on/low
                                setter(adjacent_tile_second, height - 1);
                            }
                        }
                    }
                }
            }
        }
    }
    //! depends on/low (just pass functions)
    grow_grass_recursive<getter, setter>(ter, next_grass_tiles, height - 1);
}

inline bool edge_detector_high(Tile* t){
    return !t->is_grass() && t->is_solid();
}

inline void setter_high(Tile* t, int set_to){
    t->set_grow_data_high(set_to);
}

inline int getter_high(Tile* t){
    return t->get_grow_data_high();
}

inline bool edge_detector_low(Tile* t){
    return !t->is_solid();
}

inline void setter_low(Tile* t, int set_to){
    t->set_grow_data_low(set_to);
}

inline int getter_low(Tile* t){
    return t->get_grow_data_low();
}

} // namespace helper

} // namespace terrain
