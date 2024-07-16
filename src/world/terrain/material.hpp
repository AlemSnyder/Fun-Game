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

#include "gui/render/gpu_data/texture.hpp"
#include "types.hpp"
#include "util/color.hpp"

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

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
    float speed_multiplier = 1;         // speed on this material
    bool solid = false;                 // Is the material solid?
    MaterialId element_id = AIR_MAT_ID; // The ID of the material (Air is 0)
    const std::string name = "Air";     // The material name
    // int8_t deterioration from wind
    // int8_t deterioration from water
};

/**
 * @brief Defines a map from colors to a color texture that is sent to the gpu.
 */
class TerrainColorMapping {
 private:
    // color map
    // index -> color vector
    static std::vector<ColorInt> color_ids_map;
    // color -> index
    static std::unordered_map<ColorInt, MatColorId> colors_inverse_map;
    // texture id saved on gpu.
    gui::gpu_data::Texture1D color_texture_;

    TerrainColorMapping() : color_texture_(color::convert_color_data(color_ids_map)) {}

 public:
    // Delete all CTORs and CTOR-like operators
    TerrainColorMapping(TerrainColorMapping&&) = delete;
    TerrainColorMapping(TerrainColorMapping const&) = delete;

    void operator=(TerrainColorMapping&&) = delete;
    void operator=(TerrainColorMapping const&) = delete;

    // Instance accessor
    [[nodiscard]] static inline TerrainColorMapping&
    instance() {
        assert(color_ids_map.size() > 0 && "must initialize first.");
        static TerrainColorMapping obj;
        return obj;
    }

    static void
    assign_color_mapping(const std::map<MaterialId, const Material>& materials);

    // may discard
    // static GLuint_p assign_color_texture();

    [[nodiscard]] inline static std::vector<ColorInt>&
    get_color_ids_map() {
        return color_ids_map;
    }

    [[nodiscard]] inline static std::unordered_map<ColorInt, MatColorId>
    get_colors_inverse_map() {
        return colors_inverse_map;
    }

    [[nodiscard]] inline static gui::gpu_data::Texture1D&
    get_color_texture() {
        TerrainColorMapping& obj = instance();

        return obj.color_texture_;
    };

    [[nodiscard]] inline static size_t
    get_num_colors() {
        return color_ids_map.size();
    }
};

class MaterialGroup {
 private:
    // Set of materials in the group. Any material and color no mater the color
    // is in this group if the material is in the below set.
    std::set<MaterialId> materials_no_color_requirement_;
    // Map of materials to allowable color. For a material and color to be in
    // this group the material key must map to a set containing the given color.
    std::map<MaterialId, std::set<ColorId>> materials_with_color_requirement_;

 public:
    MaterialGroup(){};
    MaterialGroup(
        std::set<MaterialId> materials,
        std::map<MaterialId, std::set<ColorId>> materials_w_color
    ) :
        materials_no_color_requirement_(materials),
        materials_with_color_requirement_(materials_w_color){};

    [[nodiscard]] inline bool
    material_in(MaterialId material_id, ColorId color_id) const {
        if (material_in(material_id))
            return true; // material found in set that disregards color
        auto iter = materials_with_color_requirement_.find(material_id);
        if (iter == materials_with_color_requirement_.end())
            return false; // material not found
        // material found; find color
        return iter->second.contains(color_id);
    }

    [[nodiscard]] inline bool
    material_in(MaterialId material_id) const {
        return materials_no_color_requirement_.contains(material_id);
    }
};

[[nodiscard]] inline bool
material_in(
    const std::set<std::pair<MaterialId, ColorId>> materials, MaterialId material_id,
    ColorId color_id
) {
    auto same_mat = [&material_id](MaterialId m) {
        return m == MAT_ANY_MATERIAL || m == material_id;
    };

    auto same_color = [&color_id](ColorId c) {
        return c == COLOR_ANY_COLOR || c == color_id;
    };

    for (const auto& [mat, color] : materials) {
        if (same_mat(mat) && same_color(color))
            return true;
    }

    return false;
}

} // namespace terrain