
// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software
 *
 * Foundation, version 2 of the License, or
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
 */

#pragma once

#include "../types.hpp"

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// not including all of glew
using GLuint_p = unsigned int;

namespace terrain {

/**
 * @brief Holds Material data
 *
 * @details World class should have a map of materials organized by Material
 * Id. Each tile has the id of a material (currently uint8_t).
 *
 * materials organized by Material Id. Each tile has a material id.
 * The material determines if the tile is solid, and the potential color. Other
 * data will be added like how cretin materials respond to weather...
 */
struct Material {
    Material(
        std::vector<std::pair<const std::string, ColorInt>> color_in,
        float speed_multiplier_in, bool solid_in, MaterialId element_id_in,
        std::string name_in
    ) :
        color(color_in),
        speed_multiplier(speed_multiplier_in), solid(solid_in),
        element_id(element_id_in), name(name_in){};
    // vector of <name hex color> for possible colors
    std::vector<std::pair<const std::string, ColorInt>> color;
    float speed_multiplier = 1;     // speed on this material compared to base
    bool solid = false;             // Is the material solid?
    MaterialId element_id = 0;      // The ID of the material (Air is 0)
    const std::string name = "Air"; // The material name
    // int8_t deterioration from wind
    // int8_t deterioration from water
};

class TerrainColorMapping {
 private:
    // color map
    // index -> color vector
    static std::vector<ColorInt> color_ids_map;
    // color -> index
    static std::unordered_map<ColorInt, MatColorId> colors_inverse_map;
    // texture id saved on gpu.
    static GLuint_p color_texture_;

 public:
    static void
    assign_color_mapping(const std::map<MaterialId, const Material>& materials);
    // may discard
    static GLuint_p assign_color_texture();

    inline static std::vector<ColorInt>&
    get_color_ids_map() {
        return color_ids_map;
    }

    inline static std::unordered_map<ColorInt, MatColorId>
    get_colors_inverse_map() {
        return colors_inverse_map;
    }

    inline static unsigned int
    get_color_texture() {
        return color_texture_;
    };
};

[[nodiscard]] inline bool
material_in(
    const std::set<std::pair<MaterialId, ColorId>> materials, MaterialId material_id,
    ColorId color_id
) {
    for (auto element : materials) {
        if ((element.first == MAT_ANY_MATERIAL || element.first == material_id)
            && (element.second == COLOR_ANY_COLOR || element.second == color_id)) {
            return true;
        }
    }

    return false;
}

} // namespace terrain
