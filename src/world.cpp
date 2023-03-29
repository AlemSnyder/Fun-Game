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
 *
 */

#include "world.hpp"

#include "entity/mesh.hpp"
#include "terrain/material.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"

#include <cstdint>
#include <fstream>
#include <string>

const terrain::Material*
World::get_material(int material_id) const {
    return &materials.at(material_id);
}

std::vector<int>
World::get_grass_grad_data(Json::Value materials_json) {
    std::vector<int> grass_grad_data;
    for (unsigned int i = 0; i < materials_json["Dirt"]["Gradient"]["levels"].size();
         i++) {
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt(
        ));
    }
    return grass_grad_data;
}

std::map<uint8_t, const terrain::Material>
World::init_materials(Json::Value material_data) {
    std::map<uint8_t, const terrain::Material> out;
    for (auto element_it = material_data.begin(); element_it != material_data.end();
         element_it++) {
        std::vector<std::pair<const std::string, uint32_t>> color_vector;

        Json::Value material = *element_it;
        std::string name = element_it.key().asString();
        for (unsigned int i = 0; i < material["colors"].size(); i++) {
            const std::string string = material["colors"][i]["name"].asString();
            uint32_t color = std::stoll(material["colors"][i]["hex"].asString(), 0, 16);
            color_vector.push_back(std::make_pair(string, color));
        }

        terrain::Material mat{
            color_vector,                                    // color
            static_cast<uint8_t>(material["speed"].asInt()), // speed_multiplier
            material["solid"].asBool(),                      // solid
            static_cast<uint8_t>(material["id"].asInt()),    // element_id
            name};                                           // name
        out.insert(std::make_pair(mat.element_id, mat));
    }

    terrain::TerrainColorMapping::assign_color_mapping(&out);
    return out;
}

World::World(Json::Value materials_json, const std::string path) :
    materials(init_materials(materials_json)),
    terrain_main(
        path, &materials, get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {
    chunks_mesh.resize(terrain_main.get_chunks().size());

    for (size_t i = 0; i < chunks_mesh.size(); i++) {
        chunks_mesh[i] = std::make_shared<terrain::TerrainMesh>();
    }
}

World::World(
    Json::Value materials_json, Json::Value biome_data, uint32_t x_tiles,
    uint32_t y_tiles
) :
    materials(init_materials(materials_json)),
    terrain_main(
        x_tiles, y_tiles, macro_tile_size, height, 5, &materials, biome_data["Biome_1"],
        get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {
    chunks_mesh.resize(terrain_main.get_chunks().size());
    for (size_t i = 0; i < chunks_mesh.size(); i++) {
        chunks_mesh[i] = std::make_shared<terrain::TerrainMesh>();
    }
}

World::World(Json::Value materials_json, Json::Value biome_data, int tile_type) :
    materials(init_materials(materials_json)),
    terrain_main(
        macro_tile_size, height, 5, tile_type, &materials, biome_data["Biome_1"],
        get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {
    chunks_mesh.resize(terrain_main.get_chunks().size());
    for (size_t i = 0; i < chunks_mesh.size(); i++) {
        chunks_mesh[i] = std::make_shared<terrain::TerrainMesh>();
    }
}

// ! deprecated
std::vector<entity::Mesh>
World::get_mesh_greedy() const {
    std::vector<entity::Mesh> out;
    for (const terrain::Chunk& c : terrain_main.get_chunks()) {
        auto chunk_mesh = entity::generate_mesh(c);

        chunk_mesh.change_color_indexing(
            materials, terrain::TerrainColorMapping::get_colors_inverse_map()
        );

        if (chunk_mesh.get_indices().size() > 0) {
            out.push_back(chunk_mesh);
        }
    }
    return out;
}

void
World::update_all_chunk_mesh() {
    const std::vector<terrain::Chunk>& chunks = terrain_main.get_chunks();
    for (size_t i = 0; i < chunks.size(); i++) {
        entity::Mesh chunk_mesh = entity::generate_mesh(chunks[i]);

        chunk_mesh.change_color_indexing(
            materials, terrain::TerrainColorMapping::get_colors_inverse_map()
        );

        chunks_mesh[i]->init(chunk_mesh);
    }
}

void
World::update_single_mesh(uint16_t chunk_pos) {
    const std::vector<terrain::Chunk>& chunks = terrain_main.get_chunks();
    entity::Mesh chunk_mesh = entity::generate_mesh(chunks[chunk_pos]);

    chunk_mesh.change_color_indexing(
        materials, terrain::TerrainColorMapping::get_colors_inverse_map()
    );

    chunks_mesh[chunk_pos]->init(chunk_mesh);
}

void
World::set_tile(uint16_t pos, const terrain::Material* mat, uint8_t color_id) {
    terrain_main.get_tile(pos)->set_material(mat, color_id);

    auto [x, y, z] = terrain_main.sop(pos);

    uint16_t chunk_pos = terrain_main.get_chunk_from_tile(x, y, z);
    update_single_mesh(chunk_pos);

    // do some math:
    // if the tile is on the edge of a chunk then both chunks must be updated.
    uint8_t edge_case = x % terrain::Chunk::SIZE;
    if (edge_case == 0) {
        if (terrain_main.in_range(x - 1, y, z)) {
            chunk_pos = terrain_main.get_chunk_from_tile(x - 1, y, z);
            update_single_mesh(chunk_pos);
        }
    } else if (edge_case == terrain::Chunk::SIZE - 1) {
        if (terrain_main.in_range(x + 1, y, z)) {
            chunk_pos = terrain_main.get_chunk_from_tile(x + 1, y, z);
            update_single_mesh(chunk_pos);
        }
    }
    edge_case = y % terrain::Chunk::SIZE;
    if (edge_case == 0) {
        if (terrain_main.in_range(x, y - 1, z)) {
            chunk_pos = terrain_main.get_chunk_from_tile(x, y - 1, z);
            update_single_mesh(chunk_pos);
        }
    } else if (edge_case == terrain::Chunk::SIZE - 1) {
        if (terrain_main.in_range(x, y + 1, z)) {
            chunk_pos = terrain_main.get_chunk_from_tile(x, y + 1, z);
            update_single_mesh(chunk_pos);
        }
    }
    edge_case = z % terrain::Chunk::SIZE;
    if (edge_case == 0) {
        if (terrain_main.in_range(x, y, z - 1)) {
            chunk_pos = terrain_main.get_chunk_from_tile(x, y, z - 1);
            update_single_mesh(chunk_pos);
        }
    } else if (edge_case == terrain::Chunk::SIZE - 1) {
        if (terrain_main.in_range(x, y, z + 1)) {
            chunk_pos = terrain_main.get_chunk_from_tile(x, y, z + 1);
            update_single_mesh(chunk_pos);
        }
    }
}
