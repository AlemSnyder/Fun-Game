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

/**
 * @brief Reads JSON data and converts it to a stamp generator.
 *
 * @details This is a virtual class that handles creating tile stamps from JSON
 * Data.
 */
class JsonToTile {
 protected:
    Dim height_;          // Average height generated
    Dim height_variance_; // Maximum chance from height
    Dim width_;           // Average width generated
    Dim width_variance_;  // Maximum change in width generated

    // set of tile materials-colors that can be changed by the returned stamp
    const std::set<std::pair<MaterialId, ColorId>> elements_can_stamp_;
    const MaterialId stamp_material_id_; // MaterialId that will be set
    const ColorId stamp_color_id_;       // ColorId that will be set

 public:
    /**
     * @brief Default initializer use dictionary from "Tile_Macros" "Land_Data".
     */
    JsonToTile(const Json::Value& data);
    /**
     * @brief Returns a tile stamp depending on the current sub region.
     */
    virtual TileStamp get_stamp(size_t current_sub_region) const = 0;
    /**
     * @brief Returns the number of unique stamps this stamp generator can
     * generate.
     *
     * @details Do not try to use a sub region larger than the number of sub
     * regions.
     */
    virtual size_t num_sub_region() const = 0;

    virtual ~JsonToTile() {}

    /**
     * @brief Read the materials and colors that this stamp can overwrite in
     * terrain. Use the "Can_Stamp" dictionary.
     */
    static std::set<std::pair<MaterialId, ColorId>>
    read_elements(const Json::Value& data);

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
    TileStamp get_stamp(size_t current_sub_region) const override;

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
    TileStamp get_stamp(size_t current_sub_region) const override;

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
    TileStamp get_stamp(size_t current_sub_region) const override;

    [[nodiscard]] inline size_t
    num_sub_region() const override {
        return number_ * number_;
    }

    FromGrid(const Json::Value& data);
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
    TileStamp get_stamp() const;

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

enum class AddDirections : uint8_t {
    None = 0,
    To = 1,
    Add = 2,
};

struct AddMethod {
    // enum class about add n vs to n

    Dim start;
    Dim stop;
    Dim data;

    AddDirections add_directions;

    bool
    operator<(const AddMethod& other) const {
        return start < other.start;
    }
};

class AddToTop {
    const std::set<std::pair<MaterialId, ColorId>> elements_above_;

    const std::set<std::pair<MaterialId, ColorId>> elements_can_overwrite_;
    const MaterialId stamp_material_id_; // MaterialId that will be set
    const ColorId stamp_color_id_;       // ColorId that will be set

    std::set<AddMethod> data_;
    // use ordering by start
    // index into using find/ at
    // will likely give the correct AddMethod, but still should be checked

 public:
    AddToTop(const Json::Value& json_data);

    Dim get_final_height(Dim height) const;

    [[nodiscard]] inline bool
    can_overwrite_material(MaterialId material_id, ColorId color_id) const {
        return material_in(elements_can_overwrite_, material_id, color_id);
    }

    inline MaterialId
    get_material_id() const {
        return stamp_material_id_;
    }

    inline ColorId
    get_color_id() const {
        return stamp_color_id_;
    }

    inline const std::set<std::pair<MaterialId, ColorId>>&
    get_elements_above() const {
        return elements_above_;
    }
};

} // namespace generation

} // namespace terrain
