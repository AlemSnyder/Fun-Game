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
 * @brief Defines Land_Generator class.
 *
 * @ingroup Terrain
 *
 */

#ifndef __LAND_GENERATOR_HPP__
#define __LAND_GENERATOR_HPP__

#include <cstdint>
#include "json/json.h"
#include <map>
#include <math.h>
#include <string>
#include <iostream>
#include <string>

#include "tile_stamp.hpp"
#include "material.hpp"

/**
 * @brief Reads JSON data and generates Tile_Stamp objects
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
class Land_Generator{
public:
    /**
     * @brief Construct a new Land_Generator object
     * 
     * @param materials the materials used in this biome
     * @param data the description of how tiles stamps should be generated
     */
    Land_Generator( const std::map<int, const Material> *materials, Json::Value data);
    /**
     * @brief Construct a new Land_Generator object (default constructor)
     * 
     * This should not be used.
     */
    Land_Generator();

    /**
     * @brief Test if iteration is complete
     * 
     * @return true if iteration is complete,
     * @return false otherwise
     */
    inline bool empty()const{
        return (current_region >= data_.size());
    }
    /**
     * @brief Generate and return Tile Stamp object
     * 
     * @return Tile_Stamp 
     */
    Tile_Stamp get_this_stamp() const;
    /**
     * @brief increment the data of the Tile Stamp that will be generated
     */
    void operator++(){
        current_sub_region++;
        if (current_sub_region == get_num_stamps(data_[current_region])){
            current_region++;
            current_sub_region=0;
        }
    }
    /**
     * @brief Resets this land generator
     * 
     */
    void reset(){
        current_region = 0;
        current_sub_region = 0;
    };

private:
    unsigned int current_region;
    unsigned int current_sub_region;

    Json::Value data_; //this should be a structure
    const std::map<int, const Material> *materials;

    unsigned int static get_num_stamps(Json::Value biome);

    std::array<int, 6> get_volume(int center[2][2], int Sxy, int Sz, int Dxy, int Dz ) const ;

    void From_Radius(int cr, int csr, Tile_Stamp & ts) const ;
    void From_Grid(int cr, int csr, Tile_Stamp &ts) const ;
    void From_Positions(int cr, int csr, Tile_Stamp &ts) const ;
};

#endif
