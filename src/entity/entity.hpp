#include <json/json.h>

#include "bones.hpp"

namespace entity {

class Entity {
 private:
    Bones bones_;
    //  species
 public:
    // a bunch of getters
    // some setters
    Entity(const Json::Value& entity_data);
    ~Entity();
};

} // namespace entity
