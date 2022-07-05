#ifndef __TILE_STAMP_HPP__
#define __TILE_STAMP_HPP__

#include <set>
#include "material.hpp"

struct Tile_Stamp{
    int x_start;
    int y_start;
    int z_start;
    int x_end;
    int y_end;
    int z_end;

    const Material * mat;
    uint8_t color_id;
    std::set<std::pair<int, int>> elements_canstamp;
};

#endif
