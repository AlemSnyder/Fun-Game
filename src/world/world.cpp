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
#include "entity/object_handler.hpp"
#include "entity/tile_object.hpp"
#include "glm/gtx/transform.hpp"
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

namespace world {

const terrain::material_t*
World::get_material(MaterialId material_id) const {
    return &biome_.get_materials().at(material_id);
}

World::World(const std::string& biome_name, const std::string& path, size_t seed) :
    biome_(biome_name, seed), terrain_main_(path, biome_) {}

World::World(
    const std::string& biome_name, MacroDim x_tiles, MacroDim y_tiles, size_t seed
) :
    biome_(biome_name, seed),
    terrain_main_(
        x_tiles, y_tiles, macro_tile_size, height, biome_,
        std::move(biome_.get_map(x_tiles))
    ) {}

World::World(const std::string& biome_name, MapTile_t tile_type, size_t seed) :
    biome_(biome_name, seed),
    terrain_main_(
        3, 3, macro_tile_size, height, biome_, biome_.single_tile_type_map(tile_type)
    ) {}

void
World::generate_plants() {
    auto plant_maps = biome_.get_plant_map(terrain_main_.X_MAX);

    std::unordered_map<int, glm::ivec2> ordered_tiles;
    for (Dim x = 0; x < terrain_main_.X_MAX; x++) {
        for (Dim y = 0; y < terrain_main_.Y_MAX; y++) {
            size_t tile_hash = biome_.seed;
            utils::hash_combine(tile_hash, x);
            utils::hash_combine(tile_hash, y);
            ordered_tiles[tile_hash] = glm::vec2(x, y);
        }
    }

    // This next part can be done in parallel.
    // maybe generating each plant position, orientation, and model
    entity::ObjectHandler& object_handler = entity::ObjectHandler::instance();

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

    for (const auto& tile_position_pair : ordered_tiles) {
        glm::vec2 tile_position = tile_position_pair.second;
        for (const terrain::generation::plant_t& plant : biome_.get_generate_plants()) {
            auto map = plant_maps[plant.map_name];

            float chance = map.get_tile(tile_position.x, tile_position.y);

            if (uniform_distribution(rand_engine) < chance) {
                uint rotation = rotation_distribution(rand_engine) % 4;

                uint z_position =
                    terrain_main_.get_Z_solid(tile_position.x, tile_position.y) + 1;

                // zero is for one of the models should be random number between 0, and
                // num meshes
                //                entity::ModelController& model =
                //                object_type->get_model(0);

                // position, then rotation, and texture
                gui::Placement placement(
                    tile_position.x, tile_position.y, z_position, rotation, 0
                );

                auto tile_object_type = std::dynamic_pointer_cast<entity::TileObject>(
                    object_handler.get_object(plant.identification)
                );

                if (!tile_object_type) {
                    continue;
                }

                auto new_object = std::make_shared<entity::TileObjectInstance>(
                    tile_object_type, uint8_t(0), placement
                );

                tile_entities_.insert(new_object);
            }
        }
    }
}

// Should not be called except by lambda function
void
World::update_single_mesh(ChunkPos chunk_pos) {
    const auto chunk = terrain_main_.get_chunk(chunk_pos);
    if (!chunk) {
        return;
    }
    entity::Mesh chunk_mesh =
        entity::ambient_occlusion_mesher(terrain::ChunkData(*chunk));

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

void
World::spawn_entity(std::string identification, glm::vec3 position) {
    auto& object_handler = entity::ObjectHandler::instance();
    auto object_type = object_handler.get_object(identification);

    if (!object_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} does not exists", identification
        );
        return;
    }

    auto entity_type = std::dynamic_pointer_cast<entity::Entity>(object_type);

    if (!entity_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} is not an entity type",
            identification
        );
        return;
    }

    auto entity = std::make_shared<entity::EntityInstance>(entity_type);

    glm::mat4 transformation(1.0);

    entity->update(glm::translate(transformation, position));

    entities_.insert(entity);
}

} // namespace world
