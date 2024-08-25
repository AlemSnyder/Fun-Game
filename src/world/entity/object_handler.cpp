#include "object_handler.hpp"

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
ObjectHandler::read_object(const Manifest::descriptor_t& descriptor) {
    // json to read data into

    object_t object_data;

    // read contents from path
    auto contents = files::open_data_file(descriptor.path);
    if (contents.has_value()) {
        std::string content;
        *contents.value() >> content;

        auto ec = glz::read_json(object_data, content);
        if (ec) {
            LOG_ERROR(logging::file_io_logger, "{}", glz::format_error(ec, content));
            return;
        }
    } else {
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
        std::forward_as_tuple(object_data, descriptor)
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