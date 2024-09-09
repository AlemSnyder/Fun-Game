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

#include "generation/terrain_genreration_types.hpp"
#include "gui/render/gpu_data/texture.hpp"
#include "types.hpp"
#include "util/color.hpp"

#include <glaze/glaze.hpp>

#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace terrain {

struct material_color_t {
    std::string color_name;
    ColorInt hex_color;

    void
    read_hex_color(std::string hex_string) {
        hex_color = std::stoll(hex_string, nullptr, 16);
    }

    std::string
    write_hex_color() const {
        return fmtquill::format("{:08X}", hex_color);
    }
};

struct grass_data_t {
    std::vector<int> levels;
    int midpoint;
};

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
struct material_t {
    std::vector<material_color_t> color;

    float speed_multiplier = 1;          // speed on this material
    bool solid = false;                  // Is the material solid?
    MaterialId material_id = AIR_MAT_ID; // The ID of the material (Air is 0)
    const std::string name = "Air";      // The material name
    std::optional<grass_data_t> gradient;

    // int8_t deterioration from wind
    // int8_t deterioration from water
};

struct all_materials_t {
    std::map<std::string, material_t> data;
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

    /**
     * @brief Initializes data for color_ids_map and colors_inverse_map.
     *
     * @param const std::map<MaterialId, const material_t>& materials materials map
     */
    static void
    assign_color_mapping(const std::map<MaterialId, const material_t>& materials);

    /**
     * @brief Return vector that maps terrain color id to color
     *
     * @return std::vector<ColorInt> vector of colors than can be
     * indexed with terrain color ids
     */
    [[nodiscard]] inline static std::vector<ColorInt>&
    get_color_ids_map() {
        return color_ids_map;
    }

    /**
     * @brief Return a map that converts material color id to a color
     *
     * @return `const std::unordered_map<ColorInt, MatColorId>&` map
     */
    [[nodiscard]] inline static const std::unordered_map<ColorInt, MatColorId>&
    get_colors_inverse_map() {
        return colors_inverse_map;
    }

    /**
     * @brief Get the Opengl texture of the colors
     *
     * @return `gui::gpu_data::Texture1D&` 1D color map on gpu
     */
    [[nodiscard]] inline static gui::gpu_data::Texture1D&
    get_color_texture() {
        TerrainColorMapping& obj = instance();

        return obj.color_texture_;
    };

    /**
     * @brief Get the number of colors
     *
     * @return `size_t` the number of colors
     */
    [[nodiscard]] inline static size_t
    get_num_colors() {
        return color_ids_map.size();
    }
};

/**
 * @brief A group of materials. Used to generate terrain.
 *
 * @details Can be used to determine if a material is of a cretin type.
 */
class MaterialGroup {
 private:
    bool contain_all_materials;
    // Any material in this set is in the group no matter the color.
    std::set<MaterialId> materials_no_color_requirement_;
    // Map of materials to allowable color. For a material and color to be in
    // this group the material key must map to a set containing the given color.
    std::map<MaterialId, std::unordered_set<ColorId>> materials_with_color_requirement_;

 public:
    /**
     * @brief Construct new MaterialGroup object.
     *
     * @details Default constructor. Nothing will be in the group.
     */
    MaterialGroup() : contain_all_materials(false){};

    /**
     * @brief Construct new MaterialGroup object.
     *
     * @param std::set<MaterialId> materials materials in group no matter the color
     * @param std::map<MaterialId, std::set<ColorId>> materials_w_color materials in
     * group when they have specific color
     */
    MaterialGroup(
        std::set<MaterialId> materials,
        std::map<MaterialId, std::unordered_set<ColorId>> materials_w_color
    ) :
        contain_all_materials(false),
        materials_no_color_requirement_(materials),
        materials_with_color_requirement_(materials_w_color){};

    /**
     * @brief Read the materials and colors that this stamp can overwrite in
     * terrain. Use the "Can_Stamp" dictionary.
     */
    MaterialGroup(const std::vector<generation::material_designation_t>& data);

    /**
     * @brief Check if given material and color id are in the group.
     *
     * @param MaterialId material_id
     * @param ColorId color_id
     *
     * @return True given material is in the group
     * @return False material is not in the group
     */
    [[nodiscard]] inline bool
    material_in(MaterialId material_id, ColorId color_id) const {
        if (contain_all_materials)
            return true;
        if (material_in(material_id))
            return true; // material found in set that disregards color
        auto iter = materials_with_color_requirement_.find(material_id);
        if (iter == materials_with_color_requirement_.end())
            return false; // material not found
        // material found; find color
        return iter->second.contains(color_id);
    }

    /**
     * @brief Check if given material with arbitrary color id would be in the group.
     *
     * @param MaterialId material_id
     *
     * @return True given material is in the group
     * @return False material is not in the group
     */
    [[nodiscard]] inline bool
    material_in(MaterialId material_id) const {
        if (contain_all_materials)
            return true;
        return materials_no_color_requirement_.contains(material_id);
    }

 private:
    void insert_(std::unordered_set<MaterialId> material_id);
    void insert_(
        std::unordered_set<MaterialId> material_id,
        std::unordered_set<ColorId> color_ids
    );

    inline void
    set_all() {
        materials_no_color_requirement_.clear();
        materials_with_color_requirement_.clear();
        contain_all_materials = true;
    }
};

} // namespace terrain

template <>
struct glz::meta<terrain::material_color_t> {
    using T = terrain::material_color_t;
    // clang-format off
    static constexpr auto value = object(
        "hex_color",  custom<&T::read_hex_color, &T::write_hex_color>,
        "color_name", &T::color_name
    );
    // clang-format on
};

template <>
inline glz::detail::any_t::operator terrain::grass_data_t() const {
    assert(false && "Not Implemented");
    return {};
}
