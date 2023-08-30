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
 * @file land_generator.hpp
 *
 * @brief Defines LandGenerator class.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "../../types.hpp"
#include "../material.hpp"
#include "tile_stamp.hpp"

#include <json/json.h>

#include <math.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <string>

namespace terrain {

namespace terrain_generation {

/**
 * @brief Reads JSON data and generates TileStamp objects
 *
 * @details The way biomes are generated is saved in data/biome_data.json.
 * The biome pipeline starts with getting a 2D tile map. Each tile in the map
 * a different type (usually height). Next is defining macros. These define how
 * some part of a map tile is generated. This is useful because some parts of a
 * map tile will be the same as another with a different value (all bedrock is
 * the same). Each map tile type is now assigned macros. Land Generator
 * iterates though these macros, and creates Tile Stamps.
 *
 */
class LandGenerator {
 private:
    unsigned int current_region_;
    unsigned int current_sub_region_;

    const std::map<MaterialId, const Material>& materials_;
    Json::Value data_; // this should be a structure

 public:
    /**
     * @brief Construct a new LandGenerator object
     *
     * @param materials the materials used in this biome
     * @param data the description of how tiles stamps should be generated
     */
    LandGenerator(
        const std::map<MaterialId, const Material>& materials, const Json::Value data
    );

    /**
     * @brief Construct a new LandGenerator object (default constructor)
     *
     * This should not be used.
     */
    //    LandGenerator();

    /**
     * @brief Test if iteration is complete
     *
     * @return true if iteration is complete,
     * @return false otherwise
     */
    inline bool
    empty() const {
        return (current_region_ >= data_.size());
    }

    /**
     * @brief Generate and return Tile Stamp object
     *
     * @return TileStamp
     */
    TileStamp get_this_stamp() const;

    /**
     * @brief increment the data of the Tile Stamp that will be generated
     */
    void next();

    /**
     * @brief Resets this land generator
     */
    inline void
    reset() {
        current_region_ = 0;
        current_sub_region_ = 0;
    };

 private:
    unsigned int static get_num_stamps_(const Json::Value& biome);

    void
    generate_tile_stamp_(int center[2][2], int Sxy, int Sz, int Dxy, int Dz, TileStamp& ts) const;

    void from_radius_(int cr, int csr, TileStamp& ts) const;
    void from_grid_(int cr, int csr, TileStamp& ts) const;
    void from_positions_(int cr, int csr, TileStamp& ts) const;
};

} // namespace terrain_generation

} // namespace terrain
