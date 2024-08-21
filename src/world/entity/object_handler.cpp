#include "object_handler.hpp"

#include "util/voxel.hpp"

#include <utility>

namespace world {

namespace entity {

ObjectData&
ObjectHandler::get_object(const std::string& id) {
    return ided_objects.at(id);
}

void
ObjectHandler::read_object(std::filesystem::path object_path) {
    // json to read data into
    Json::Value object_json;

    // read contents from path
    auto contents = files::open_data_file(object_path);
    if (contents.has_value()) {
        contents.value() >> object_json;
    } else {
        LOG_WARNING(
            logging::file_io_logger, "Cannot open file {}.", object_path.string()
        );
        return;
    }

    std::lock_guard<std::mutex> lock(this->map_mutex_);

    // check identification
    std::string identification = object_json["identification"].asString();
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
        std::forward_as_tuple(object_json, object_path)
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