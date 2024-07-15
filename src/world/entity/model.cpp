#include "model.hpp"

#include <filesystem>
#include <iterator>

namespace world {

namespace entity {

    ModelController&
    ObjectData::get_model(size_t mesh_id) {
        return model_meshes_[mesh_id];
    }

    size_t
    ObjectData::num_models() const noexcept {
        return model_meshes_.size();
    }

    std::vector<world::entity::ModelController>::iterator
    ObjectData::begin() noexcept {
        return model_meshes_.begin();
    }

    std::vector<world::entity::ModelController>::iterator
    ObjectData::end() noexcept{
        return model_meshes_.end();
    }

ObjectData::ObjectData(
    const Json::Value& object_json, std::filesystem::path object_path
) :
    name_(object_json["name"].asString()),
    identification_(object_json["identification"].asString()) {
    for (Json::Value mesh_data : object_json["models"]) {
        // each object may have multiple models
        std::filesystem::path file_path =
            object_path.remove_filename() / mesh_data["file_path"].asString();

        // generate a model from the given filepath
        voxel_utility::VoxelObject model(file_path);

        // generate a mesh from the model
        auto mesh = ambient_occlusion_mesher(model);
        // load the mesh to the gpu
        model_meshes_.emplace_back(
            mesh, std::vector<std::vector<ColorFloat>>(
                      {color::convert_color_data(mesh.get_color_map())}
                  )
        );

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
ObjectData::update() {
    for (auto& mesh : model_meshes_) {
        mesh.update();
    }
}

void
ModelController::insert(Placement placement) {
    auto [iter, successes] = placements_.insert(placement);

    // if insertion fails
    if (!successes) {
        LOG_WARNING(
            logging::opengl_logger, "Failed to insert placement. Don't know why this "
                                    "would happen. Probably not enough memory."
        );
        return;
    }
    // cpp crimes no more
    // I was wrong
    uint offset_of_last_insertion = std::distance(placements_.begin(), iter);

    if (offset_of_last_insertion < offset_) {
        offset_ = offset_of_last_insertion;
    }
}

void
ModelController::remove(Placement placement) {
    // why would they do this?
    auto iter = placements_.erase(placements_.find(placement));
    uint offset_of_last_insertion = std::distance(placements_.begin(), iter);

    if (offset_of_last_insertion < offset_) {
        offset_ = offset_of_last_insertion;
    }
}

// call this once per frame
void
ModelController::update() {
    if (offset_ == NO_UPDATE) {
        return;
    }

    GlobalContext& context = GlobalContext::instance();
    context.push_task([this, &context]() {
        std::vector<uint8_t> texture_data;
        // no conversion from position to ivec4
        // need to do ivec all the way down
        std::vector<glm::ivec4> data;
        auto iterator = placements_.begin();
        for (size_t x = 0; x < offset_; x++, iterator++) {} // cpp crimes

        for (; iterator != placements_.end(); iterator++) {
            data.push_back((*iterator).as_vec());
            texture_data.push_back((*iterator).texture_id);
        }

        // queueing three things on main thread. will eventually be run in this order.
        model_mesh_.update_transforms_array(data, offset_);
        texture_id_.update(texture_data, offset_);
        context.push_opengl_task([this]() { reset_offset(); });
    });
}

} // namespace entity

} // namespace world
