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

#include "biome.hpp"
#include "entity/tile_object.hpp"
#include "gui/render/gpu_data/terrain_mesh.hpp"
#include "terrain/material.hpp"
#include "terrain/terrain.hpp"
#include "types.hpp"

#include <glm/glm.hpp>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace world {

// forward declaration of entity::Mesh
namespace entity {
class Mesh;
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
    std::vector<std::shared_ptr<gui::gpu_data::TerrainMesh>> chunks_mesh_;
    // chunks_mesh like attorneys general

    // Set of chunks that need to be updated on gpu.
    // They Need to be re-meshed, and that mesh need to be sent to gpu.
    std::unordered_set<ChunkIndex> chunks_to_update_;

    // Set of meshes that need to be sent to gpu. These meshes should be sent
    // once per frame.
    std::unordered_map<ChunkIndex, entity::Mesh> meshes_to_update_;
    // Multiple threads are writing to this map concurrently so this is its
    // mutex
    std::mutex meshes_to_update_mutex_;

    // entity
    std::unordered_set<entity::TileObject, entity::TileObjectOrder> tile_entities_;

 public:
    /**
     * @brief Get terrain
     */
    const auto&
    get_terrain_main() const {
        return terrain_main_;
    }

    /**
     * @brief Get terrain
     */
    auto&
    get_terrain_main() {
        return terrain_main_;
    }

    /**
     * @brief Get gui mesh data
     */
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
     * @return const std::unordered_map<int, const material_t>* map of materials_id to
     * materials pointer
     */
    inline const std::unordered_map<MaterialId, const terrain::material_t>&
    get_materials() const noexcept {
        return biome_.get_materials();
    }

    /**
     * @brief Get material from material_id
     *
     * @param material_id
     * @return const material_t* corresponding material
     */
    const terrain::material_t* get_material(MaterialId material_id) const;

    // Updating chunks works by changing a tile in terrain. This marks the
    // chunk's mesh for update, and surrounding chunks is applicable.
    // Update marked chunks mesh runs update single mesh on marked chunks in a
    // background thread. The returned meshes are saved and sent to the gpu when
    // send_updated_chunks_mesh is called.

    /**
     * @brief Marks a single chunk given by its position to be updated.
     */
    void
    mark_for_update(ChunkIndex chunk_pos) {
        chunks_to_update_.insert(chunk_pos);
    }

    /**
     * @brief Marks a single chunk given by a tile in the chunk for update
     */
    void
    mark_for_update(TerrainDim3 tile_sop) {
        if (!terrain_main_.in_range(tile_sop))
            return;
        Dim chunk_pos = terrain_main_.get_chunk_from_tile(tile_sop);
        mark_for_update(chunk_pos);
    }

    /**
     * @brief Runs a loop over marked chunks and generates a mesh.
     */
    void update_marked_chunks_mesh();

    /**
     * @brief Generates a mesh for a single chunk.
     */
    void update_single_mesh(ChunkIndex chunk_pos);

    /**
     * @brief Sends chunk mesh data to gpu.
     */
    void send_updated_chunks_mesh();

    /**
     * @brief Update all chunk mesh.
     *
     * @details Updates and initializes all chunks mesh.
     *
     */
    void update_all_chunks_mesh();

    // set a region to given material, and color
    /**
     * @brief Set a tile to have a material and color
     *
     * @param Dim pos tile position
     * @param terrain::material_t* material to change to
     * @param ColorId color id to change to
     */
    void set_tile(Dim pos, const terrain::material_t* mat, ColorId color_id);

    /**
     * @brief Save terrain with debug information
     *
     * @details Adds the chunk navigation regions
     *
     * @param std::string& path to save file to
     */
    inline void
    qb_save_debug(const std::string& path) {
        terrain_main_.qb_save_debug(path);
    }

    /**
     * @brief Save terrain
     *
     * @param std::string& path to save file to
     */
    inline void
    qb_save(const std::string& path) const {
        terrain_main_.qb_save(path);
    }

 private:
    inline void
    initialize_chunks_mesh_() {
        chunks_mesh_.resize(terrain_main_.get_chunks().size());
        for (auto& m : chunks_mesh_)
            m = std::make_shared<gui::gpu_data::TerrainMesh>();
    }
};

} // namespace world
