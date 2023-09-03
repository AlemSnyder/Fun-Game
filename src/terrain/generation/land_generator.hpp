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

namespace generation {

namespace stamps {

class JsonToTile {
 protected:
    Dim height_;
    Dim height_variance_;
    Dim width_;
    Dim width_variance_;

    const std::set<std::pair<MaterialId, ColorId>> elements_can_stamp_;
    const MaterialId stamp_material_id_;
    const ColorId stamp_color_id_;

 public:
    JsonToTile(const Json::Value& data);
    virtual TileStamp get_this_stamp(ssize_t current_sub_region) const = 0;
    virtual size_t num_sub_region() const = 0;

    virtual ~JsonToTile() {}

    static std::set<std::pair<MaterialId, ColorId>>
    read_elements_can_stamp(const Json::Value& data);

 protected:
    TileStamp get_volume(
        glm::imat2x2 center, TerrainOffset Sxy, TerrainOffset Sz, TerrainOffset Dxy,
        TerrainOffset Dz
    ) const;
};

class FromPosition : public JsonToTile {
 private:
    std::vector<glm::ivec2> points_;
    TerrainOffset center_variance_;

 public:
    TileStamp get_this_stamp(ssize_t current_sub_region) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return points_.size();
    }

    FromPosition(const Json::Value& data);
};

class FromRadius : public JsonToTile {
 private:
    TerrainOffset radius_;
    TerrainOffset number_;
    TerrainOffset center_variance_;

 public:
    TileStamp get_this_stamp(ssize_t current_sub_region) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return number_;
    }

    FromRadius(const Json::Value& data);
};

class FromGrid : public JsonToTile {
 private:
    TerrainOffset radius_;
    TerrainOffset number_;
    TerrainOffset center_variance_;

 public:
    TileStamp get_this_stamp(ssize_t current_sub_region) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return number_ * number_;
    }

    FromGrid(const Json::Value& data);
};

enum struct Side {
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3,
};

} // namespace stamps

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
    size_t current_region;
    size_t current_sub_region;

    // const std::map<MaterialId, const Material>& materials;
    // const std::set<Material*> elements_can_stamp_;
    // const Material* material_;
    std::vector<std::shared_ptr<stamps::JsonToTile>> stamp_generators_;
    // Json::Value data_; // this should be a structure

 public:
    /**
     * @brief Construct a new LandGenerator object
     *
     * @param materials the materials used in this biome
     * @param data the description of how tiles stamps should be generated
     */
    LandGenerator(const Json::Value& data);

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
        return (current_region >= stamp_generators_.size());
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
        current_region = 0;
        current_sub_region = 0;
    };

 private:
    size_t static get_num_stamps(const Json::Value& biome);
};

} // namespace generation

} // namespace terrain
