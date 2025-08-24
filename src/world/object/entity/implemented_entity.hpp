#pragma once

#include "entity.hpp"

namespace world {

namespace object {

namespace entity {

class ImplementedEntity : public virtual Entity {
 public:
    ImplementedEntity(
        const object_t& object_data, const manifest::descriptor_t& model_path
    ) :
        Entity(object_data, model_path) {}

    [[nodiscard]] virtual glm::vec3 decision(EntityInstance* entity_instance) override;
};

} // namespace entity

} // namespace object

} // namespace world
