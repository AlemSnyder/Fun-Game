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

#include "../material.hpp"
#include "tilestamp.hpp"

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
    unsigned int current_region;
    unsigned int current_sub_region;

    Json::Value data_; // this should be a structure
    const std::map<int, const Material>* materials;

 public:
    /**
     * @brief Construct a new LandGenerator object
     *
     * @param materials the materials used in this biome
     * @param data the description of how tiles stamps should be generated
     */
    LandGenerator(const std::map<int, const Material>* materials, Json::Value data);
    /**
     * @brief Construct a new LandGenerator object (default constructor)
     *
     * This should not be used.
     */
    LandGenerator();

    /**
     * @brief Test if iteration is complete
     *
     * @return true if iteration is complete,
     * @return false otherwise
     */
    inline bool empty() const { return (current_region >= data_.size()); }

    /**
     * @brief Generate and return Tile Stamp object
     *
     * @return TileStamp
     */
    TileStamp get_this_stamp() const;

    /**
     * @brief increment the data of the Tile Stamp that will be generated
     */
    void operator++() {
        current_sub_region++;
        if (current_sub_region == get_num_stamps(data_[current_region])) {
            current_region++;
            current_sub_region = 0;
        }
    }

    /**
     * @brief Resets this land generator
     */
    void reset() {
        current_region = 0;
        current_sub_region = 0;
    };

 private:
    unsigned int static get_num_stamps(Json::Value biome);

    std::array<int, 6>
    get_volume(int center[2][2], int Sxy, int Sz, int Dxy, int Dz) const;

    void from_radius(int cr, int csr, TileStamp& ts) const;
    void from_grid(int cr, int csr, TileStamp& ts) const;
    void from_positions(int cr, int csr, TileStamp& ts) const;
};

} // namespace terrain_generation

} // namespace terrain
