#include "terrain_helper.hpp"
#include "terrain.hpp"
#include "tile.hpp"

namespace terrain {
namespace helper {

Tile* Ninos_horrible_function(Terrain& ter, Tile*tile, int x, int y){
    if (x == 0 && y == 0) {
        return nullptr;
    }
    // safety function to test if you xyz is in range;
    if (!ter.in_range(tile->get_x() + x, tile->get_y() + y, tile->get_z())) {
        return nullptr;
    }
    Tile* adjacent_tile =
        ter.get_tile(tile->get_x() + x, tile->get_y() + y, tile->get_z());
    return adjacent_tile;
}

} // namespace helper

} // namespace terrain
