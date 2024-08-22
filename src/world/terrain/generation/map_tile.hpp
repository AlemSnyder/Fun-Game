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
 * @file object_handler.hpp
 *
 * @brief Defines ObjectHandler class.
 *
 * @ingroup World Entity
 *
 */

#pragma once

#include "noise.hpp"
#include "types.hpp"
#include "util/png_image.hpp"

#include <random>
#include <vector>

namespace terrain {

namespace generation {

// a number chosen at random by the developer
constexpr size_t RANDOM_NUMBER = 7;

/**
 * @brief Tile object for macro map
 *
 * @details Biome generation starts with a map of the world. This object represents
 * a tile in that map.
 */
class MapTile {
 private:
    MacroDim x_; // x position
    MacroDim y_; // y position

    MapTile_t tile_type_; // map tile type
    // TODO add mutex
    // This requires >= 64 x 64 map macro tiles. This would correspond to
    // 2^30 tiles. This is 4+ gigabytes.
    // std::mutex mut_; // mutex for locking to avoid collisions during generation
    std::default_random_engine rand_engine_; // random number generator

 public:
    /**
     * @brief Create a new MapTile object
     *
     * @param MapTile_t tile type
     * @param size_t random seed
     * @param MacroDim x position
     * @param MacroDim y position
     */
    MapTile(MapTile_t tile_type, size_t seed, MacroDim x = 0, MacroDim y = 0) :
        x_(x), y_(y), tile_type_(tile_type),
        rand_engine_(
            Noise::get_double((seed ^ tile_type) % RANDOM_NUMBER, x, y) * INT32_MAX
        ){};

    /**
     * @brief Get the x coordinate
     *
     * @return MacroDim the x coordinate
     */
    [[nodiscard]] inline MacroDim
    get_x() const {
        return x_;
    }

    /**
     * @brief Get the y coordinate
     *
     * @return `MacroDim` the y coordinate
     */
    [[nodiscard]] inline MacroDim
    get_y() const {
        return y_;
    }

    /**
     * @brief Get the tile type
     *
     * @return `MapTile_t` the tile type
     */
    [[nodiscard]] inline MapTile_t
    get_tile_type() const {
        return tile_type_;
    }

    /**
     * @brief Get a random engine unique to this tile
     *
     * @return `std::default_random_engine&`
     */
    [[nodiscard]] inline std::default_random_engine&
    get_rand_engine() {
        return rand_engine_;
    }
};

/**
 * @brief Map of terrain
 *
 * @details Map generated by world generation.
 */
class TerrainMacroMap {
 private:
    MacroDim width_;
    MacroDim height_;

    std::vector<MapTile> terrain_map_;

 public:
    /**
     * @brief Create a new TerrainMacroMap object
     *
     * @details default constructor
     */
    inline TerrainMacroMap() : width_(0), height_(0){};

    /**
     * @brief Create a new TerrainMacroMap
     *
     * @param std::vector<MapTile> map data
     * @param size_t map width
     * @param size_t map height
     */
    inline TerrainMacroMap(
        std::vector<MapTile> terrain_map, size_t width, size_t height
    ) :
        width_(width),
        height_(height), terrain_map_(terrain_map) {
        assert(terrain_map_.size() == width_ * height_);
    };

    /**
     * @brief Get the MapTile at a given position
     *
     * @param size_t i x axis
     * @param size_t j y axis
     */
    const MapTile&
    get_tile(size_t i, size_t j) const {
        assert(i < width_ && j < height_);
        return terrain_map_[height_ * j + i];
    }

    /**
     * @brief Get the color representation of the given position
     *
     * @param size_t i x axis
     * @param size_t j y axis
     */
    png_byte
    get_color(size_t i, size_t j) const {
        return get_tile(i, j).get_tile_type();
    }

    /**
     * @brief iterator to first map tile
     */
    const auto
    begin() const {
        return terrain_map_.begin();
    }

    /**
     * @brief iterator past last map tile
     */
    const auto
    end() const {
        return terrain_map_.end();
    }

    /**
     * @brief Get the map vector
     *
     * @return `std::vector<MapTile>&` vector of map tiles
     */
    const std::vector<MapTile>&
    data() const {
        return terrain_map_;
    }

    /**
     * @brief Get the width of the map
     *
     * @return `MacorDim` width of the map
     */
    MacroDim
    get_width() const {
        return width_;
    }

    /**
     * @brief Get the height of the map
     *
     * @return `MacorDim` height of the map
     */
    MacroDim
    get_height() const {
        return height_;
    }
};

/**
 * @brief Map of probability of generating a plant type in the terrain.
 *
 */
class PlantMap {
 private:
    // map width
    Dim width_;
    // map height
    Dim height_;

    // map data
    std::vector<float> plant_map_;

 public:
    /**
     * @brief construct a new PlantMap object
     *
     * @details Default constructor
     */
    inline PlantMap() : width_(0), height_(0){};

    /**
     * @brief Construct a new PlantMap object
     *
     * @param std::vector<float> plant_map map data
     * @param Dim width map width
     * @param Dim height map height
     */
    inline PlantMap(std::vector<float> plant_map, Dim width, Dim height) :
        width_(width), height_(height), plant_map_(plant_map) {
        assert(plant_map_.size() == width_ * height_);
    };

    /**
     * @brief Get plant probability at given position
     *
     * @param Dim i column
     * @param Dim j row
     *
     * @return `float` probability of generating a plant at the given position.
     */
    [[nodiscard]] float
    get_tile(Dim i, Dim j) const {
        assert(i < width_ && j < height_);
        return plant_map_[height_ * j + i];
    }

    /**
     * @brief Get plant probability at given position
     *
     * @param Dim i column
     * @param Dim j row
     *
     * @return `png_byte` pixel representation of probability
     */
    [[nodiscard]] png_byte
    get_color(Dim i, Dim j) const {
        return get_tile(i, j);
    }

    /**
     * @brief Get plant probability at given position
     *
     * @param Dim i column
     * @param Dim j row
     *
     * @return iterator to first element in map data
     */
    [[nodiscard]] const auto
    begin() const {
        return plant_map_.begin();
    }

    /**
     * @brief Get plant probability at given position
     *
     * @param Dim i column
     * @param Dim j row
     *
     * @return iterator past end of map
     */
    [[nodiscard]] const auto
    end() const {
        return plant_map_.end();
    }

    /**
     * @brief Get plant probability at given position
     *
     * @param Dim i column
     * @param Dim j row
     *
     * @return `const std::vector<float>&` map data
     */
    [[nodiscard]] const std::vector<float>&
    data() const {
        return plant_map_;
    }

    /**
     * @brief Get map width
     *
     * @return Dim width
     */
    [[nodiscard]] Dim
    get_width() const {
        return width_;
    }

    /**
     * @brief Get map height
     *
     * @return Dim height
     */
    [[nodiscard]] Dim
    get_height() const {
        return height_;
    }
};

} // namespace generation

} // namespace terrain