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
 * @file world.cpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines World class
 *
 */

#pragma once

#include "entity/terrain_mesh.hpp"
#include "terrain/material.hpp"
#include "terrain/terrain.hpp"
#include "types.hpp"

#include <json/json.h>

#include <glm/glm.hpp>

#include <map>
#include <vector>

// forward declaration of entity::Mesh
namespace entity {
class Mesh;
}

/**
 * @brief Holds information regarding terrain, entities, objects, and items
 *
 * @details The world holds a Terrain objects, and contains entities like
 * flora, and fauna. Paced objects, and other things will also be stored in
 * this class.
 *
 */
class World {
    // materials that exist
    std::map<MaterialId, const terrain::Material> materials;

    // terrain in the world
    terrain::Terrain terrain_main_;

    // TerrainMesh for each chunk in terrain
    std::vector<std::shared_ptr<terrain::TerrainMesh>> chunks_mesh_;

    // chunks_mesh like attorneys general

 public:
    const auto&
    get_terrain_main() const {
        return terrain_main_;
    }

    auto&
    get_terrain_main() {
        return terrain_main_;
    }

    const auto&
    get_chunks_mesh() const {
        return chunks_mesh_;
    }

    // all of these things are for saving
    // const char *path;
    // const char *name;
    // const char *another_name;

    /**
     * @brief Construct a new World object from a save
     *
     * @param path where world was saved
     */
    World(const Json::Value& materials_json, const std::string path);
    /**
     * @brief Construct a new World object to test biome generation.
     *
     * @param biome_data biome parameters
     * @param type determines the type of terrain to be generated
     * (see) data/biome_data.json > `biome` > Tile_Data
     * (see) src/terrain/terrain_generation/land_generator.hpp
     */
    World(const Json::Value& materials_json, const Json::Value& biome_data, int type);
    World(
        const Json::Value& materials_json, const Json::Value& biome_data,
        uint32_t x_tiles, uint32_t y_tiles
    );

    constexpr static int macro_tile_size = 32;
    constexpr static int height = 128;

    // void save(); TODO define save
    //  this would require creating some sort of file type

    /**
     * @brief Get the materials that exist in the world
     *
     * @return const std::map<int, const Material>* map of materials_id to
     * materials pointer
     */
    inline const std::map<MaterialId, const terrain::Material>*
    get_materials() const noexcept {
        return &materials;
    }

    /**
     * @brief Get material from material_id
     *
     * @param material_id
     * @return const Material* corresponding material
     */
    const terrain::Material* get_material(int material_id) const;

    /**
     * @brief Load materials from json data
     *
     * @param material_data data to load from
     * (see) data/materials.json
     */
    std::map<MaterialId, const terrain::Material>
    init_materials(const Json::Value& material_data);

    /**
     * @brief Get the grass gradient data
     *
     * @param material_json data to load from
     * @return std::vector<int> width of each grass color
     */
    std::vector<int> get_grass_grad_data(const Json::Value& material_json);

    /**
     * @brief update all chunk mesh
     *
     */
    void update_all_chunks_mesh();

    void update_single_mesh(Dim chunk_pos);

    void update_single_mesh(TerrainDim3 tile_sop);

    // set a region to given material, and color
    void set_tile(Dim pos, const terrain::Material* mat, ColorId color_id);

    void stamp_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const terrain::Material* mat,std::set<std::pair<int, int>> elements_can_stamp,
        ColorId color_id
    );

    void stamp_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const terrain::Material* mat, ColorId color_id
    );

    inline void
    qb_save_debug(const std::string& path) {
        terrain_main_.qb_save_debug(path);
    }

    inline void
    qb_save(const std::string& path) const {
        terrain_main_.qb_save(path);
    }

 private:
    inline void
    initialize_chunks_mesh_() {
        chunks_mesh_.resize(terrain_main_.get_chunks().size());
        for (auto& m : chunks_mesh_)
            m = std::make_shared<terrain::TerrainMesh>();
    }
};
