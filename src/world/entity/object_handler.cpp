#include "object_handler.hpp"

#include "entity.hpp"
#include "tile_object.hpp"
#include "util/files.hpp"
#include "util/voxel.hpp"

#include <utility>

namespace world {

namespace entity {

// TODO change to return a pointer (could be null)
std::shared_ptr<Object>
ObjectHandler::get_object(const std::string& id) {
    return ided_objects.at(id);
}

/*void
ObjectHandler::update() {
    for (auto& [key, object] : ided_objects) {
        object->update();
    }
}*/

void
ObjectHandler::start_update() {
    position_synchronizer_.start(ided_objects);
}

void
ObjectHandler::stop_update() {
    position_synchronizer_.stop();
}

} // namespace entity

} // namespace world