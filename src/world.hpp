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
    World(const char* path);
    /**
     * @brief Construct a new World object to test biome generation.
     *
     * @param biome_data biome parameters
     * @param type determines the type of terrain to be generated
     * (see) data/biome_data.json > `biome` > Tile_Data
     * (see) src/terrain/terrain_generation/land_generator.hpp
     */
    World(Json::Value biome_data, int type);
    World(Json::Value materials_json, Json::Value biome_data);

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
     * @param indices index of vertex data drawn in this order
     * @param indexed_vertices vertex in 3D space
     * @param indexed_colors color of vertex
     * @param indexed_normals normal of face vertex is a part of
     */
    void get_mesh_greedy(
        std::vector<unsigned short>& indices, std::vector<glm::vec3>& indexed_vertices,
        std::vector<glm::vec3>& indexed_colors, std::vector<glm::vec3>& indexed_normals
    ) const;
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
