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
World::get_grass_grad_data(const Json::Value& materials_json) {
    std::vector<int> grass_grad_data;
    for (const Json::Value& grass_level :
         materials_json["Dirt"]["Gradient"]["levels"]) {
        grass_grad_data.push_back(grass_level.asInt());
    }

    return grass_grad_data;
}

std::map<MaterialId, const terrain::Material>
World::init_materials(const Json::Value& material_data) {
    std::map<uint8_t, const terrain::Material> out;
    for (auto element_it = material_data.begin(); element_it != material_data.end();
         element_it++) {
        std::vector<std::pair<const std::string, ColorInt>> color_vector;

        const Json::Value material = *element_it;
        std::string name = element_it.key().asString();
        for (const Json::Value& json_color : material["colors"]) {
            const std::string color_name = json_color["name"].asString();
            ColorInt color_value = std::stoll(json_color["hex"].asString(), 0, 16);
            color_vector.push_back(std::make_pair(std::move(color_name), color_value));
        }

        terrain::Material mat{
            color_vector,                                    // color
            static_cast<uint8_t>(material["speed"].asInt()), // speed_multiplier
            material["solid"].asBool(),                      // solid
            static_cast<MaterialId>(material["id"].asInt()), // element_id
            name};                                           // name
        out.insert(std::make_pair(mat.element_id, mat));
    }

    terrain::TerrainColorMapping::assign_color_mapping(out);
    return out;
}

World::World(const Json::Value& materials_json, const std::string path) :
    materials(init_materials(materials_json)),
    terrain_main(
        path, materials, get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {
    initialize_chunks_mesh_();
}

World::World(
    const Json::Value& materials_json, const Json::Value& biome_data, uint32_t x_tiles,
    uint32_t y_tiles
) :
    materials(init_materials(materials_json)),
    terrain_main(
        x_tiles, y_tiles, macro_tile_size, height, 5, materials, biome_data["Biome_1"],
        get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {
    initialize_chunks_mesh_();
}

World::World(
    const Json::Value& materials_json, const Json::Value& biome_data, int tile_type
) :
    materials(init_materials(materials_json)),
    terrain_main(
        macro_tile_size, height, 5, tile_type, materials, biome_data["Biome_1"],
        get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {
    // on initialization world reserves the space it would need for shared pointers
    initialize_chunks_mesh_();
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
World::update_all_chunks_mesh() {
    for (size_t i = 0; i < chunks_mesh.size(); i++)
        update_single_mesh(i);
}

void
World::update_single_mesh(uint16_t chunk_pos) {
    const auto& chunks = terrain_main.get_chunks();
    entity::Mesh chunk_mesh = entity::generate_mesh(chunks[chunk_pos]);

    chunk_mesh.change_color_indexing(
        materials, terrain::TerrainColorMapping::get_colors_inverse_map()
    );

    chunks_mesh[chunk_pos]->update(chunk_mesh);
}

void
World::update_single_mesh(TerrainDim3 tile_sop) {
    if (terrain_main.in_range(tile_sop.x + 1, tile_sop.y, tile_sop.z)) {
        uint16_t chunk_pos =
            terrain_main.get_chunk_from_tile(tile_sop.x + 1, tile_sop.y, tile_sop.z);
        update_single_mesh(chunk_pos);
    }
}

void
World::set_tile(uint16_t pos, const terrain::Material* mat, uint8_t color_id) {
    terrain_main.get_tile(pos)->set_material(mat, color_id);

    TerrainDim3 tile_sop = terrain_main.sop(pos);
    update_single_mesh(tile_sop);

    // do some math:
    // if the tile is on the edge of a chunk then both chunks must be updated.
    uint8_t edge_case = tile_sop.x % terrain::Chunk::SIZE;
    if (edge_case == 0) {
        update_single_mesh({tile_sop.x - 1, tile_sop.y, tile_sop.z});
    } else if (edge_case == terrain::Chunk::SIZE - 1) {
        update_single_mesh({tile_sop.x + 1, tile_sop.y, tile_sop.z});
    }
    edge_case = tile_sop.y % terrain::Chunk::SIZE;
    if (edge_case == 0) {
        update_single_mesh({tile_sop.x, tile_sop.y - 1, tile_sop.z});
    } else if (edge_case == terrain::Chunk::SIZE - 1) {
        update_single_mesh({tile_sop.x, tile_sop.y + 1, tile_sop.z});
    }
    edge_case = tile_sop.z % terrain::Chunk::SIZE;
    if (edge_case == 0) {
        update_single_mesh({tile_sop.x, tile_sop.y, tile_sop.z - 1});
    } else if (edge_case == terrain::Chunk::SIZE - 1) {
        update_single_mesh({tile_sop.x, tile_sop.y, tile_sop.z + 1});
    }
}
