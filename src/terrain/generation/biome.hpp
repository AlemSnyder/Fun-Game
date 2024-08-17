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
#include "map_tile.hpp"
#include "terrain_genreration_types.hpp"

#include <sol/sol.hpp>

#include <map>

#pragma once

namespace terrain {

namespace generation {

struct biome_json_data {
    // Name of biome. Used both for file name and display name
    std::string biome_name;
    // Json data that describes biome
    biome_data_t biome_data;
    // Json data that describes materials
    all_materials_t materials_data;
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
    /**
     * @brief Construct a new GrassData ojbect
     *
     * @param json_grass_data Json data that describes grass gradient
     */
    GrassData(const grass_data_t& json_grass_data);

    GrassData(const std::optional<grass_data_t>& json_grass_data);

    /**
     * @brief Get the grass gradient length
     *
     * @return length of grass gradient
     */
    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return grass_grad_length_;
    }

    /**
     * @brief Get index where grass gradient splits
     *
     * @return grass mid point
     */
    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return grass_mid_;
    }

    /**
     * @brief Get grass gradient
     *
     * @return vector of grass color ids
     */
    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return grass_colors_;
    }
};

class Biome {
 private:
    //  vector of const data for creating land generators for every tile
    // create a map of TileMacro_t -> LandGenerator
    std::vector<generation::LandGenerator> land_generators_;

    // map of MapTile_t -> vector of TileMacro_t
    std::vector<std::vector<TileMacro_t>> macro_tile_types_;

    std::vector<AddToTop> add_to_top_generators_;

    // materials that exist
    const std::map<MaterialId, const terrain::Material> materials_;

    GrassData grass_data_;

    // lua state that generates macro map
    sol::state lua_;

    size_t seed_;

 public:
    /**
     * @brief Construct a new Biome object
     *
     * @param biome_data data containing biome_data material data and biome name
     */
    Biome(biome_json_data biome_data, size_t seed);

    /**
     * @brief Construct a new Biome object
     *
     * @param biome_name name of biome
     */
    Biome(const std::string& biome_name, size_t seed) :
        Biome(get_json_data(biome_name), seed) {}

    /**
     * @brief Get macro tile map
     *
     * @param length side length of square map
     *
     * @return 2D map of map tiles
     */
    [[nodiscard]] const TerrainMacroMap get_map(MacroDim length) const;

    inline static TerrainMacroMap
    single_tile_type_map(MapTile_t type) {
        std::vector<terrain::generation::MapTile> out;
        out.reserve(9);
        for (size_t i = 0; i < 4; i++)
            out.emplace_back(0, 0);
        out.emplace_back(type, 2);
        for (size_t i = 0; i < 4; i++)
            out.emplace_back(0, 0);

        return TerrainMacroMap(out, 3, 3);
    }

    /**
     * @brief Get land generator from TileMacro_t
     *
     * @return land_generator
     */
    [[nodiscard]] inline const LandGenerator&
    get_generator(TileMacro_t tile_macro_id) const {
        return land_generators_.at(tile_macro_id);
    }

    /**
     * @brief Get TileMacros corrsponding to a tile map type
     *
     * @return vector of TileMacro_t used to generate terrain on given MapTile_t
     */
    [[nodiscard]] inline const std::vector<TileMacro_t>&
    get_macro_ids(MapTile_t tile_id_type) const {
        return macro_tile_types_[tile_id_type];
    }

    /**
     * @brief Get the top generators
     *
     * @return add_to_top_generators_
     */
    [[nodiscard]] const std::vector<AddToTop>
    get_top_generators() const {
        return add_to_top_generators_;
    }

    /**
     * @brief Get the grass gradient length
     *
     * @return length of grass gradient
     */
    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return grass_data_.get_grass_grad_length();
    }

    /**
     * @brief Get index where grass gradient splits
     *
     * @return grass mid point
     */
    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return grass_data_.get_grass_mid();
    }

    /**
     * @brief Get grass colors
     *
     * @return vector of grass color ids
     */
    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return grass_data_.get_grass_colors();
    }

    /**
     * @brief Get materials map
     *
     * @return materials_ map of MaterialId to material
     */
    [[nodiscard]] inline const std::map<MaterialId, const terrain::Material>&
    get_materials() const {
        return materials_;
    }

    /**
     * @brief Get material pointer if it exists else null pointer
     *
     * @param mat_id MaterialId id of material
     *
     * @return pointer to corresponding material
     */
    [[nodiscard]] inline const terrain::Material*
    get_material(MaterialId mat_id) const {
        auto mat = materials_.find(mat_id);
        if (mat == materials_.end()) [[unlikely]] {
            return nullptr;
        }
        return &mat->second;
    }

    /**
     * @brief Generate a map from Color to material and color id
     *
     * @return map from ColorInt to pair of material pointer and color id
     */
    [[nodiscard]] std::map<ColorInt, std::pair<const Material*, ColorId>>
    get_colors_inverse_map() const;

    static void init_lua_state(
        sol::state& lua, const std::filesystem::path& lua_map_generator_file
    );

    static TerrainMacroMap map_generation_test(
        const std::filesystem::path& lua_map_generator_file, size_t size
    );

    [[nodiscard]] static biome_json_data get_json_data(const std::filesystem::path& biome_file_folder);

 private:
    // read data to create generator component
    void read_tile_macro_data_(const std::vector<tile_macros_t>& biome_data);

    // read data to generate tile components for each macro map tile
    void read_map_tile_data_(const std::vector<tile_data_t>& biome_data);

    // read data to generate the add to top after affect
    void read_add_to_top_data_(const std::vector<layer_effects_t>& biome_data);

    void
    init_lua_state_(const std::filesystem::path& lua_map_generator_file) {
        init_lua_state(lua_, lua_map_generator_file);
    };

    /**
     * @brief Load materials from json data
     *
     * @param material_data data to load from (see) data/materials.json
     */
    [[nodiscard]] std::map<MaterialId, const terrain::Material>
    init_materials_(const all_materials_t& material_data);
};

} // namespace generation

} // namespace terrain
