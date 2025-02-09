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
#include "terrain_genreration_types.hpp"
#include "tile_stamp.hpp"
#include "types.hpp"

#include <glm/gtc/matrix_integer.hpp>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <string>

namespace terrain {

namespace generation {

namespace stamps {

/**
 * @brief Reads JSON data and converts it to a stamp generator.
 *
 * @details This is a virtual class that handles creating tile stamps from JSON
 * Data.
 */
class StampGenerator {
 protected:
    Dim height_;          // Average height generated
    Dim height_variance_; // Maximum chance from height
    Dim width_;           // Average width generated
    Dim width_variance_;  // Maximum change in width generated

    // set of tile materials-colors that can be changed by the returned stamp
    const MaterialGroup elements_can_stamp_;
    const MaterialId stamp_material_id_; // MaterialId that will be set
    const ColorId stamp_color_id_;       // ColorId that will be set

 public:
    /**
     * @brief Default initializer use dictionary from "Tile_Macros" "Land_Data".
     */
    StampGenerator(const generation_stamp_t& data) :
        height_(data.height), height_variance_(data.height_range), width_(data.size),
        width_variance_(data.center_range), elements_can_stamp_(data.can_override),
        stamp_material_id_(data.material_id), stamp_color_id_(data.color_id) {}

    /**
     * @brief Returns a tile stamp depending on the current sub region.
     */
    virtual TileStamp get_stamp(
        size_t current_sub_region, std::default_random_engine& rand_engine
    ) const = 0;
    /**
     * @brief Returns the number of unique stamps this stamp generator can
     * generate.
     *
     * @details Do not try to use a sub region larger than the number of sub
     * regions.
     */
    virtual size_t num_sub_region() const = 0;

    virtual ~StampGenerator() {}

    [[nodiscard]] virtual Dim
    height() const {
        return height_;
    }

    [[nodiscard]] virtual Dim
    height_variance() const {
        return height_variance_;
    }

    [[nodiscard]] virtual Dim
    width() const {
        return width_;
    }

    [[nodiscard]] virtual Dim
    width_variance() const {
        return width_variance_;
    }

    [[nodiscard]] virtual MaterialId
    material() const {
        return stamp_material_id_;
    }

    [[nodiscard]] virtual ColorId
    color() const {
        return stamp_color_id_;
    }

 protected:
    /**
     * @brief Gets a TileStamp given
     *
     * @param center glm::imat2x2 region with in center can be placed
     * @param Sxy TerrainOffset width
     * @param Sz TerrainOffset height
     * @param Dxy TerrainOffset range of allowable difference in width
     * @param Sz TerrainOffset range of allowable difference in height
     *
     * @return TileStamp
     */
    TileStamp
    get_volume(glm::imat2x2 center, std::default_random_engine& rand_engine) const;
};

class FromPosition : public StampGenerator {
 private:
    std::vector<glm::ivec2> points_;
    TerrainOffset center_variance_;

 public:
    TileStamp get_stamp(
        size_t current_sub_region, std::default_random_engine& rand_engine
    ) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return points_.size();
    }

    FromPosition(
        const generation_stamp_t& data,
        const stamp_generation_position_data_t& type_data
    );

    FromPosition(const generation_stamp_t& data) :
        FromPosition(data, data.position.value()) {}
};

class FromRadius : public StampGenerator {
 private:
    TerrainOffset radius_;
    TerrainOffset number_;
    TerrainOffset center_variance_;

 public:
    TileStamp get_stamp(
        size_t current_sub_region, std::default_random_engine& rand_engine
    ) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return number_;
    }

    FromRadius(
        const generation_stamp_t& data, const stamp_generation_radius_data_t& type_data
    ) :
        StampGenerator(data),
        radius_(type_data.radius), number_(type_data.number),
        center_variance_(data.center_range) {}

    FromRadius(const generation_stamp_t& data) :
        FromRadius(data, data.radius.value()) {}
};

class FromGrid : public StampGenerator {
 private:
    TerrainOffset radius_;
    TerrainOffset number_;
    TerrainOffset center_variance_;

 public:
    TileStamp get_stamp(
        size_t current_sub_region, std::default_random_engine& rand_engine
    ) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return number_ * number_;
    }

    FromGrid(
        const generation_stamp_t& data, const stamp_generation_grid_data_t& type_data
    ) :
        StampGenerator(data),
        radius_(type_data.radius), number_(type_data.number),
        center_variance_(data.center_range) {}

    FromGrid(const generation_stamp_t& data) : FromGrid(data, data.grid.value()) {}
};

enum class Side : uint8_t {
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3,
};

} // namespace stamps

/**
 * @brief Reads JSON data and generates TileStamp objects
 *
 * @details The way biomes are generated is saved in data/{mod}/biome_data.json.
 * The biome pipeline starts with getting a 2D tile map. Each tile in the map
 * a different type (usually height). Next is defining how to generate that
 * tile region. There are two parts. The smaller part can be used by multiple
 * larger parts. This is useful because some parts of a
 * map tile will be the same as another with a different value (all bedrock is
 * the same). Each map tile type is now assigned macros. Land Generator
 * iterates though these macros, and creates Tile Stamps.
 *
 */
class LandGenerator {
    size_t current_region; // index of the next region to be generated
    size_t current_sub_region;

    // const std::unordered_map<MaterialId, const Material>& materials;
    // const std::set<Material*> elements_can_stamp_;
    // const Material* material_;
    std::vector<std::shared_ptr<stamps::StampGenerator>> stamp_generators_;
    // Json::Value data_; // this should be a structure

 public:
    /**
     * @brief Construct a new LandGenerator object
     *
     * @param materials the materials used in this biome
     * @param data the description of how tiles stamps should be generated
     */
    LandGenerator(const std::vector<terrain::generation::generation_stamp_t>& data);

    /**
     * @brief Construct a new LandGenerator object (default constructor)
     *
     * This should not be used.
     */
    LandGenerator() = delete;

    /**
     * @brief Test if iteration is complete
     *
     * @return true if iteration is complete,
     * @return false otherwise
     */
    inline bool
    empty() const {
        return (current_region >= stamp_generators_.size());
    }

    /**
     * @brief Generate and return Tile Stamp object
     *
     * @return TileStamp
     */
    TileStamp
    get_stamp(std::default_random_engine& rand_engine) const {
        return stamp_generators_[current_region]->get_stamp(
            current_sub_region, rand_engine
        );
    }

    /**
     * @brief increment the data of the Tile Stamp that will be generated
     */
    void next();

    /**
     * @brief Resets this land generator
     */
    inline void
    reset() {
        current_region = 0;
        current_sub_region = 0;
    };

    [[nodiscard]] inline auto
    begin() const {
        return stamp_generators_.begin();
    }

    [[nodiscard]] inline auto end() const {
        return stamp_generators_.end();
}
}; // namespace generation

class AddToTop {
    const MaterialGroup elements_above_;

    const MaterialGroup elements_can_overwrite_;
    const MaterialId stamp_material_id_; // MaterialId that will be set
    const ColorId stamp_color_id_;       // ColorId that will be set

    std::set<layer_effect_data_t> data_;
    // use ordering by start
    // index into using find/ at
    // will likely give the correct AddMethod, but still should be checked

 public:
    AddToTop(const layer_effects_t& layer_effect_data);

    Dim get_final_height(Dim height) const;

    [[nodiscard]] inline bool
    can_overwrite_material(MaterialId material_id, ColorId color_id) const {
        return elements_can_overwrite_.material_in(material_id, color_id);
    }

    inline MaterialId
    get_material_id() const {
        return stamp_material_id_;
    }

    inline ColorId
    get_color_id() const {
        return stamp_color_id_;
    }

    inline const MaterialGroup&
    get_elements_above() const {
        return elements_above_;
    }

    inline const MaterialGroup&
    get_elements_can_overwrite() const {
        return elements_can_overwrite_;
    }
};

} // namespace terrain

} // namespace terrain
