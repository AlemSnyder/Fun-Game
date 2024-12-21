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

void
ObjectHandler::read_object(const manifest::descriptor_t& descriptor) {
    // read contents from path
    auto object_data = files::read_json_from_file<object_t>(descriptor.path);

    if (!object_data) {
        LOG_ERROR(
            logging::file_io_logger, "Failed to load {} from {}.",
            descriptor.identification, descriptor.path
        );
        return;
    }

    std::lock_guard<std::mutex> lock(this->map_mutex_);

    // check identification
    std::string identification = descriptor.identification;
    if (ided_objects.find(identification) != ided_objects.end()) {
        LOG_WARNING(
            logging::file_io_logger, "Duplicate Identification \"{}\" found.",
            identification
        );
        return;
    }

    switch (object_data->type) {
        case OBJECT_TYPE::TILE_OBJECT:
            {
                std::shared_ptr<TileObject> new_object =
                    std::make_shared<TileObject>(*object_data, descriptor);

                // when objects are initalized data is sent to the gpu.
                // we want to run the mesher async, but need to send the data to the gpu
                // on the main thread
                ided_objects[identification] = static_pointer_cast<Object>(new_object);
            }
            break;
        case OBJECT_TYPE::ENTITY:
            {
                std::shared_ptr<Entity> new_object =
                    std::make_shared<Entity>(*object_data, descriptor);
                ided_objects[identification] = static_pointer_cast<Object>(new_object);
            }
            break;

        default:
            break;
    }
}

void
ObjectHandler::update() {
    for (auto& [key, object] : ided_objects) {
        object->update();
    }
}

} // namespace entity

} // namespace world