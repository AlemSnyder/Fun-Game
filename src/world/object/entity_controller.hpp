#pragma once
#include "entity/entity.hpp"
#include "manifest/object_handler.hpp"
#include "types.hpp"

#include <glm/glm.hpp>

#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace world {

namespace object {

class EntityController {
    manifest::ObjectHandler* object_handler_;

    std::unordered_map<
        glm::ivec3, std::unordered_set<std::shared_ptr<entity::EntityInstance>>>
        entity_instances_;
    std::unordered_map<
        glm::ivec3, std::unordered_set<std::shared_ptr<entity::TileObjectInstance>>>
        object_instances_;

 public:
    EntityController(manifest::ObjectHandler* object_handler) :
        object_handler_(object_handler){};

    void update_entities(glm::mat4 transforms_matrix);

    std::shared_ptr<entity::EntityInstance>
    spawn_entity(std::string identification, glm::vec3 position);
    // TODO should also be texture, and placement but not that important rn

    void remove_entity(std::shared_ptr<entity::EntityInstance> entity_instance);

    std::shared_ptr<entity::TileObjectInstance> spawn_tile_object(
        std::string identification, uint8_t texture, gui::Placement placement
    );

    void remove_tile_object(std::shared_ptr<entity::TileObjectInstance> entity_instance
    );

    void load_to_gup();

    /**
     * @brief Get object handler
     */
    const auto
    get_object_handler() const {
        return object_handler_;
    }

    /**
     * @brief Get object handler
     */
    auto
    get_object_handler() {
        return object_handler_;
    }

 private:
    void region_update_visible(glm::ivec3 region);
    void region_update_invisible(glm::ivec3 region);
};

} // namespace object

} // namespace world

/*

    auto object_type = object_handler_->get_object(identification);

    if (!object_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} does not exists", identification
        );
        return {};
    }

    auto entity_type = std::dynamic_pointer_cast<object::entity::Entity>(object_type);

    if (!entity_type) {
        LOG_ERROR(
            logging::main_logger, "Identification {} is not an entity type",
            identification
        );
        return {};
    }

    auto entity = std::make_shared<object::entity::EntityInstance>(entity_type,
   position);

    glm::mat4 transformation(1.0);

    entity->update(glm::translate(transformation, position));






    if (!entity) {
        LOG_WARNING(logging::main_logger, "Entity is null.");
        return;
    }
    // entity->destroy();
    // size_t erased = entities_.erase(entity);
    // if (!erased) {
    //     LOG_WARNING(logging::main_logger, "Entity not found in World.");
    // }




                    auto tile_object_type =
                        std::dynamic_pointer_cast<object::entity::TileObject>(object);

                    if (!tile_object_type) {
                        continue;
                    }

                    auto new_object =
   std::make_shared<object::entity::TileObjectInstance>( tile_object_type, uint8_t(0),
   placement
                    );

                    // tile_entities_.insert(new_object);
 */