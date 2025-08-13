#include "entity_controller.hpp"

#include "util/position.hpp"

namespace world {

namespace object {

// TODO
// then check that update entities is correctly called.

void
EntityController::update_entities(glm::mat4 transforms_matrix) {
    // maybe multi-processes
    for (auto& region : entity_instances_) {
        if (true) {
            region_update_visible(region.first);
        } else {
            region_update_invisible(region.first);
        }
    }
}

std::shared_ptr<entity::EntityInstance>
EntityController::spawn_entity(std::string identification, glm::vec3 position) {
    // get object
    auto object_type = object_handler_->get_object(identification);

    if (!object_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} does not exists", identification
        );
        return nullptr;
    }

    auto entity_type = std::dynamic_pointer_cast<entity::Entity>(object_type);

    if (!entity_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} is not an entity type",
            identification
        );
        return nullptr;
    }

    // get position
    ChunkPos chunk_position = util::position::chunk_pos_from_vec(position);

    if (!entity_instances_.contains(chunk_position)) {
        entity_instances_.emplace(
            std::piecewise_construct, std::make_tuple(chunk_position), std::make_tuple()
        );
    }

    // entity_instances_.at(position).insert(object);
    auto inserted =
        entity_instances_.at(chunk_position)
            .insert(std::make_shared<entity::EntityInstance>(entity_type, position));

    return *inserted.first;
}

void
EntityController::remove_entity(std::shared_ptr<entity::EntityInstance> entity_instance
) {
    // get position
    ChunkPos chunk_position =
        util::position::chunk_pos_from_vec(entity_instance->get_position());

    entity_instances_.at(chunk_position).erase(entity_instance);
}

std::shared_ptr<entity::TileObjectInstance>
EntityController::spawn_tile_object(
    std::string identification, uint8_t model_id, gui::Placement placement
) {
    // get object
    auto object_type = object_handler_->get_object(identification);

    if (!object_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} does not exists", identification
        );
        return nullptr;
    }

    auto tile_object_type = std::dynamic_pointer_cast<entity::TileObject>(object_type);

    if (!tile_object_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} is not an entity type",
            identification
        );
        return nullptr;
    }

    // get position
    ChunkPos chunk_position = util::position::chunk_pos_from_vec(
        glm::vec3(placement.x, placement.y, placement.z)
    );

    if (!object_instances_.contains(chunk_position)) {
        object_instances_.emplace(
            std::piecewise_construct, std::make_tuple(chunk_position), std::make_tuple()
        );
    }

    // tile_object_instances_.at(position).insert(object);
    auto inserted = object_instances_.at(chunk_position)
                        .insert(std::make_shared<entity::TileObjectInstance>(
                            tile_object_type, model_id, placement
                        ));

    return *inserted.first;
}

void
EntityController::remove_tile_object(
    std::shared_ptr<entity::TileObjectInstance> entity_instance
) {
    ChunkPos chunk_position =
        util::position::chunk_pos_from_vec(entity_instance->get_position());
    object_instances_.at(chunk_position).erase(entity_instance);
}

void
EntityController::load_to_gup() {
    // needs accesses to object controller I think
    for (auto& object : *object_handler_) {
        object.second->sync_data_to_gpu();
    }
}

void
EntityController::region_update_visible(/*time*/ glm::ivec3 region) {
    auto entities = entity_instances_.at(region);
    for (auto& entity_instance : entities) {
        entity_instance->operate(std::chrono::milliseconds(1), true);
    }
}

void
EntityController::region_update_invisible(/*time*/ glm::ivec3 region) {
    auto entities = entity_instances_.at(region);
    for (auto& entity_instance : entities) {
        entity_instance->operate(std::chrono::milliseconds(1), false);
    }
}

} // namespace object
} // namespace world