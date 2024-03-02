#include "object_handler.hpp"

#include "json/json.h"
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
    Json::Value object_json;

    auto contents = files::open_data_file(object_path);
    if (contents.has_value()) {
        contents.value() >> object_json;
    } else {
        LOG_WARNING(
            logging::file_io_logger, "Cannot open file {}.", object_path.string()
        );
        return;
    }

    //ObjectData object_data(object_json, object_path);

    // voxel_utility::VoxelObject voxel_object(path);
    // entity::Mesh mesh = world::entity::ambient_occlusion_mesher(voxel_object);

    std::lock_guard<std::mutex> lock(this->map_mutex_);

    // TODO check identification

    ided_objects.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(object_json["identification"].asString()),
        std::forward_as_tuple(object_json, object_path)
    );
}

} // namespace entity

} // namespace world