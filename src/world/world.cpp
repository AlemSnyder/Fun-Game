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

#include "glm/gtx/transform.hpp"
#include "global_context.hpp"
#include "logging.hpp"
#include "manifest/object_handler.hpp"
#include "object/entity/tile_object.hpp"
#include "terrain/generation/map_tile.hpp"
#include "terrain/material.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"
#include "util/mesh.hpp"

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>

namespace world {

const terrain::material_t*
World::get_material(MaterialId material_id) const {
    return &biome_.get_materials().at(material_id);
}

World::World(
    manifest::ObjectHandler* object_handler, const std::string& biome_name,
    const std::string& path, size_t seed
) :
    biome_(biome_name, seed),
    terrain_main_(path, biome_), controller_(object_handler) {}

World::World(
    manifest::ObjectHandler* object_handler, const std::string& biome_name,
    MacroDim x_tiles, MacroDim y_tiles, size_t seed
) :
    biome_(biome_name, seed),
    terrain_main_(
        x_tiles, y_tiles, macro_tile_size, height, biome_,
        std::move(biome_.get_map(x_tiles))
    ),
    controller_(object_handler) {}

World::World(
    manifest::ObjectHandler* object_handler, const std::string& biome_name,
    MapTile_t tile_type, size_t seed
) :
    biome_(biome_name, seed),
    terrain_main_(
        3, 3, macro_tile_size, height, biome_, biome_.single_tile_type_map(tile_type)
    ),
    controller_(object_handler) {}

void
World::generate_plants() {
    auto plant_maps = biome_.get_plant_map(terrain_main_.X_MAX);

    // This next part can be done in parallel.
    // maybe generating each plant position, orientation, and model

    std::default_random_engine rand_engine(biome_.seed + 1);
    std::uniform_real_distribution uniform_distribution(0.0, 1.0);
    std::uniform_int_distribution rotation_distribution(0, 3);

    // just debug
    for (const terrain::generation::plant_t& plant : biome_.get_generate_plants()) {
        auto map = plant_maps[plant.map_name];
        LOG_DEBUG(logging::terrain_logger, "Plant map name: {}.", plant.map_name);

        LOG_DEBUG(
            logging::terrain_logger, "Map width: {}, height: {}", map.get_width(),
            map.get_height()
        );
    }

    for (Dim tile_position_x = 0; tile_position_x < terrain_main_.X_MAX;
         tile_position_x++) {
        for (Dim tile_position_y = 0; tile_position_y < terrain_main_.Y_MAX;
             tile_position_y++) {
            for (const terrain::generation::plant_t& plant :
                 biome_.get_generate_plants()) {
                auto map = plant_maps[plant.map_name];

                float chance = map.get_tile(tile_position_x, tile_position_y);

                if (uniform_distribution(rand_engine) < chance) {
                    uint rotation = rotation_distribution(rand_engine) % 4;

                    uint z_position =
                        terrain_main_.get_Z_solid(tile_position_x, tile_position_y) + 1;

                    // position, then rotation, and texture
                    gui::Placement placement(
                        tile_position_x, tile_position_y, z_position, rotation, 0
                    );
                    // id, model, then placement with texture id.
                    controller_.spawn_tile_object(
                        plant.identification, uint8_t(0), placement
                    );
                }
            }
        }
    }
}

// Should not be called om main thread
void
World::update_single_mesh(ChunkPos chunk_pos) {
    const auto chunk = terrain_main_.get_chunk(chunk_pos);
    if (!chunk) {
        return;
    }
    util::Mesh chunk_mesh = util::ambient_occlusion_mesher(terrain::ChunkData(*chunk));

    chunk_mesh.change_color_indexing(
        biome_.get_materials(), terrain::TerrainColorMapping::get_colors_inverse_map()
    );

    if (chunk_mesh.get_indices().size() > 0) {
        std::scoped_lock lock(meshes_to_update_mutex_);
        meshes_to_update_.insert({chunk_pos, chunk_mesh});
    }
}

// TODO should set a limit the the number
// update_marked_chunks_mesh runs every frame, so the number of chunks updated
// each frame should be capped.
void
World::update_marked_chunks_mesh() {
    for (auto chunk_pos : chunks_to_update_) {
        GlobalContext& context = GlobalContext::instance();
        context.submit_task([this, chunk_pos]() { this->update_single_mesh(chunk_pos); }
        );
    }
    chunks_to_update_.clear();
}

void
World::update_all_chunks_mesh() {
    LOG_DEBUG(logging::terrain_logger, "Begin load chunks mesh");
    size_t num_chunks = terrain_main_.num_chunks();

    std::vector<std::future<void>> wait_for;
    wait_for.reserve(num_chunks);
    GlobalContext& context = GlobalContext::instance();
    for (const auto& [chunk_pos, chunk] : terrain_main_.get_chunks()) {
        auto future = context.submit_task([this, chunk_pos]() {
            this->update_single_mesh(chunk_pos);
        });
        wait_for.push_back(std::move(future));
    }
    // Should only wait for the previously queued tasks.
    for (const auto& task : wait_for) {
        task.wait();
    }

    std::scoped_lock lock(meshes_to_update_mutex_);
    terrain_mesh_ = std::make_shared<gui::gpu_data::TerrainMesh>(
        meshes_to_update_, terrain::TerrainColorMapping::get_color_texture()
    );
}

// will be called once per frame
void
World::send_updated_chunks_mesh() {
    std::scoped_lock lock(meshes_to_update_mutex_);
    for (const auto& [chunk_pos, mesh_data] : meshes_to_update_) {
        terrain_mesh_->replace(chunk_pos, mesh_data);
    }
}

void
World::set_tile(
    TerrainOffset3 tile_sop, const terrain::material_t* mat, ColorId color_id
) {
    terrain_main_.get_tile(tile_sop)->set_material(mat, color_id);

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

std::shared_ptr<object::entity::EntityInstance>
World::spawn_entity(std::string identification, glm::vec3 position) {
    return controller_.spawn_entity(identification, position);
}

void
World::remove_entity(std::shared_ptr<object::entity::EntityInstance> entity) {
    controller_.remove_entity(entity);
}

std::optional<std::vector<TerrainOffset3>>
World::pathfind_to_object(TerrainOffset3 start_position, const std::string& object_id)
    const {
    auto object = get_object_handler()->get_object(object_id);
    if (!object) {
        LOG_WARNING(logging::terrain_logger, "Object {} not found.", object_id);
        return {};
    }
    // object->

    std::unordered_set<TerrainOffset3> object_positions;

    // This makes me sad
    // TODO fix the storage mechanism
    // for (const auto& tile_entity : controller_) {
    //     if (tile_entity->get_object() == object) {
    //         object_positions.insert(tile_entity->get_terrain_position());
    //     }
    // }

    auto path = terrain_main_.get_path_breadth_first(start_position, object_positions);

    return path;
}

} // namespace world
