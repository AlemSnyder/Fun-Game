#include "model.hpp"

#include <iterator>
#include <filesystem>

namespace world {

namespace entity {

ObjectData::ObjectData(const Json::Value& object_json, std::filesystem::path object_path) {
    for (Json::Value mesh_data : object_json["models"]) {
        // each object may have multiple models
        std::filesystem::path file_path = object_path.remove_filename() / mesh_data["file_path"].asString();

        // generate a model from the given filepath
        voxel_utility::VoxelObject model(file_path);

        // generate a mesh from the model
        auto mesh = ambient_occlusion_mesher(model);
        // load the mesh to the gpu
        // this is bad
        // TODO
        model_meshes_.emplace_back(mesh);

        // some how change because other things.
        // if we want glow or color in the model
        // get to that eventually
        // read complex texture data from json
        // basically map color to
        // color, diffuse, ambient, specular
        // in addition to that need to generate multiple textures.
    }
}

void
ModelController::insert(Placement placement) {
    auto iter = placements_.insert(placement);

    // cpp crimes no more
    uint offset = std::distance(iter.first, placements_.begin());

    std::vector<uint8_t> texture_data;
    std::vector<glm::ivec4> data;
    for (auto iterator = iter.first; iterator != placements_.end(); iterator++) {
        data.push_back((*iterator).as_vec());
        texture_data.push_back((*iterator).texture_id);
    }

    model_mesh_.update_position(offset, data);
}

void
ModelController::remove(Placement placement) {
    // why would they do this?
    auto iter = placements_.erase(placements_.find(placement));
    uint offset = std::distance(iter, placements_.begin());

    std::vector<uint8_t> texture_data;
    // no conversion from position to ivec4
    // need to do ivec all the way down
    std::vector<glm::ivec4> data;
    for (auto iterator = iter; iterator != placements_.end(); iterator++) {
        data.push_back((*iterator).as_vec());
        texture_data.push_back((*iterator).texture_id);
    }

    model_mesh_.update_position(offset, data);
    model_textures_.update(texture_data, offset);
}

} // namespace entity

} // namespace world
