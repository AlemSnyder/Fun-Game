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
 * @file tilestamp.hpp
 *
 * @brief Defines Tile_stamp struct.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "../material.hpp"

#include <set>

namespace terrain {

namespace terrain_generation {

/**
 * @brief Holds data used to set the material and color of a rectangular prism
 * in terrain.
 *
 * @details TileStamp holds 6 ints to define the start and end of a
 * rectangular prism in 3D space. TileStamp also contains the material
 * pointer, and color ID of the final tile. The types of tiles that can be
 * changed are also saved
 */
struct TileStamp {
    int x_start; // Lower bound in the x direction.
    int y_start; // Lower bound in the y direction.
    int z_start; // Lower bound in the z direction.
    int x_end;   // Upper bound in the x direction.
    int y_end;   // Upper bound in the y direction.
    int z_end;   // Upper bound in the z direction.

    const Material* mat; // Material that tiles will be set to.
    Color_id_t color_id;    // color that tiles will be set to.
    // set of <material ID, color ID> determines what tiles types can be changed.
    std::set<std::pair<Material_id_t, Color_id_t>> elements_can_stamp;
};

} // namespace terrain_generation

} // namespace terrain
