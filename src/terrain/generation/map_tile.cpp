#include "map_tile.hpp"

#include "noise.hpp"

// a number chosen at random by the developer
#define RANDOM_NUMBER 7

namespace terrain {

namespace generation {

MapTile::MapTile(MapTile_t tile_type, size_t seed, MacroDim x, MacroDim y) :
    x_(x), y_(y), tile_type_(tile_type),
    rand_engine_(Noise::get_double((seed ^ tile_type) % RANDOM_NUMBER, x, y) * INT32_MAX) {}

// `tile_type % RANDOM_NUMBER` creates some randomness for the get double
// function. The first parameter is used to pick three prime numbers.

} // namespace generation

} // namespace terrain
