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

#include "plant.hpp"
#include "terrain/generation/land_generator.hpp"
#include "terrain/generation/map_tile.hpp"
#include "terrain/generation/terrain_map.hpp"
#include "util/files.hpp"

#include <sol/sol.hpp>

#include <unordered_map>
#include <unordered_set>

#pragma once

namespace terrain {

namespace generation {
/**
 * @brief Contains Json data from a biome file.
 */

struct biome_json_data {
    // Name of biome. Used both for file name and display name
    std::string biome_name;
    // Json data that describes biome
    biome_data_t biome_data;
    // Json data that describes materials
    all_materials_t materials_data;
};

/**
 * @brief Contains grass colors and pattern for a biome.
 */
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

/**
 * @brief Generates terrain and plants map
 */
class Biome {
 private:
    //  vector of const data for creating land generators for every tile
    // create a map of TileMacro_t -> LandGenerator
    std::vector<generation::LandGenerator> land_generators_;

    // map of MapTile_t -> vector of TileMacro_t
    std::vector<TileType> macro_tile_types_;

    std::vector<AddToTop> add_to_top_generators_;

    // materials that exist
    const std::unordered_map<MaterialId, const terrain::material_t> materials_;

    std::unordered_set<plant_t> generate_plants_;

    GrassData grass_data_;

    const std::filesystem::path lua_map_generator_file_;

    // display name
    const std::string name_;
    // unique identifier name
    const std::string id_name_;

 public:
    const size_t seed;

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
    Biome(const std::string& biome_name, size_t seed);

    /**
     * @brief Get macro tile map
     *
     * @param length side length of square map
     *
     * @return 2D map of map tiles
     */
    [[nodiscard]] TerrainMacroMap get_map(MacroDim length) const;

    /**
     * @brief Get plant map
     *
     * @param length side length of square map
     *
     * @return 2D map of plant percentages
     */
    [[nodiscard]] const std::unordered_map<std::string, PlantMap>
    get_plant_map(MacroDim length) const;

    // TODO pass seed
    inline TerrainMacroMap
    single_tile_type_map(MapTile_t type) {
        std::vector<terrain::generation::MapTile> out;
        out.reserve(9);

        out.emplace_back(get_macro_ids(0), 0, 0, 0);
        out.emplace_back(get_macro_ids(0), 0, 0, 1);
        out.emplace_back(get_macro_ids(0), 0, 0, 2);

        out.emplace_back(get_macro_ids(0), 0, 1, 0);
        out.emplace_back(get_macro_ids(type), 2, 1, 1);
        out.emplace_back(get_macro_ids(0), 0, 1, 2);

        out.emplace_back(get_macro_ids(0), 0, 2, 0);
        out.emplace_back(get_macro_ids(0), 0, 2, 1);
        out.emplace_back(get_macro_ids(0), 0, 2, 2);

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
    [[nodiscard]] inline const TileType&
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
    [[nodiscard]] inline const std::unordered_map<
        MaterialId, const terrain::material_t>&
    get_materials() const {
        return materials_;
    }

    [[nodiscard]] inline const std::unordered_set<plant_t>&
    get_generate_plants() const {
        return generate_plants_;
    }

    /**
     * @brief Get material pointer if it exists else null pointer
     *
     * @param mat_id MaterialId id of material
     *
     * @return pointer to corresponding material
     */
    [[nodiscard]] inline const terrain::material_t*
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
    [[nodiscard]] std::unordered_map<ColorInt, MaterialColor>
    get_colors_inverse_map() const;

    static TerrainMacroMap map_generation_test(
        const std::filesystem::path& lua_map_generator_file, size_t size
    );

    [[nodiscard]] static biome_json_data
    get_json_data(const std::filesystem::path& biome_file_folder);

 private:
    // read data to create generator component
    void read_tile_macro_data_(const std::vector<tile_macros_t>& biome_data);

    // read data to generate tile components for each macro map tile
    void read_map_tile_data_(const std::vector<tile_data_t>& biome_data);

    // read data to generate the add to top after affect
    void read_add_to_top_data_(const std::vector<layer_effects_t>& biome_data);

    void init_lua_state_(const std::filesystem::path& lua_map_generator_file);

    /**
     * @brief Load materials from json data
     *
     * @param material_data data to load from (see) data/materials.json
     */
    [[nodiscard]] std::unordered_map<MaterialId, const terrain::material_t>
    init_materials_(const all_materials_t& material_data);

    [[nodiscard]] biome_json_data get_json_data_(const std::string& biome_name);
};

} // namespace generation

} // namespace terrain
