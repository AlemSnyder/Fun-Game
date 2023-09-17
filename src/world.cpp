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

#include "world.hpp"

#include "entity/mesh.hpp"
#include "terrain/material.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"

#include <cstdint>
#include <fstream>
#include <string>

const terrain::Material*
World::get_material(MaterialId material_id) const {
    return &biome_.get_materials().at(material_id);
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

World::World(const std::string& biome_name, const std::string path) :
    biome_(biome_name), terrain_main_(path, biome_) {
    // initialize_chunks_mesh_();
}

World::World(const std::string& biome_name, MacroDim x_tiles, MacroDim y_tiles) :
    biome_(biome_name),
    terrain_main_(
        x_tiles, y_tiles, macro_tile_size, height, 5, biome_, biome_.get_map()
    ) {
    // initialize_chunks_mesh_();
}

World::World(const std::string& biome_name, MapTile_t tile_type) :
    biome_(biome_name),
    terrain_main_(
        3, 3, macro_tile_size, height, 5, biome_, {0, 0, 0, 0, tile_type, 0, 0, 0, 0}
    ) {
    // on initialization world reserves the space it would need for shared pointers
    // initialize_chunks_mesh_();
}

void
World::update_single_mesh(ChunkIndex chunk_pos) {
    const auto& chunks = terrain_main_.get_chunks();
    entity::Mesh chunk_mesh = entity::generate_mesh(chunks[chunk_pos]);

    chunk_mesh.change_color_indexing(
        biome_.get_materials(), terrain::TerrainColorMapping::get_colors_inverse_map()
    );

    chunks_mesh_[chunk_pos]->update(chunk_mesh);
    chunks_mesh_[chunk_pos]->set_color_texture(terrain::TerrainColorMapping::get_color_texture());
}

void
World::update_single_mesh(TerrainDim3 tile_sop) {
    if (!terrain_main_.in_range(tile_sop))
        return;
    Dim chunk_pos = terrain_main_.get_chunk_from_tile(tile_sop);
    update_single_mesh(chunk_pos);
}

void
World::update_all_chunks_mesh() {
    size_t num_chunks = terrain_main_.get_chunks().size();
    if (chunks_mesh_.size() != num_chunks) {
        chunks_mesh_.reserve(num_chunks);
        for (size_t i = 0; i < num_chunks; i++) {
            chunks_mesh_.push_back(std::make_shared<gui::data_structures::TerrainMesh>(
                                       gui::data_structures::TerrainMesh()
            ));
        }
    }

    for (size_t i = 0; i < num_chunks; i++)
        update_single_mesh(i);
}

void
World::set_tile(Dim pos, const terrain::Material* mat, ColorId color_id) {
    terrain_main_.get_tile(pos)->set_material(mat, color_id);

    TerrainDim3 tile_sop = terrain_main_.sop(pos);
    update_single_mesh(tile_sop);

    // do some math:
    // if the tile is on the edge of a chunk then both chunks must be updated.
    Dim edge_case = tile_sop.x % terrain::Chunk::SIZE;
    if (edge_case == 0)
        update_single_mesh({tile_sop.x - 1, tile_sop.y, tile_sop.z});
    else if (edge_case == terrain::Chunk::SIZE - 1)
        update_single_mesh({tile_sop.x + 1, tile_sop.y, tile_sop.z});

    edge_case = tile_sop.y % terrain::Chunk::SIZE;
    if (edge_case == 0)
        update_single_mesh({tile_sop.x, tile_sop.y - 1, tile_sop.z});
    else if (edge_case == terrain::Chunk::SIZE - 1)
        update_single_mesh({tile_sop.x, tile_sop.y + 1, tile_sop.z});

    edge_case = tile_sop.z % terrain::Chunk::SIZE;
    if (edge_case == 0)
        update_single_mesh({tile_sop.x, tile_sop.y, tile_sop.z - 1});
    else if (edge_case == terrain::Chunk::SIZE - 1)
        update_single_mesh({tile_sop.x, tile_sop.y, tile_sop.z + 1});
}
