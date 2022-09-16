
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

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace terrain {

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
struct Material {
    Material(std::vector<std::pair<const std::string, uint32_t>> color_in, uint8_t speed_multiplier_in,
    bool solid_in, uint8_t element_id_in, std::string name_in
    ) : color(color_in), speed_multiplier(speed_multiplier_in),
        solid(solid_in), element_id(element_id_in), name(name_in) {

    };
    // vector of <name hex color> for possible colors
    std::vector<std::pair<const std::string, uint32_t>> color;
    uint8_t speed_multiplier = 1;   // speed on this material compared to base
    bool solid = false;             // Is the material solid?
    uint8_t element_id = 0;         // The ID of the material (Air is 0)
    const std::string name = "Air"; // The material name
    // int8_t deterioration from wind
    // int8_t deterioration from water
};

} // namespace terrain
