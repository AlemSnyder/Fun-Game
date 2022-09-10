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
    // The tile color is determined by this and the material type.
    uint8_t color_id;
    // Determined by the horizontal manhattan distance from a wall
    uint8_t grow_data_high;
    // Determined by the horizontal manhattan distance from a edge
    uint8_t grow_data_low;

    bool grow_sink;   // not used
    bool grow_source; // not used
    bool grass;       // Does this tile obey grass color Gradient?

    // used for can stand, grass edges, etc. Should be the same as mat->solid.
    bool solid;          // is the tile solid
    const Material* mat; // The material of the tile
    // adjacent tiles
    // std::map<Tile*, UnitPath, TilePCompare> adjacent;

 public:
    /**
     * @brief Construct a new Tile object (default constructor)
     *
     */
    Tile();
    /**
     * @brief Construct a new Tile object
     *
     * @param sop tile position
     * @param mat material of tile
     */
    Tile(std::array<int, 3> sop, const Material* mat);
    /**
     * @brief Construct a new Tile object
     *
     * @param sop tile position
     * @param mat material of tile
     * @param color_id color of tile
     */
    Tile(std::array<int, 3> sop, const Material* mat, uint8_t color_id);
    /**
     * @brief tile initializer
     *
     * @param sop tile position
     * @param solid is the tile solid
     */
    void init(std::array<int, 3> sop, bool solid);
    /**
     * @brief tile initializer
     *
     * @param sop tile position
     * @param mat material of tile
     */
    void init(std::array<int, 3> sop, const Material* mat);
    /**
     * @brief tile initializer
     *
     * @param sop tile position
     * @param mat material of tile
     * @param color_id color of tile
     */
    void init(std::array<int, 3> sop, const Material* mat, uint8_t color_id);
    // Setters
    /**
     * @brief Set the material of this tile, and update color and solid state.
     *
     * @param mat_ material to set
     */
    void set_material(const Material* mat_);
    /**
     * @brief Set the material, and color, and update solid state
     *
     * @param mat_ material to set
     * @param color_id color to set
     */
    void set_material(const Material* mat_, uint8_t color_id);
    /**
     * @brief Set the color id
     *
     * @param color_id color to set
     */
    void set_color_id(uint8_t color_id);
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
     * @brief sets grass to true, and set color id to 1
     *
     */
    void try_grow_grass();
    /**
     * @brief Set the color id if this tile is grass
     *
     * @param grass_grad_length total length of grass gradient
     * @param grass_mid index of color not effected by edge affects
     * @param grass_colors grass color vector
     */
    void set_grass_color(
        int grass_grad_length, int grass_mid, std::vector<uint8_t> grass_colors
    );

    // Getters
    /**
     * @brief Get the x position
     *
     * @return int x position
     */
    int get_x() const { return x; }

    /**
     * @brief Get the y position
     *
     * @return int y position
     */
    int get_y() const { return y; }

    /**
     * @brief Get the z position
     *
     * @return int z position
     */
    int get_z() const { return z; }

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
    bool is_grass() const { return grass; }

    /**
     * @brief Get the material
     *
     * @return const Material*
     */
    const Material* get_material() const { return mat; }

    /**
     * @brief Get the color
     *
     * @return uint32_t color
     */
    uint32_t get_color() const;
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
     * @brief Get the distance from edge
     *
     * @return uint8_t distance from edge
     */
    uint8_t get_grow_low() const;
    /**
     * @brief Get the distance from wall
     *
     * @return uint8_t distance from wall
     */
    uint8_t get_grow_high() const;
    /**
     * @brief add an adjacent tile
     *
     * @param tile tile to add
     * @param type path type between tiles
     */
    //void add_adjacent(Tile* tile, UnitPath type);
    /**
     * @brief add an adjacent tile (fast)
     *
     * @param it iterator to position in adjacent tiles
     * @param tile tile to add
     * @param type path type
     */
    //void
    //add_adjacent(std::map<Tile*, UnitPath>::iterator it, Tile* tile, UnitPath type);
    /**
     * @brief clear the adjacent tiles
     *
     */
    //void clear_adjacent();

    /**
     * @brief Get the adjacency map
     *
     * @return std::map<Tile *, UnitPath, TilePCompare>& map of adjacent tile to
     */
    //std::map<Tile*, UnitPath, TilePCompare>& get_adjacent_map() { return adjacent; };

    /**
     * @brief Get the adjacency map
     *
     * @return std::map<Tile *, UnitPath, TilePCompare>& map of adjacent tile to
     */
    //const std::map<Tile*, UnitPath, TilePCompare>& get_adjacent_map() const {
    //    return adjacent;
    //};

    /**
     * @brief Get the adjacent tiles that match the path given
     *
     * @param path_type path to match
     * @return std::set<Tile *> adjacent tiles that match the path type given
     */
    //std::set<Tile*> get_adjacent_clear(int path_type);
    /**
     * @brief Get the adjacent tiles that match the path given
     *
     * @param path_type path to match
     * @return std::set<Tile *> adjacent tiles that match the path type given
     */
    //std::set<const Tile*> get_adjacent_clear(int path_type) const;

    /**
     * @brief is this tile solid
     *
     * @return true the tiles is solid
     * @return false the tile is not solid
     */
    inline bool is_solid() const { return solid; }

    bool operator==(const Tile other) const {
        return (this->x == other.x && this->y == other.y && this->z == other.z);
    }

    bool operator==(const Tile* other) const {
        return (this->x == other->x && this->y == other->y && this->z == other->z);
    }

    bool operator>(const Tile other) const;
};

} // namespace terrain
