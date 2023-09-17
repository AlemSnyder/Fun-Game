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

#include <map>

#pragma once

namespace terrain {

namespace generation {

struct biome_json_data {
    std::string biome_name;
    Json::Value biome_data;
    Json::Value materials_data;
};

class GrassData {
 private:
    // vector that determines grass color from edge distance
    std::vector<ColorId> grass_colors_;
    // length of grass gradient
    uint8_t grass_grad_length_;
    // gradient index of grass not by an edge
    uint8_t grass_mid_;

 public:
    GrassData(const Json::Value& json_grass_data);

    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return grass_grad_length_;
    }

    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return grass_mid_;
    }

    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return grass_colors_;
    }
};

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

    // map of MapTile_t -> vector of TileMacro_t
    std::vector<std::vector<TileMacro_t>> macro_tile_types_;

    std::vector<AddToTop> add_to_top_generators_;

    // materials that exist
    std::map<MaterialId, const terrain::Material> materials_;

    GrassData grass_data_;

 public:
    Biome(const biome_json_data& biome_data);
    Biome(const std::string& biome_name) : Biome(get_json_data(biome_name)) {}


    inline const std::vector<MapTile_t>&
    get_map() const {
        return tile_map_vector_;
    }

    inline const LandGenerator&
    get_generator(TileMacro_t tile_macro_id) const {
        return land_generators_.at(tile_macro_id);
    }

    inline const std::vector<TileMacro_t>&
    get_macro_ids(MapTile_t tile_id_type) const {
        return macro_tile_types_[tile_id_type];
    }

    const std::vector<AddToTop>
    get_top_generators() const {
        return add_to_top_generators_;
    }

    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return grass_data_.get_grass_grad_length();
    }

    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return grass_data_.get_grass_mid();
    }

    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return grass_data_.get_grass_colors();
    }

    inline const std::map<MaterialId, const terrain::Material>&
    get_materials() const {
        return materials_;
    }

    std::map<ColorInt, std::pair<const Material*, ColorId>>
    get_colors_inverse_map() const;

 private:
    void read_tile_macro_data(const Json::Value& biome_data);

    void read_map_tile_data(const Json::Value& biome_data);

    void read_add_to_top_data(const Json::Value& biome_data);

    /**
     * @brief Load materials from json data
     *
     * @param material_data data to load from (see) data/materials.json
     */
    std::map<MaterialId, const terrain::Material>
    init_materials(const Json::Value& material_data);

    biome_json_data get_json_data(const std::string& biome_name);
};

} // namespace generation

} // namespace terrain
