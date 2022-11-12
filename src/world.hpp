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

#include "entity/mesh.hpp"
#include "terrain/terrain.hpp"

#include <json/json.h>

#include <glm/glm.hpp>

#include <map>
#include <vector>

/**
 * @brief Holds information regarding terrain, entities, objects, and items
 *
 * @details The world holds a Terrain objects, and contains entities like
 * flora, and fauna. Paced objects, and other things will also be stored in
 * this class.
 *
 */
class World {
 public:
    /**
     * @brief Construct a new World object
     *
     */
    World();
    /**
     * @brief Construct a new World object from a save
     *
     * @param path where world was saved
     */
    World(const std::string path);
    /**
     * @brief Construct a new World object to test biome generation.
     *
     * @param biome_data biome parameters
     * @param type determines the type of terrain to be generated
     * (see) data/biome_data.json > `biome` > Tile_Data
     * (see) src/terrain/terrain_generation/land_generator.hpp
     */
    World(Json::Value biome_data, int type);
    World(
        Json::Value materials_json, Json::Value biome_data, uint32_t x_tiles,
        uint32_t y_tiles
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
    const std::map<int, const terrain::Material>* get_materials() const {
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
    void init_materials(Json::Value material_data);

    /**
     * @brief Get the mesh using greedy meshing
     *
     * @return entity::Mesh the mesh generated
     */
    inline std::vector<entity::Mesh> get_mesh_greedy() const {
        // TODO should be for each chunk
        //return entity::generate_mesh(terrain_main);
        std::vector<entity::Mesh> out;
        for (const terrain::Chunk &c : terrain_main.get_chunks()){
            auto chunk_mesh = entity::generate_mesh(c);
            if (chunk_mesh.indices_.size() > 0){
                out.push_back(chunk_mesh);
            }
        }
        return out;
    }

    // terrain in the world
    terrain::Terrain terrain_main;

 private:
    // materials that exist
    std::map<int, const terrain::Material> materials;

    // all of these things are for saving
    // const char *path;
    // const char *name;
    // const char *another_name;
};
