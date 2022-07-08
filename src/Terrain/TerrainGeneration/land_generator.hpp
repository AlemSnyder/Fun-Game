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

class Land_Generator{
public:
    Land_Generator( const std::map<int, const Material> *materials_, Json::Value data);
    Land_Generator();


    inline bool empty()const{
        return (current_region >= data_.size());
    }
    Tile_Stamp get_this_stamp() const;

    void operator++(){
        current_sub_region++;
        if (current_sub_region == get_num_stamps(data_[current_region])){
            current_region++;
            current_sub_region=0;
        }
    }
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
