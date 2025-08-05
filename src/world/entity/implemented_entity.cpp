#include "implemented_entity.hpp"

namespace world {
namespace entity {
glm::vec3
ImplementedEntity::decision(EntityInstance* entity_instance) {
    glm::vec3 position = entity_instance->get_position();
    if (position.z > 60) {
        return {position.x, position.y, 30};
    }
    return position + glm::vec3(0, 0, 0.01);
}
} // namespace entity

} // namespace world
