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

#include "land_generator.hpp"
#include "noise.hpp"
#include "types.hpp"
#include "util/png_image.hpp"

#include <random>
#include <unordered_set>
#include <vector>

namespace terrain {

namespace generation {

// a number chosen at random by the developer
constexpr size_t RANDOM_NUMBER = 7;

/**
 * @brief Tile type for macro map
 */
class TileType {
 private:
    // I want to use the order
    const std::unordered_set<const LandGenerator*> land_generators_;

    MapTile_t tile_type_;

    ColorInt top_color_;
    ColorInt secondary_color_;

    Dim height_;

 public:
    [[nodiscard]] inline MapTile_t
    get_tile_type() const {
        return tile_type_;
    }

    TileType(
        const std::unordered_set<const LandGenerator*> land_generators,
        MapTile_t tile_type, const std::vector<AddToTop>& layer_effect_generators,
        const std::unordered_map<MaterialId, const terrain::Material> materials
    );

    inline auto
    begin() const {
        return land_generators_.begin();
    }

    inline auto
    end() const {
        return land_generators_.end();
    }

    inline auto get_top_color() const {
        return top_color_;
    }

    inline auto get_secondary_color() const {
        return secondary_color_;
    }

    inline auto get_height() const {
        return height_;
    }
};

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

    const TileType& tile_type_; // map tile type
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
    MapTile(const TileType& tile_type, size_t seed, MacroDim x = 0, MacroDim y = 0) :
        x_(x), y_(y), tile_type_(tile_type),
        rand_engine_(
            Noise::get_double((seed ^ tile_type.get_tile_type()) % RANDOM_NUMBER, x, y)
            * INT32_MAX
        ) {};

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
    get_type_id() const {
        return tile_type_.get_tile_type();
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

    [[nodiscard]] inline const TileType&
    get_type() const {
        return tile_type_;
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
    inline PlantMap() : width_(0), height_(0) {};

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
