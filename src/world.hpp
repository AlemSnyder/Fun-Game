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

#include "gui/render/graphics_data/terrain_mesh.hpp"
#include "terrain/generation/biome.hpp"
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

inline std::vector<terrain::generation::MapTile>
get_test_map(MapTile_t type) {
    std::vector<terrain::generation::MapTile> out;
    out.reserve(9);
    for (size_t i = 0; i < 4; i++)
        out.emplace_back(0, 0);
    out.emplace_back(type, 2);
    for (size_t i = 0; i < 4; i++)
        out.emplace_back(0, 0);

    return out;
}

/**
 * @brief Holds information regarding terrain, entities, objects, and items
 *
 * @details The world holds a Terrain objects, and contains entities like
 * flora, and
 * fauna. Paced objects, and other things will also be stored in
 * this class.
 *
 */
class World {
    // Biome of the world. Will contain the materials, and grass data
    terrain::generation::Biome biome_;

    // terrain in the world
    terrain::Terrain terrain_main_;

    // TerrainMesh for each chunk in terrain
    std::vector<std::shared_ptr<gui::data_structures::TerrainMesh>> chunks_mesh_;
    // chunks_mesh like attorneys general

    std::set<ChunkIndex> chunks_to_update_;

    std::map<ChunkIndex, entity::Mesh> meshes_to_update_;
    std::mutex meshes_to_update_mutex_;

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
     * @param path
     * where world was saved
     */
    World(const std::string& biome_name, const std::string& path, size_t seed);
    /**
     * @brief Construct a new World object to test biome generation.
     *
     * @param biome_data biome parameters
     * @param type determines the type of terrain to be generated
     * (see) data/biome_data.json > `biome` > Tile_Data
     * (see) src/terrain/generation/land_generator.hpp
     */
    World(const std::string& biome_name, MapTile_t type, size_t seed);
    World(
        const std::string& biome_name, MacroDim x_tiles, MacroDim y_tiles, size_t seed
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
    inline const std::map<MaterialId, const terrain::Material>&
    get_materials() const noexcept {
        return biome_.get_materials();
    }

    /**
     * @brief Get material from material_id
     *
     * @param material_id
     * @return const Material* corresponding material
     */
    const terrain::Material* get_material(MaterialId material_id) const;

    /**
     * @brief Get the grass gradient data
     *
     * @param material_json data to load from
     *
     * @return std::vector<int> width of each grass color
     */
    std::vector<int> get_grass_grad_data(const Json::Value& material_json);

    /**
     * @brief update all chunk mesh
     *
     */
    void update_all_chunks_mesh();

    void update_single_mesh(ChunkIndex chunk_pos);
    void update_single_mesh(TerrainDim3 tile_sop);

    void
    mark_for_update(ChunkIndex chunk_pos) {
        chunks_to_update_.insert(chunk_pos);
    }

    void
    mark_for_update(TerrainDim3 tile_sop) {
        if (!terrain_main_.in_range(tile_sop))
            return;
        Dim chunk_pos = terrain_main_.get_chunk_from_tile(tile_sop);
        mark_for_update(chunk_pos);
    }

    void update_marked_chunks_mesh();
    void send_updated_chunks_mesh();

    // set a region to given material, and color
    void set_tile(Dim pos, const terrain::Material* mat, ColorId color_id);

    // set a region to given material, and color
    // void set_tiles();

    void stamp_tile_region(
        TerrainOffset x_start, TerrainOffset y_start, TerrainOffset z_start,
        TerrainOffset x_end, TerrainOffset y_end, TerrainOffset z_end,
        const terrain::Material* mat, std::set<std::pair<int, int>> elements_can_stamp,
        ColorId color_id
    );

    void stamp_tile_region(
        TerrainOffset x_start, TerrainOffset y_start, TerrainOffset z_start,
        TerrainOffset x_end, TerrainOffset y_end, TerrainOffset z_end,
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
            m = std::make_shared<gui::data_structures::TerrainMesh>();
    }
};
