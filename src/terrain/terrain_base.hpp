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
#include "terrain_generation/tile_stamp.hpp"

#include <json/json.h>

#include <stdio.h>

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
    const Dim area_size_;
    // vector that determines grass color from edge distance
    std::vector<ColorId> grass_colors_;
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

    /**
     * @brief Construct a new Terrain Base object most default constructor
     * 
     * @param materials 
     * @param grass_grad_data 
     * @param grass_mid 
     * @param x_map_tiles 
     * @param y_map_tiles 
     * @param area_size 
     * @param z_tiles 
     */
    TerrainBase(
        const std::map<MaterialId, const terrain::Material>& materials,
        const std::vector<int>& grass_grad_data, unsigned int grass_mid,
        Dim x_map_tiles = 1, Dim y_map_tiles = 1, Dim area_size = 32, Dim z_tiles = 1
    );

    /**
     * @brief Construct a new Terrain Base object from qb data
     * 
     * @details This constructor loads a terrain object from a file
     * 
     * @param materials Materials that make up the terrain
     * @param grass_grad_data grass gradient data
     * @param grass_mid position in grass gradient data that denotes the middle
     * @param data qb_data_t read from file
     */
    TerrainBase(
        const std::map<MaterialId, const Material>& materials,
        std::vector<int> grass_grad_data, unsigned int grass_mid,
        voxel_utility::qb_data_t data
    );

    /**
     * @brief Construct a new Terrain Base object for demonstrating biomes
     * 
     * @param x_map_tiles 
     * @param y_map_tiles 
     * @param area_size 
     * @param z_tiles 
     * @param materials 
     * @param biome_data 
     * @param grass_grad_data 
     * @param grass_mid 
     */
    inline TerrainBase(
        Dim x_map_tiles, Dim y_map_tiles, Dim area_size, Dim z_tiles,
        const std::map<MaterialId, const Material>& materials,
        const Json::Value& biome_data, std::vector<int> grass_grad_data,
        unsigned int grass_mid
    ) :
        TerrainBase(
            x_map_tiles, y_map_tiles, area_size, z_tiles, materials, biome_data, grass_grad_data, grass_mid,
            generate_macro_map(x_map_tiles, y_map_tiles, biome_data["Terrain_Data"])
        ) {}

    /**
     * @brief Construct a new Terrain Base object using terrain generation
     * 
     * @param x_map_tiles 
     * @param y_map_tiles 
     * @param Area_size 
     * @param z_tiles 
     * @param materials 
     * @param biome_data 
     * @param grass_grad_data 
     * @param grass_mid 
     * @param Terrain_Maps 
     */
    TerrainBase(
        Dim x_map_tiles, Dim y_map_tiles, Dim Area_size, Dim z_tiles,
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
    pos(Dim x, Dim y, Dim z) const noexcept {
        // for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param sop coordinate as an array
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos(const TerrainDim3& sop) const {
        return sop.x * Y_MAX * Z_MAX + sop.y * Z_MAX + sop.z;
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
    [[nodiscard]] inline TileIndex
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
     * @return const TerrainDim3 position in space
     */
    [[nodiscard]] inline const TerrainDim3
    sop(TileIndex xyz) const {
        return {
            static_cast<Dim>(xyz / (Y_MAX * Z_MAX)),
            static_cast<Dim>((xyz / Z_MAX) % Y_MAX),
            static_cast<Dim>(xyz % (Z_MAX))};
    }

    /**
     * @brief return position in space of given index
     *
     * @param xyz index
     * @param xm length in x direction
     * @param ym length in y direction
     * @param zm length in z direction
     * @return TerrainDim3 position in 3D space
     */
    [[nodiscard]] inline static TerrainDim3
    sop(TileIndex xyz, TileIndex xm, TileIndex ym, TileIndex zm) {
        if (xyz >= xm * ym * zm) {
            throw std::invalid_argument("index out of range");
        }
        return {
            static_cast<Dim>(xyz / (ym * zm)), static_cast<Dim>((xyz / zm) % ym),
            static_cast<Dim>(xyz % (zm))};
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
     * @return VoxelSize array of sizes
     */
    [[nodiscard]] inline VoxelSize
    get_size() const noexcept {
        return {X_MAX, Y_MAX, Z_MAX};
    }

    /**
     * @brief Used for getting mesh
     *
     * @return glm::i32vec3 0 3 times
     */
    [[nodiscard]] inline VoxelOffset
    get_offset() const noexcept {
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
            static_cast<Dim>(x) < X_MAX && x >= 0 && static_cast<Dim>(y) < Y_MAX
            && y >= 0 && static_cast<Dim>(z) < Z_MAX && z >= 0
        );
    }

    [[nodiscard]] inline bool
    in_range(TerrainDim3 xyz) const {
        return (xyz.x < X_MAX && xyz.y < Y_MAX && xyz.z < Z_MAX);
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
        return &tiles_[pos(
            static_cast<Dim>(x), static_cast<Dim>(y), static_cast<Dim>(z)
        )];
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
        if (!in_range(x, y, z)) {
            LOG_CRITICAL(
                logging::terrain_logger, "Tile position ({}, {}, {}), out of range.", x,
                y, z
            );
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[pos(
            static_cast<Dim>(x), static_cast<Dim>(y), static_cast<Dim>(z)
        )];
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

    [[nodiscard]] inline VoxelColorId
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
    stamp_tile_region(const terrain_generation::TileStamp& tStamp, int x, int y) {
        stamp_tile_region(
            tStamp.x_start + x * area_size_ + area_size_ / 2,
            tStamp.y_start + y * area_size_ + area_size_ / 2, tStamp.z_start,
            tStamp.x_end + x * area_size_ + area_size_ / 2,
            tStamp.y_end + y * area_size_ + area_size_ / 2, tStamp.z_end, tStamp.mat,
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
