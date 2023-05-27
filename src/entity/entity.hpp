#include "bones.hpp"

#include <json/json.h>

#include <glm/glm.hpp>

namespace entity {

class Entity {
 private:
    Bones bones_;
    //  species
 public:
    // a bunch of getters
    // some setters
    Entity(const Json::Value& entity_data);
    void update_bones();

    inline size_t
    num_bones() {
        return bones_.size();
    }

    glm::mat4 bone_transform(size_t bone_index);
    ~Entity();
};

} // namespace entity
