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
 * @ingroup Terrain generation
 *
 */

#include "../../util/files.hpp"
#include "land_generator.hpp"

#pragma once

namespace terrain {

namespace generation {

class Biome {
 private:
    std::vector<MapTile_t> tile_map_vector_;
    // std::vector<LandGenerator> land_generators_;
    //  vector of const data for creating land generators for every tile

    // std::vector<MacroTile> map_tiles_
    // 32 by 32 chunks that load data
    // each one will have a mutex lock so this can be threaded

    // create a map of TileMacro_t -> LandGenerator
    std::vector<generation::LandGenerator> land_generators_;

    // map of TileMap_t -> vector of TileMacro_t
    std::vector<std::vector<TileMacro_t>> macro_tile_types_;

    void
    read_tile_macro_data(const Json::Value& biome_data);

    void
    read_map_tile_data(const Json::Value& biome_data);

 public:
    Biome(const std::string& biome_json_path);

    inline const std::vector<MapTile_t>&
    get_tile_vector() {
        return tile_map_vector_;
    }
};

} // namespace generation

} // namespace terrain
