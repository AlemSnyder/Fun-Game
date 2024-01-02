#include "tile_object.hpp"

namespace entity {

ObjectData::ObjectData(Json::Value object_json){
    for (Json::Value mesh_data : object_json){
        std::filesystem::path file_path(mesh_data["file_path"].asString());

        models_.emplace_back(file_path);

        // some how change because other things.
        // if we want glow or color in the model
        // get to that eventually
    }
}

} // namespace entity
