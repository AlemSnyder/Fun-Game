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

#include "../logging.hpp"
#include "../types.hpp"
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
    // size of terrain generation tile (see terrain generation)
    const Dim area_size;
    // vector that determines grass color from edge distance
    std::vector<uint8_t> grass_colors_;
    // length of grass gradient
    uint8_t grass_grad_length_;
    // gradient index of grass not by an edge
    uint8_t grass_mid_;
    // mat of material id to material that describes materials in this terrain
    const std::map<MaterialId, const terrain::Material>& materials_;
 public:
    // length in the x direction
    const Dim X_MAX;
    // length in the y direction
    const Dim Y_MAX;
    // length in the z direction
    const Dim Z_MAX;

    TerrainBase(
        const std::map<MaterialId, const terrain::Material>& materials,
        const std::vector<int>& grass_grad_data, unsigned int grass_mid,
        Dim x_tiles = 1, Dim y_tiles = 1, Dim area_size = 32, Dim z_tiles = 1
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
        const std::map<MaterialId, const Material>& materials,
        std::vector<int> grass_grad_data, unsigned int grass_mid,
        voxel_utility::qb_data data
    ) :
        TerrainBase(
            materials, grass_grad_data, grass_mid, data.size[0], data.size[1], 32,
            data.size[2]
        ) {
        std::map<ColorInt, std::pair<const Material*, ColorId>> materials_inverse;
        for (auto it = materials_.begin(); it != materials_.end(); it++) {
            for (size_t color_id = 0; color_id < it->second.color.size(); color_id++) {
                materials_inverse.insert(
                    std::map<ColorInt, std::pair<const Material*, ColorId>>::value_type(
                        it->second.color.at(color_id).second,
                        std::make_pair(&it->second, (ColorId)color_id)
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
        Dim x, Dim y, Dim Area_size, Dim z,
        const std::map<MaterialId, const Material>& materials,
        const Json::Value& biome_data, std::vector<int> grass_grad_data,
        unsigned int grass_mid
    ) :
        TerrainBase(
            x, y, Area_size, z, materials, biome_data, grass_grad_data, grass_mid,
            generate_macro_map(x, y, biome_data["Terrain_Data"])
        ) {}

    TerrainBase(
        Dim x, Dim y, Dim Area_size, Dim z,
        const std::map<uint8_t, const Material>& materials,
        const Json::Value& biome_data, std::vector<int> grass_grad_data,
        unsigned int grass_mid, std::vector<int> Terrain_Maps
    );

    void qb_read(
        std::vector<ColorInt> data,
        const std::map<ColorInt, std::pair<const Material*, uint8_t>>& materials_inverse
    );

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos(Dim x, Dim y,
        Dim z) const { // for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param sop coordinate as an array
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos(const std::array<Dim, 3> sop) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    [[nodiscard]] inline TileIndex
    pos(const Dim sop[3]) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos(const Tile* const tile) const {
        return pos(tile->sop());
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    [[nodiscard]] inline int
    pos(const Tile tile) const {
        return pos(tile.get_x(), tile.get_y(), tile.get_z());
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline int
    pos_for_map(const Tile tile) const {
        return pos(tile);
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline int
    pos_for_map(const Tile* const tile) const {
        return pos(tile);
    }

    /**
     * @brief return position in space of given vector index
     *
     * @param xyz vector index
     * @return const std::array<int, 3> position in space
     */
    [[nodiscard]] inline const std::array<Dim, 3>
    sop(TileIndex xyz) const {
        return {
            Dim(xyz / (Y_MAX * Z_MAX)), Dim((xyz / Z_MAX) % Y_MAX),
            Dim(xyz % (Z_MAX))};
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
    [[nodiscard]] inline static std::array<Dim, 3>
    sop(TileIndex xyz, TileIndex xm, TileIndex ym, TileIndex zm) {
        if (xyz >= xm * ym * zm) {
            throw std::invalid_argument("index out of range");
        }
        return {
            Dim(xyz / (ym * zm)), Dim((xyz / zm) % ym), Dim(xyz % (zm))};
    }

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    [[nodiscard]] inline Dim
    get_X_MAX() const {
        return X_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    [[nodiscard]] inline Dim
    get_Y_MAX() const {
        return Y_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    [[nodiscard]] inline Dim
    get_Z_MAX() const {
        return Z_MAX;
    };

    /**
     * @brief Get the size of terrain
     *
     * @details overloaded so must use base class definition.
     *
     * @return glm::u32vec3 array of sizes
     */
    [[nodiscard]] inline glm::u32vec3
    get_size() const {
        return { X_MAX, Y_MAX, Z_MAX };
    }

    /**
     * @brief Used for getting mesh
     *
     * @return std::array<int32_t, 3> 0 3 times
     */
    [[nodiscard]] inline glm::i32vec3
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
    [[nodiscard]] inline bool
    in_range(int x, int y, int z) const {
        return (
            static_cast<Dim>(x) < X_MAX && x >= 0 && static_cast<Dim>(y) < Y_MAX && y >= 0 && static_cast<Dim>(z) < Z_MAX
            && z >= 0
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
    [[nodiscard]] inline Tile*
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

    [[nodiscard]] inline Tile*
    get_tile(int xyz) {
        if (xyz < 0 || static_cast<TileIndex>(xyz) >= X_MAX * Y_MAX * Z_MAX) {
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
    [[nodiscard]] inline const Tile*
    get_tile(int x, int y, int z) const {
        if ((static_cast<Dim>(x) >= X_MAX || x < 0 || static_cast<Dim>(y) >= Y_MAX
             || y < 0 || static_cast<Dim>(z) >= Z_MAX || z < 0)) {
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
    [[nodiscard]] inline const Tile*
    get_tile(int xyz) const {
        if (xyz < 0 || static_cast<TileIndex>(xyz) >= X_MAX * Y_MAX * Z_MAX) {
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
     * @return ColorInt color or tile
     */
    [[nodiscard]] inline ColorInt
    get_voxel(int x, int y, int z) const {
        // using static ints to prevent dereferencing
        // The previous material id, and color id are cached so that materials do
        // not need to be dereferenced, and searched through.
        static MaterialId previous_mat_id = 0;
        static ColorId previous_color_id = 0;
        static ColorInt previous_out_color = 0;

        if (!in_range(x, y, z))
            return 0;

        MaterialId mat_id = tiles_[pos(x, y, z)].get_material_id();
        ColorId color_id = tiles_[pos(x, y, z)].get_color_id();

        if (mat_id != previous_mat_id || color_id != previous_color_id) {
            previous_mat_id = mat_id;
            previous_color_id = color_id;

            auto mat = materials_.at(previous_mat_id);
            previous_out_color = mat.color[previous_color_id].second;
        }

        return previous_out_color;
    }

    [[nodiscard]] inline uint16_t
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
    [[nodiscard]] int get_first_not(
        const std::set<std::pair<MaterialId, ColorId>>& materials, int x, int y,
        int guess
    ) const;

    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept{
        return grass_grad_length_;
    }

    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return grass_mid_;
    }

    [[nodiscard]] inline std::vector<uint8_t>
    get_grass_colors() const {
        return grass_colors_;
    }

    [[nodiscard]] inline const std::map<MaterialId, const terrain::Material>&
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
        const Material* mat,
        const std::set<std::pair<MaterialId, ColorId>>& elements_can_stamp,
        ColorId color_id
    );

    /**
     * @brief add material on top of extant voxels
     *
     * @param to_data json data determines number of layers, and where voxels
     * are added
     * @param material material type to add
     */
    void add_to_top(
        const Json::Value& to_data, const std::map<MaterialId, const Material>& material
    );

    /**
     * @brief generates a 2D 'height' map to use to generate the terrain
     *
     * @param size_x number of tile types in x direction
     * @param size_y number of tile types in y direction
     * @param map_data json data on how to generate map
     * @return std::vector<int> (size_x * size_y) vector of ints
     */
    [[nodiscard]] static std::vector<int> generate_macro_map(
        unsigned int size_x, unsigned int size_y, const Json::Value& map_data
    );

    /**
     * @brief Get the max allowable height of added material
     *
     * @param height height of terrain in question
     * @param how_to_add json data that defines biome generation
     * @return int max height
     */
    [[nodiscard]] static int get_stop_height(int height, const Json::Value& how_to_add);

    [[nodiscard]] inline bool
    has_tile_material(
        const std::set<std::pair<MaterialId, ColorId>>& material_test, int x, int y,
        int z
    ) const {
        return has_tile_material(material_test, get_tile(x, y, z));
    }

    [[nodiscard]] inline bool
    has_tile_material(
        const std::set<std::pair<MaterialId, ColorId>>& material_test, const Tile* tile
    ) const {
        MaterialId mat_id = tile->get_material_id();
        ColorId color_id = tile->get_color_id();

        return (
            material_test.find(std::make_pair(mat_id, color_id)) != material_test.end()
        );
    }
};

} // namespace terrain
