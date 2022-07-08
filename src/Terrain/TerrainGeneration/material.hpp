
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
 * @file material.hpp
 *
 * @brief Defines Material struct
 *
 * @ingroup Terrain
 *
 */

#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include <cstdint>

/*
    block types:
    rock (five types ~)
    dirt (grass (4) and dirt (3) )
    mineral types
        cloth   W most water should go through
        thatch  W ~W v. w
        mud     W W v. w
        wood    M M v. w
        brick   S S v. w
        stone   S S v. w

        ladder open and can go up and down
        closed a placed object blocks this tile
        open   like a dore or open window
        air    open and no placed objects
*/

/**
 * @brief Holds Material data
 * 
 * @details World class should have a map of materials organized by Material
 * Id. Each tile has a pointer to one of those materials (though this should
 * be an uint8_t as there are less than 256 materials).
 * 
 * The material determines if the tile is solid, and the potential color. Other
 * data will be added like how cretin materials respond to weather...
 */
struct Material{
    // vector of <name hex color> for possible colors
    std::vector<std::pair<const char *, uint32_t>> color;
    uint8_t speed_multiplier = 1; // speed on this material compared to base
    bool solid = false; // Is the material solid?
    uint8_t element_id = 0; // The ID of the material (Air is 0)
    const char * element = "Air"; // The material name (why is this not name?)
    //int8_t deteraition from wind
    //int8_t deteraition from water
};

#endif
