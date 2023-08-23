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
 * @file biome.hpp
 *
 * @brief Defines Biome class.
 *
 * @ingroup Terrain Terrain_generation
 *
 */

#include "../../util/files.hpp"
#include "land_generator.hpp"

#pragma once

namespace terrain {

namespace terrain_generation {

class Biome {
 private:
    std::vector<int> tile_map_vector_;
    // std::vector<LandGenerator> land_generators_;
    //  vector of const data for creating land generators for every tile

    // std::vector<MacroTile> map_tiles_
    // 32 by 32 chunks that load data
    // each one will have a mutex lock so this can be threaded

 public:
    Biome(std::string biome_json_path);
    inline std::vector<int> get_tile_vector(){
        return tile_map_vector_;
    }
};

} // namespace terrain_generation

} // namespace terrain
