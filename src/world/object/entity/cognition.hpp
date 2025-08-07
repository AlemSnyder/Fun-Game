#include <glm/glm.hpp>

namespace world {

namespace object {

namespace entity {

class EntityInstance;

class Cognition {
 public:
    virtual ~Cognition() = default;
    virtual glm::vec3 decision(EntityInstance* entity_instance) = 0;

    virtual void make_plan(EntityInstance* entity_instance) = 0;
    virtual void execute_plan(EntityInstance* entity_instance) = 0;
};

} // namespace entity

} // namespace object

} // namespace world
