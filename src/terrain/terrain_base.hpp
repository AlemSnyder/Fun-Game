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
 * @file terrain.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Terrain class
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "../constants.hpp"
#include "../logging.hpp"
#include "../util/voxel.hpp"
#include "material.hpp"
#include "path/unit_path.hpp"
#include "terrain_generation/land_generator.hpp"
#include "tile.hpp"

#include <json/json.h>

#include <stdio.h>

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace terrain {

/**
 * @brief The land in the world.
 *
 * @details Terrain Base holds all the tiles that exist. Base class for Terrain.
 * Contains the data, and lookup informaiton.
 *
 * Everything should be inline and in this one header file.
 *
 */
class TerrainBase : public voxel_utility::VoxelBase {
 protected:
    // vector of voxels in terrain
    std::vector<Tile> tiles_;
    // length in the x direction
    const uint32_t X_MAX;
    // length in the y direction
    const uint32_t Y_MAX;
    // length in the z direction
    const uint32_t Z_MAX;
    // size of terrain generation tile (see terrain generation)
    const uint32_t area_size;
    // vector that determines grass color from edge distance
    std::vector<uint8_t> grass_colors_;
    // length of grass gradient
    uint8_t grass_grad_length_;
    // gradient index of grass not by an edge
    uint8_t grass_mid_;
    // mat of material id to material that describes materials in this terrain
    const std::map<Material_id_t, const terrain::Material>& materials_;

 public:
    TerrainBase(
        const std::map<Material_id_t, const terrain::Material>& materials,
        const std::vector<int>& grass_grad_data, unsigned int grass_mid,
        int x_tiles = 1, int y_tiles = 1, int area_size = 32, int z_tiles = 1
    ) :
        X_MAX(x_tiles * area_size),
        Y_MAX(y_tiles * area_size), Z_MAX(z_tiles * area_size), area_size(area_size),
        materials_(materials) {
        tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

        if (grass_mid >= grass_grad_data.size()) {
            grass_mid_ = grass_grad_data.size() - 1;
            std::cerr << "Grass Mid (from biome_data.json) not valid";
        }

        for (size_t i = 0; i < grass_grad_data.size(); i++) {
            if (i == static_cast<size_t>(grass_mid)) {
                grass_mid_ = grass_colors_.size();
            }
            for (int j = 0; j < grass_grad_data[i]; j++) {
                grass_colors_.push_back(i);
            }
        }
        grass_grad_length_ = grass_colors_.size();
    }

    TerrainBase(
        const std::map<Material_id_t, const Material>& materials,
        std::vector<int> grass_grad_data, unsigned int grass_mid,
        voxel_utility::qb_data data
    ) :
        TerrainBase(
            materials, grass_grad_data, grass_mid, data.size[0], data.size[1], 32,
            data.size[2]
        ) {
        std::map<Color_int_t, std::pair<const Material*, Color_id_t>> materials_inverse;
        for (auto it = materials_.begin(); it != materials_.end(); it++) {
            for (size_t color_id = 0; color_id < it->second.color.size(); color_id++) {
                materials_inverse.insert(
                    std::map<Color_int_t, std::pair<const Material*, Color_id_t>>::
                        value_type(
                            it->second.color.at(color_id).second,
                            std::make_pair(&it->second, (Color_id_t)color_id)
                        )
                );
            }
        }

        try {
            qb_read(data.data, materials_inverse);
        } catch (const std::exception& e) {
            LOG_ERROR(
                logging::terrain_logger, "Could not load terrain save file due to {}",
                e.what()
            );
            throw;
        }
    }

    TerrainBase(
        int x, int y, int Area_size, int z,
        const std::map<Material_id_t, const Material>& materials,
        const Json::Value& biome_data, std::vector<int> grass_grad_data,
        unsigned int grass_mid
    ) :
        TerrainBase(
            x, y, Area_size, z, materials, biome_data, grass_grad_data, grass_mid,
            generate_macro_map(x, y, biome_data["Terrain_Data"])
        ) {}

    TerrainBase(
        int x, int y, int Area_size, int z,
        const std::map<uint8_t, const Material>& materials,
        const Json::Value& biome_data, std::vector<int> grass_grad_data,
        unsigned int grass_mid, std::vector<int> Terrain_Maps
    ) :
        TerrainBase(materials, grass_grad_data, grass_mid, x, y, Area_size, z) {
        for (unsigned int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
            tiles_.push_back(Tile(sop(xyz), &materials_.at(0)));
        }

        // srand(seed);
        LOG_INFO(logging::terrain_logger, "Start of land generator.");

        // create a map of int -> LandGenerator
        std::map<int, terrain_generation::LandGenerator> land_generators;

        // for tile macro in data biome
        for (unsigned int i = 0; i < biome_data["Tile_Macros"].size(); i++) {
            // create a land generator for each tile macro
            terrain_generation::LandGenerator gen(
                materials, biome_data["Tile_Macros"][i]["Land_Data"]
            );
            land_generators.insert(std::make_pair(i, gen));
        }

        LOG_INFO(
            logging::terrain_logger,
            "End of land generator: create macro tile generator."
        );

        // TODO make this faster 4
        for (int i = 0; i < x; i++)
            for (int j = 0; j < y; j++) {
                int tile_type = Terrain_Maps[j + i * y];
                Json::Value macro_types =
                    biome_data["Tile_Data"][tile_type]["Land_From"];
                for (Json::Value generator_macro : macro_types) {
                    init_area(i, j, land_generators[generator_macro.asInt()]);
                }
            }

        LOG_INFO(logging::terrain_logger, "End of land generator: place tiles .");

        // TODO make this faster 3
        for (unsigned int i = 0; i < biome_data["After_Effects"]["Add_To_Top"].size();
             i++) {
            add_to_top(biome_data["After_Effects"]["Add_To_Top"][i], materials);
        }

        LOG_INFO(
            logging::terrain_logger, "End of land generator: top layer placement."
        );
    }

    void qb_read(
        std::vector<uint32_t> data,
        const std::map<uint32_t, std::pair<const Material*, uint8_t>>& materials_inverse
    );

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     * @return int
     */
    inline uint32_t
    pos(uint16_t x, uint16_t y,
        uint16_t z) const { // for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param sop coordinate as an array
     * @return int
     */
    inline uint32_t
    pos(const std::array<uint16_t, 3> sop) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    inline uint32_t
    pos(const uint16_t sop[3]) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    inline uint32_t
    pos(const Tile* const tile) const {
        return pos(tile->sop());
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    inline int
    pos(const Tile tile) const {
        return pos(tile.get_x(), tile.get_y(), tile.get_z());
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    inline int
    pos_for_map(const Tile tile) const {
        return pos(tile);
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    inline int
    pos_for_map(const Tile* const tile) const {
        return pos(tile);
    }

    /**
     * @brief return position in space of given vector index
     *
     * @param xyz vector index
     * @return const std::array<int, 3> position in space
     */
    inline const std::array<uint16_t, 3>
    sop(uint32_t xyz) const {
        return {
            uint16_t(xyz / (Y_MAX * Z_MAX)), uint16_t((xyz / Z_MAX) % Y_MAX),
            uint16_t(xyz % (Z_MAX))};
    }

    /**
     * @brief return position in space of given index
     *
     * @param xyz index
     * @param xm length in x direction
     * @param ym length in y direction
     * @param zm length in z direction
     * @return std::array<int, 3> position in 3D space
     */
    inline static std::array<uint16_t, 3>
    sop(uint32_t xyz, uint32_t xm, uint32_t ym, uint32_t zm) {
        if (xyz >= xm * ym * zm) {
            throw std::invalid_argument("index out of range");
        }
        return {
            uint16_t(xyz / (ym * zm)), uint16_t((xyz / zm) % ym), uint16_t(xyz % (zm))};
    }

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline uint32_t
    get_X_MAX() const {
        return X_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline uint32_t
    get_Y_MAX() const {
        return Y_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline uint32_t
    get_Z_MAX() const {
        return Z_MAX;
    };

    /**
     * @brief Get the size of terrain
     *
     * @return std::array<uint32_t, 3> array of sizes
     */
    inline std::array<uint32_t, 3>
    get_size() const {
        return {
            static_cast<uint32_t>(X_MAX),
            static_cast<uint32_t>(Y_MAX),
            static_cast<uint32_t>(Z_MAX),
        };
    }

    /**
     * @brief Used for getting mesh
     *
     * @return std::array<int32_t, 3> 0 3 times
     */
    inline std::array<int32_t, 3>
    get_offset() const {
        return {0, 0, 0};
    }

    /**
     * @brief test if tile position is within terrain bounds
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return true tile is in bounds
     * @return false tile is not in bounds
     */
    inline bool
    in_range(int x, int y, int z) const {
        return (
            (uint32_t)x < X_MAX && x >= 0 && (uint32_t)y < Y_MAX && y >= 0
            && (uint32_t)z < Z_MAX && z >= 0
        );
    }

    /**
     * @brief Get the tile object at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return Tile* tile at given position
     */
    inline Tile*
    get_tile(int x, int y, int z) {
        if (!in_range(x, y, z)) {
            LOG_CRITICAL(
                logging::terrain_logger, "Tile position ({}, {}, {}), out of range.", x,
                y, z
            );
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[pos(x, y, z)];
    };

    inline Tile*
    get_tile(int xyz) {
        if (xyz < 0 || (uint32_t)xyz >= X_MAX * Y_MAX * Z_MAX) {
            LOG_CRITICAL(logging::terrain_logger, "Tile index {}, out of range.", xyz);
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[xyz];
    }

    /**
     * @brief Get the tile at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return Tile* tile at given position
     */
    inline const Tile*
    get_tile(int x, int y, int z) const {
        if (((uint32_t)x >= X_MAX || x < 0 || (uint32_t)y >= Y_MAX || y < 0
             || (uint32_t)z >= Z_MAX || z < 0)) {
            LOG_CRITICAL(
                logging::terrain_logger, "Tile position ({}, {}, {}), out of range.", x,
                y, z
            );
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[pos(x, y, z)];
    };

    /**
     * @brief Get the tile at the given index
     *
     * @param xyz tile index
     * @return const Tile* tile at index
     */
    inline const Tile*
    get_tile(int xyz) const {
        if (xyz < 0 || (uint32_t)xyz >= X_MAX * Y_MAX * Z_MAX) {
            LOG_CRITICAL(logging::terrain_logger, "Tile index {}, out of range.", xyz);
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[xyz];
    }

    /**
     * @brief Get the color of a tile
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return uint32_t color or tile
     */
    inline uint32_t
    get_voxel(int x, int y, int z) const {
        // using static ints to prevent dereferencing
        // The previous material id, and color id are cached so that materials do
        // not need to be dereferenced, and searched through.
        static uint8_t previous_mat_id = 0;
        static uint8_t previous_color_id = 0;
        static uint32_t previous_out_color = 0;

        if (!in_range(x, y, z))
            return 0;

        uint8_t mat_id = tiles_[pos(x, y, z)].get_material_id();
        uint8_t color_id = tiles_[pos(x, y, z)].get_color_id();

        if (mat_id != previous_mat_id || color_id != previous_color_id) {
            previous_mat_id = mat_id;
            previous_color_id = color_id;

            auto mat = materials_.at(previous_mat_id);
            previous_out_color = mat.color[previous_color_id].second;
        }

        return previous_out_color;
    }

    inline uint16_t
    get_voxel_color_id(int x, int y, int z) const {
        // if not in range, then considered to be air
        if (!in_range(x, y, z))
            return 0;
        // use mat color id for voxel color id because the tile has that data
        // it is therefore just a look up. (pointer dereference)
        return get_tile(x, y, z)->get_mat_color_id();
    }

    /**
     * @brief Get the heights z thats material is not in materials
     *
     * @param materials materials to exclude
     * @param x x position
     * @param y y position
     * @param guess expected height (for speed)
     * @return int height
     */
    int get_first_not(
        const std::set<std::pair<Material_id_t, Color_id_t>>& materials, int x, int y,
        int guess
    ) const;

    inline uint8_t
    get_grass_grad_length() const {
        return grass_grad_length_;
    }

    inline uint8_t
    get_grass_mid() const {
        return grass_mid_;
    }

    inline std::vector<uint8_t>
    get_grass_colors() const {
        return grass_colors_;
    }

    inline const std::map<Material_id_t, const terrain::Material>&
    get_materials() const {
        return materials_;
    }

    /**
     * @brief initialize area of terrain
     *
     * @param area_x area x coordinate
     * @param area_y area y coordinate
     * @param gen Generator object that generates tile types
     */
    void init_area(int area_x, int area_y, terrain_generation::LandGenerator gen);

    /**
     * @brief Set a group of tiles
     *
     * @param tStamp where the stamp is, and material and color
     * @param x macro map x position
     * @param y macro map y position
     */
    inline void
    stamp_tile_region(terrain_generation::TileStamp tStamp, int x, int y) {
        stamp_tile_region(
            tStamp.x_start + x * area_size + area_size / 2,
            tStamp.y_start + y * area_size + area_size / 2, tStamp.z_start,
            tStamp.x_end + x * area_size + area_size / 2,
            tStamp.y_end + y * area_size + area_size / 2, tStamp.z_end, tStamp.mat,
            tStamp.elements_can_stamp, tStamp.color_id
        );
    }

    /**
     * @brief Set a group of tiles
     *
     * @param x_start lower x position
     * @param y_start lower y position
     * @param z_start lower z position
     * @param x_end greater x position
     * @param y_end greater y position
     * @param z_end greater z position
     * @param mat materials to set
     * @param color_id color id to set
     */
    void stamp_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const Material* mat, uint8_t color_id
    );
    /**
     * @brief Set a group of tiles
     *
     * @param x_start lower x position
     * @param y_start lower y position
     * @param z_start lower z position
     * @param x_end greater x position
     * @param y_end greater y position
     * @param z_end greater z position
     * @param mat materials to set
     * @param elements_can_stamp type of material that can be changed
     * @param color_id color id to set
     */
    void stamp_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const Material* mat, std::set<std::pair<Material_id_t, Color_id_t>> elements_can_stamp,
        Color_id_t color_id
    );

    /**
     * @brief add material on top of extant voxels
     *
     * @param to_data json data determines number of layers, and where voxels
     * are added
     * @param material material type to add
     */
    void add_to_top(
        const Json::Value& to_data, const std::map<Material_id_t, const Material>& material
    );

    /**
     * @brief generates a 2D 'height' map to use to generate the terrain
     *
     * @param size_x number of tile types in x direction
     * @param size_y number of tile types in y direction
     * @param map_data json data on how to generate map
     * @return std::vector<int> (size_x * size_y) vector of ints
     */
    static std::vector<int> generate_macro_map(
        unsigned int size_x, unsigned int size_y, const Json::Value& map_data
    );

    /**
     * @brief Get the max allowable height of added material
     *
     * @param height height of terrain in question
     * @param how_to_add json data that defines biome generation
     * @return int max height
     */
    static int get_stop_height(int height, const Json::Value& how_to_add);

    inline bool
    has_tile_material(
        const std::set<std::pair<Material_id_t, Color_id_t>>& material_test, int x,
        int y, int z
    ) const {
        return has_tile_material(material_test, get_tile(x, y, z));
    }

    inline bool
    has_tile_material(
        const std::set<std::pair<Material_id_t, Color_id_t>>& material_test,
        const Tile* tile
    ) const {
        Material_id_t mat_id = tile->get_material_id();
        Color_id_t color_id = tile->get_color_id();

        return (
            material_test.find(std::make_pair(mat_id, color_id)) != material_test.end()
        );
    }
};

} // namespace terrain