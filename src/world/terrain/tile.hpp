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
#include "types.hpp"

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
 * @brief represents a tile in the world
 *
 * @details A tile in the terrain. This holds data like material, and color id.
 */
class Tile {
 private:
    Dim x; // The x index
    Dim y; // The y index
    Dim z; // The z index
    // does this need to know where it is?
    // The material id of the tile
    MaterialId mat_id_;
    // The tile color is determined by this and the material type.
    ColorId color_id_;
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
    Tile(TerrainDim3 sop, const terrain::material_t* material, ColorId color_id = 0) :
        Tile(material, color_id) {}

    Tile(const terrain::material_t* material, ColorId color_id = 0);

    Tile(const Tile&) = default;

    Tile(Tile&&) = default;

    // This probably should not be used.
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
    void set_material(const terrain::material_t* const materials);

    void set_material(const terrain::material_t* const materials, ColorId color_id_);

    /**
     * @brief Set the color id, and material
     *
     * @param color_id color to set
     * @param material material to set
     */
    void set_color_id(ColorId color_id, const terrain::material_t* const material);
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
    [[nodiscard]] inline int
    get_grow_data_low() const noexcept {
        return grow_data_low_;
    }

    /**
     * @brief Get the distance from wall
     */
    [[nodiscard]] inline int
    get_grow_data_high() const noexcept {
        return grow_data_high_;
    }

    /**
     * @brief sets grass to true, and set color id to 1
     *
     */
    void try_grow_grass();
    /**
     * @brief Set the color id if this tile is grass. Updates the grass color
     * to account for edge gradient. The gradient mimics shadows caused by
     * cliffs.
     *
     * @param grass_grad_length total length of grass gradient
     * @param grass_mid index of color not effected by edge affects
     * @param grass_colors grass color vector
     */
    void set_grass_color(
        unsigned int grass_grad_length, unsigned int grass_mid,
        std::vector<ColorId> grass_colors
    );

    // Getters
    /**
     * @brief Get the x position
     *
     * @return Dim x position
     */
    [[nodiscard]] inline Dim
    get_x() const noexcept {
        return x;
    }

    /**
     * @brief Get the y position
     *
     * @return Dim y position
     */
    [[nodiscard]] inline Dim
    get_y() const noexcept {
        return y;
    }

    /**
     * @brief Get the z position
     *
     * @return Dim z position
     */
    [[nodiscard]] inline Dim
    get_z() const noexcept {
        return z;
    }

    /**
     * @brief coordinate of tile
     *
     * @return TerrainDim3 array of x, y, z
     */
    [[nodiscard]] TerrainDim3
    sop() const noexcept {
        return {x, y, z};
    }

    /**
     * @brief is the tile grass
     *
     * @return true this tile is grass
     * @return false this tile is not grass
     */
    [[nodiscard]] inline bool
    is_grass() const noexcept {
        return grass_;
    }

    /**
     * @brief Get the material id
     *
     * @return MaterialId
     */
    [[nodiscard]] inline MaterialId
    get_material_id() const noexcept {
        return mat_id_;
    }

    /**
     * @brief Get the color id
     *
     * @return ColorId color id
     */
    [[nodiscard]] inline ColorId
    get_color_id() const noexcept {
        return color_id_;
    }

    /**
     * @brief Get the material, and color id in one
     *
     * @return uint16_t 8 bit material id, and 8 bit color id
     */
    [[nodiscard]] MatColorId get_mat_color_id() const;

    /**
     * @brief is this tile solid
     *
     * @return true the tiles is solid
     * @return false the tile is not solid
     */
    [[nodiscard]] inline bool
    is_solid() const noexcept {
        return solid_;
    }

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
