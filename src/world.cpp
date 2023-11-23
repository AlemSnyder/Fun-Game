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
#include "global_context.hpp"
#include "logging.hpp"
#include "terrain/generation/map_tile.hpp"
#include "terrain/material.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"

#include <cstdint>
#include <fstream>
#include <mutex>
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

World::World(const std::string& biome_name, const std::string& path, size_t seed) :
    biome_(biome_name, seed), terrain_main_(path, biome_) {}

World::World(
    const std::string& biome_name, MacroDim x_tiles, MacroDim y_tiles, size_t seed
) :
    biome_(biome_name, seed),
    terrain_main_(
        x_tiles, y_tiles, macro_tile_size, height, seed, biome_, biome_.get_map(x_tiles)
    ) {}

World::World(const std::string& biome_name, MapTile_t tile_type, size_t seed) :
    biome_(biome_name, seed),
    terrain_main_(
        3, 3, macro_tile_size, height, seed, biome_, terrain::generation::Biome::get_test_map(tile_type)
    ) {}

// Should not be called except by lambda function
void
World::update_single_mesh(ChunkIndex chunk_pos) {
    const auto& chunk = terrain_main_.get_chunk(chunk_pos);
    entity::Mesh chunk_mesh =
        entity::ambient_occlusion_mesher(terrain::ChunkData(chunk));

    chunk_mesh.change_color_indexing(
        biome_.get_materials(), terrain::TerrainColorMapping::get_colors_inverse_map()
    );

    std::scoped_lock lock(meshes_to_update_mutex_);
    meshes_to_update_.insert({chunk_pos, chunk_mesh});
}

// TODO should set a limit the the number
// update_marked_chunks_mesh runs every frame, so the number of chunks updated
// each frame should be capped.
void
World::update_marked_chunks_mesh() {
    for (auto chunk_pos : chunks_to_update_) {
        GlobalContext& context = GlobalContext::instance();
        context.submit(
            [this](ChunkIndex p) { this->update_single_mesh(p); }, chunk_pos
        );
    }
    chunks_to_update_.clear();
}

void
World::update_all_chunks_mesh() {
    LOG_DEBUG(logging::terrain_logger, "Begin load chunks mesh");
    size_t num_chunks = terrain_main_.get_chunks().size();
    if (chunks_mesh_.size() != num_chunks) {
        chunks_mesh_.clear();
        chunks_mesh_.reserve(num_chunks);
        for (size_t i = 0; i < num_chunks; i++) {
            chunks_mesh_.push_back(std::make_shared<gui::data_structures::TerrainMesh>(
                gui::data_structures::TerrainMesh()
            ));
        }
    }

    std::vector<std::future<void>> wait_for;
    wait_for.reserve(num_chunks);
    GlobalContext& context = GlobalContext::instance();
    for (size_t chunk_pos = 0; chunk_pos < num_chunks; chunk_pos++) {
        auto future = context.submit(
            [this](ChunkIndex p) { this->update_single_mesh(p); }, chunk_pos
        );
        wait_for.push_back(std::move(future));
    }
    // Should only wait for the previously queued tasks.
    for (const auto& task : wait_for) {
        task.wait();
    }
    send_updated_chunks_mesh();
}

// will be called once per frame
void
World::send_updated_chunks_mesh() {
    std::scoped_lock lock(meshes_to_update_mutex_);
    for (const auto& mesh_data : meshes_to_update_) {
        chunks_mesh_[mesh_data.first]->update(mesh_data.second);
        chunks_mesh_[mesh_data.first]->set_color_texture(
            terrain::TerrainColorMapping::get_color_texture()
        );
    }
}

void
World::set_tile(Dim pos, const terrain::Material* mat, ColorId color_id) {
    terrain_main_.get_tile(pos)->set_material(mat, color_id);

    TerrainDim3 tile_sop = terrain_main_.sop(pos);
    mark_for_update(tile_sop);

    // do some math:
    // if the tile is on the edge of a chunk then both chunks must be updated.
    Dim edge_case = tile_sop.x % terrain::Chunk::SIZE;
    if (edge_case == 0)
        mark_for_update({tile_sop.x - 1, tile_sop.y, tile_sop.z});
    else if (edge_case == terrain::Chunk::SIZE - 1)
        mark_for_update({tile_sop.x + 1, tile_sop.y, tile_sop.z});

    edge_case = tile_sop.y % terrain::Chunk::SIZE;
    if (edge_case == 0)
        mark_for_update({tile_sop.x, tile_sop.y - 1, tile_sop.z});
    else if (edge_case == terrain::Chunk::SIZE - 1)
        mark_for_update({tile_sop.x, tile_sop.y + 1, tile_sop.z});

    edge_case = tile_sop.z % terrain::Chunk::SIZE;
    if (edge_case == 0)
        mark_for_update({tile_sop.x, tile_sop.y, tile_sop.z - 1});
    else if (edge_case == terrain::Chunk::SIZE - 1)
        mark_for_update({tile_sop.x, tile_sop.y, tile_sop.z + 1});
}
