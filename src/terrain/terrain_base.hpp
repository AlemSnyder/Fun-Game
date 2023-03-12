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
#include "material.hpp"
#include "path/unit_path.hpp"
#include "tile.hpp"
#include "../util/voxel.hpp"

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
    friend class Terrain;
    friend class Chunk;

 protected:
    // vector of voxels in terrain
    std::vector<Tile> tiles_;
    // length in the x direction
    int32_t X_MAX;
    // length in the y direction
    int32_t Y_MAX;
    // length in the z direction
    int32_t Z_MAX;
    // size of terrain generation tile (see terrain generation)
    int32_t Area_size;
    // seed for randomness
    //int seed;
    // map of tile position to the node group it is in
    //std::map<int, NodeGroup*> tile_to_group_;
    // vector that determines grass color from edge distance
    std::vector<uint8_t> grass_colors_;
    // length of grass gradient
    int grass_grad_length_;
    // gradient index of grass not by an edge
    int grass_mid_;
    // mat of material id to material that describes materials in this terrain
    const std::map<int, const terrain::Material>* materials_;

 public:

    TerrainBase() {
        X_MAX = 32;
        Y_MAX = 32;
        Z_MAX = 32;
        Area_size = 32;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     * @return int
     */
    inline int
    pos(int x, int y, int z) const { // for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param sop coordinate as an array
     * @return int
     */
    inline int
    pos(const std::array<int, 3> sop) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    inline int
    pos(const int sop[3]) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    inline int
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
    inline const std::array<int, 3>
    sop(int xyz) const {
        return {xyz / (Y_MAX * Z_MAX), (xyz / Z_MAX) % Y_MAX, xyz % (Z_MAX)};
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
    inline static std::array<int, 3>
    sop(int xyz, int xm, int ym, int zm) {
        if (xyz >= xm * ym * zm) {
            throw std::invalid_argument("index out of range");
        }
        return {xyz / (ym * zm), (xyz / zm) % ym, xyz % (zm)};
    }

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline int
    get_X_MAX() const {
        return X_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline int
    get_Y_MAX() const {
        return Y_MAX;
    };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline int
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
        return (x < X_MAX && x >= 0 && y < Y_MAX && y >= 0 && z < Z_MAX && z >= 0);
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
        if (xyz < 0 || xyz >= X_MAX * Y_MAX * Z_MAX) {
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
        if ((x >= X_MAX || x < 0 || y >= Y_MAX || y < 0 || z >= Z_MAX || z < 0)) {
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
        if (xyz < 0 || xyz >= X_MAX * Y_MAX * Z_MAX) {
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
    inline uint32_t get_voxel(int x, int y, int z) const {
        // using static ints to prevent dereferencing
        // The previous material id, and color id are cashed so that materials do
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

            auto mat = materials_->at(previous_mat_id);
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
        return get_tile(x,y,z)->get_mat_color_id();
    }

};

} // namespace terrain
