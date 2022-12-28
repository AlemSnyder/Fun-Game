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
 * @file tile.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Tile class
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "material.hpp"
#include "path/unit_path.hpp"

#include <cstdint>
#include <functional>
#include <map>
#include <set>

namespace terrain {

/**
 * @brief forward declaration of tile
 *
 */
class Tile;

/**
 * @brief arranges tiles
 *
 */
struct TilePCompare {
    bool operator()(const Tile* lhs, const Tile* rhs) const;
};

/**
 * @brief represents a tile in the world
 *
 * @details A tile in the terrain. This holds data like material, and color id.
 */
class Tile {
 private:
    uint16_t x; // The x index
    uint16_t y; // The y index
    uint16_t z; // The z index
    // does this need to know where it is?
    // The material id of the tile
    uint8_t mat_id_;
    // The tile color is determined by this and the material type.
    uint8_t color_id_;
    // Determined by the horizontal manhattan distance from a wall
    uint8_t grow_data_high_;
    // Determined by the horizontal manhattan distance from a edge
    uint8_t grow_data_low_;

    bool grow_sink_ : 1;   // not used
    bool grow_source_ : 1; // not used
    bool grass_ : 1;       // Does this tile obey grass color Gradient?
    // used for can stand, grass edges, etc. Should be the same as mat->solid.
    bool solid_ : 1; // is the tile solid

 public:
    /**
     * @brief Construct a new Tile object
     *
     * @param sop tile position
     * @param material material of tile
     * @param color_id color of tile
     */
    Tile(
        std::array<int, 3> sop, const terrain::Material* material, uint8_t color_id = 0
    );

    // I will format this later or remove it if I can
    // TODO find vector of materials that uses default initializer
    // I think it might be node
    Tile() :
        x(0), y(0), z(0), mat_id_(0), color_id_(0), grow_data_high_(0),
        grow_data_low_(0), grow_sink_(false), grow_source_(false), grass_(false),
        solid_(false) {}

    // Setters
    /**
     * @brief Set the material of this tile, and update color and solid state.
     *
     * @param mat_ material to set
     */
    void set_material(const terrain::Material* const materials);
    /**
     * @brief Set the material, and color, and update solid state
     *
     * @param mat_ material to set
     * @param color_id color to set
     */
    void set_material(const terrain::Material* const materials, uint8_t color_id);
    /**
     * @brief Set the color id
     *
     * @param color_id color to set
     */
    void set_color_id(uint8_t color_id, const terrain::Material* const material);
    /**
     * @brief Set the distance from edge
     *
     * @param num distance from edge
     */
    void set_grow_data_low(int num);
    /**
     * @brief Set the distance from wall
     *
     * @param num distance from wall
     */
    void set_grow_data_high(int num);

    /**
     * @brief Get the distance from edge
     */
    inline int get_grow_data_low() const { return grow_data_low_; }

    /**
     * @brief Get the distance from wall
     */
    inline int get_grow_data_high() const { return grow_data_high_; }

    /**
     * @brief sets grass to true, and set color id to 1
     *
     */
    void try_grow_grass();
    /**
     * @brief Set the color id if this tile is grass. Updates the grass color to account
     * for edge gradient. The gradient mimics shadows caused by cliffs.
     *
     * @param grass_grad_length total length of grass gradient
     * @param grass_mid index of color not effected by edge affects
     * @param grass_colors grass color vector
     */
    void set_grass_color(
        unsigned int grass_grad_length, unsigned int grass_mid,
        std::vector<uint8_t> grass_colors
    );

    // Getters
    /**
     * @brief Get the x position
     *
     * @return int x position
     */
    inline int get_x() const { return x; }

    /**
     * @brief Get the y position
     *
     * @return int y position
     */
    inline int get_y() const { return y; }

    /**
     * @brief Get the z position
     *
     * @return int z position
     */
    inline int get_z() const { return z; }

    /**
     * @brief coordinate of tile
     *
     * @return std::array<int, 3> array of x, y, z
     */
    std::array<int, 3> sop() const;

    /**
     * @brief is the tile grass
     *
     * @return true this tile is grass
     * @return false this tile is not grass
     */
    inline bool is_grass() const { return grass_; }

    /**
     * @brief Get the material
     *
     * @return const Material*
     */
    inline uint8_t
    get_material_id() const {
        return mat_id_;
    }

    /**
     * @brief Get the color id
     *
     * @return uint8_t color id
     */
    uint8_t get_color_id() const;

    /**
     * @brief Get the material, and color id in one
     *
     * @return uint16_t 8 bit material id, and 8 bit color id
     */
    uint16_t get_mat_color_id() const;

    /**
     * @brief is this tile solid
     *
     * @return true the tiles is solid
     * @return false the tile is not solid
     */
    inline bool is_solid() const { return solid_; }

    inline bool
    operator==(const Tile other) const {
        return (this->x == other.x && this->y == other.y && this->z == other.z);
    }

    inline bool
    operator==(const Tile* other) const {
        return (this->x == other->x && this->y == other->y && this->z == other->z);
    }

    bool operator>(const Tile other) const;
};

} // namespace terrain
