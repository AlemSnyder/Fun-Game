#pragma once
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include "types.hpp"
#include "entity/entity.hpp"

#include <memory>

namespace world {

namespace object {

class EntityController {
    std::unordered_map<glm::ivec3, std::unordered_set<std::shared_ptr<entity::EntityInstance>>> entity_instancess_;
 public:
    EntityController() {};

    inline void update_entities() {}
};

}

}
