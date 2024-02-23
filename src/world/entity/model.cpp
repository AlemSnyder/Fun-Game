#include "model.hpp"

#include <iterator>

namespace world {

namespace entity {

ObjectData::ObjectData(Json::Value object_json) {
    for (Json::Value mesh_data : object_json) {
        // each object may have multiple models
        std::filesystem::path file_path(mesh_data["file_path"].asString());

        // generate a model from the given filepath
        voxel_utility::VoxelObject model(file_path);

        // generate a mesh from the model
        auto mesh = ambient_occlusion_mesher(model);
        // load the mesh to the gpu
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

    std::vector<glm::ivec4> data;
    for (auto iterator = iter.first; iterator != placements_.end(); iterator ++){
        data.push_back((*iterator).as_vec());
    }

    model_mesh_.update_position(offset, data);
}

void
ModelController::remove(Placement placement) {
    // why would they do this?
    auto iter = placements_.erase(placements_.find(placement));
    uint offset = std::distance(iter, placements_.begin());

    // no conversion from position to ivec4
    // need to do ivec all the way down
    std::vector<glm::ivec4> data;
    for (auto iterator = iter; iterator != placements_.end(); iterator ++){
        data.push_back((*iterator).as_vec());
    }

    model_mesh_.update_position(offset, data);
}

} // namespace entity

} // namespace world
