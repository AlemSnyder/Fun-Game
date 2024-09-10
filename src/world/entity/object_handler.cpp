#include "object_handler.hpp"

#include "util/files.hpp"
#include "util/voxel.hpp"

#include <utility>

namespace world {

namespace entity {

// TODO change to return a pointer (could be null)
ObjectData&
ObjectHandler::get_object(const std::string& id) {
    return ided_objects.at(id);
}

void
ObjectHandler::read_object(const manifest::descriptor_t& descriptor) {
    // read contents from path
    auto object_data = files::read_json_from_file<object_t>(descriptor.path);

    if (!object_data.has_value()) {
        LOG_ERROR(
            logging::file_io_logger, "Attempting to load {} from {} failed.",
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

    // when objects are initalized data is sent to the gpu.
    // we want to run the mesher async, but need to send the data to the gpu
    // on the main thread
    ided_objects.emplace(
        std::piecewise_construct, std::forward_as_tuple(std::move(identification)),
        std::forward_as_tuple(object_data.value(), descriptor)
    );
}

void
ObjectHandler::update() {
    for (auto& [key, object] : ided_objects) {
        object.update();
    }
}

} // namespace entity

} // namespace world