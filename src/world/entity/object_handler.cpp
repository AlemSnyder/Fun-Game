#include "object_handler.hpp"

namespace world {

namespace entity {

ObjectData&
ObjectHandler::get_object(const std::string& id) {
    return ided_objects.at(id);
}

} // namespace entity
} // namespace world