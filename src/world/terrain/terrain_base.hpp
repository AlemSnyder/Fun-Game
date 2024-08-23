// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
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

#include "generation/land_generator.hpp"
#include "generation/map_tile.hpp"
#include "generation/tile_stamp.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "path/unit_path.hpp"
#include "tile.hpp"
#include "types.hpp"
#include "util/voxel.hpp"
#include "world/biome.hpp"

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
 */
class TerrainBase : public voxel_utility::VoxelBase {
 protected:
    // vector of voxels in terrain
    std::vector<Tile> tiles_;
    // size of terrain generation tile (see terrain generation)
    const Dim area_size_;
    // mat of material id to material that describes materials in this terrain
    const generation::Biome& biome_;

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
        Dim x, Dim y, Dim area_size_, Dim z, const generation::Biome& biome,
        const generation::TerrainMacroMap& macro_map
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
    TerrainBase(const generation::Biome& biome, voxel_utility::qb_data_t data);

    void qb_read(
        std::vector<ColorInt> data,
        const std::unordered_map<ColorInt, std::pair<const Material*, ColorId>>&
            materials_inverse
    );

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     * @return TileIndex
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
     * @return TileIndex
     */
    [[nodiscard]] inline TileIndex
    pos(const TerrainDim3& sop) const {
        return sop.x * Y_MAX * Z_MAX + sop.y * Z_MAX + sop.z;
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return TileIndex
     */
    [[nodiscard]] inline TileIndex
    pos(const Tile* const tile) const {
        return pos(tile->sop());
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return TileIndex
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
    [[nodiscard]] inline TileIndex
    pos_for_map(const Tile tile) const {
        return pos(tile);
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline TileIndex
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
            static_cast<Dim>((xyz / Z_MAX) % Y_MAX), static_cast<Dim>(xyz % (Z_MAX))};
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
     * @return Dim length
     */
    [[nodiscard]] inline Dim
    get_X_MAX() const {
        return X_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return Dim length
     */
    [[nodiscard]] inline Dim
    get_Y_MAX() const {
        return Y_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return Dim length
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
    in_range(TerrainOffset x, TerrainOffset y, TerrainOffset z) const {
        return (
            static_cast<Dim>(x) < X_MAX && x >= 0 && static_cast<Dim>(y) < Y_MAX
            && y >= 0 && static_cast<Dim>(z) < Z_MAX && z >= 0
        );
    }

    [[nodiscard]] inline bool
    in_range(TerrainOffset3 xyz) const {
        return (xyz.x < X_MAX && xyz.y < Y_MAX && xyz.z < Z_MAX);
    }

    /**
     * @brief Get the tile object at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     *
     * @return Tile* tile at given position
     */
    [[nodiscard]] inline Tile*
    get_tile(TerrainOffset x, TerrainOffset y, TerrainOffset z) {
        return const_cast<Tile*>(std::as_const(*this).get_tile(x, y, z));
    };

    [[nodiscard]] inline Tile*
    get_tile(TileIndex xyz) {
        return const_cast<Tile*>(std::as_const(*this).get_tile(xyz));
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
    get_tile(TerrainOffset x, TerrainOffset y, TerrainOffset z) const {
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
    get_tile(TileIndex xyz) const {
        if (xyz >= X_MAX * Y_MAX * Z_MAX) {
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
    get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const {
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

            auto mat = biome_.get_materials().at(previous_mat_id);
            previous_out_color = mat.color[previous_color_id].second;
        }

        return previous_out_color;
    }

    [[nodiscard]] inline ColorInt
    get_voxel(VoxelOffset position) const {
        return get_voxel(position.x, position.y, position.z);
    }

    [[nodiscard]] inline VoxelColorId
    get_voxel_color_id(Dim x, Dim y, Dim z) const {
        // if not in range, then considered to be air
        if (!in_range(x, y, z))
            return 0;
        // use mat color id for voxel color id because the tile has that data
        // it is therefore just a look up. (pointer dereference)
        return get_tile(x, y, z)->get_mat_color_id();
    }

    [[nodiscard]] inline VoxelColorId
    get_voxel_color_id(VoxelOffset position) const {
        return get_voxel_color_id(position.x, position.y, position.z);
    }

    /**
     * @brief Get the heights z thats material is not in materials
     *
     * @param materials materials to exclude
     * @param x x position
     * @param y y position
     * @param guess expected height (for speed)
     *
     * @return int height
     */
    [[nodiscard]] Dim
    get_first_not(const MaterialGroup& materials, Dim x, Dim y, Dim guess = 0) const;

    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return biome_.get_grass_grad_length();
    }

    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return biome_.get_grass_mid();
    }

    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return biome_.get_grass_colors();
    }

    [[nodiscard]] inline const std::unordered_map<MaterialId, const terrain::Material>&
    get_materials() const {
        return biome_.get_materials();
    }

    /**
     * @brief initialize area of terrain
     *
     * @param area_x area x coordinate
     * @param area_y area y coordinate
     * @param gen Generator object that generates tile types
     */
    void init_area(generation::MapTile& map_tile, generation::LandGenerator gen);

    /**
     * @brief Set a group of tiles
     *
     * @param tStamp where the stamp is, and material and color
     * @param x macro map x position
     * @param y macro map y position
     */
    inline void stamp_tile_region(const generation::TileStamp& tStamp, int x, int y);

    /**
     * @brief add material on top of extant voxels
     *
     * @param to_data json data determines number of layers, and where voxels
     * are added
     * @param material material type to add
     */
    void add_to_top(const generation::AddToTop& to_data);

    /**
     * @brief Get the max allowable height of added material
     *
     * @param height height of terrain in question
     * @param how_to_add json data that defines biome generation
     * @return int max height
     */
    [[nodiscard]] static Dim get_stop_height(Dim height, const Json::Value& how_to_add);

    [[nodiscard]] inline bool
    has_tile_material(const MaterialGroup& material_test, Dim x, Dim y, Dim z) const {
        return has_tile_material(material_test, get_tile(x, y, z));
    }

    [[nodiscard]] inline bool
    has_tile_material(const MaterialGroup& material_test, const Tile* tile) const {
        MaterialId mat_id = tile->get_material_id();
        ColorId color_id = tile->get_color_id();

        return material_test.material_in(mat_id, color_id);
    }
};

} // namespace terrain
