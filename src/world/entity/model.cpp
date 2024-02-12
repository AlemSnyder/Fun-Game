#include "model.hpp"

#include <iterator>

namespace world {

namespace entity {

ObjectData::ObjectData(Json::Value object_json) {
    for (Json::Value mesh_data : object_json) {
        std::filesystem::path file_path(mesh_data["file_path"].asString());

        std::vector<voxel_utility::VoxelObject> models;

        models.emplace_back(file_path);

        for (const auto& model : models) {
            auto mesh = ambient_occlusion_mesher(model);
            model_meshes_.emplace_back(mesh);
        }

        // some how change because other things.
        // if we want glow or color in the model
        // get to that eventually
    }
}

void ModelController::insert(Placement placement) {
    auto iter = placements_.insert(placement);

    // cpp crimes no more
    uint offset = std::distance(iter.first, placements_.begin());

    model_mesh_->update_position(offset, {iter.first, placements_.end()});
}

 void ModelController::remove(Placement placement) {
    // why would they do this?
    auto iter = placements_.erase(placements_.find(placement));
    uint offset = std::distance(iter, placements_.begin());

    // no conversion from position to ivec4
    // need to do ivec all the way down
    std::vector<glm::ivec4> data(iter, placements_.end());

    model_mesh_->update_position(offset, data);
}

} // namespace entity

} // namespace world
