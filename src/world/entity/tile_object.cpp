#include "tile_object.hpp"

#include "gui/render/structures/model.hpp"

namespace world {

namespace entity {

TileObjectInstance::TileObjectInstance(
    std::shared_ptr<TileObject> object_type, uint8_t model_id, gui::Placement placement
) : placement_(placement), model_id_(model_id), object_type_(object_type) {
    object_type->get_model(model_id_).insert(placement_);
}

TileObjectInstance::~TileObjectInstance() {
    if (std::shared_ptr<TileObject> object_type = object_type_.lock()) {
        object_type->get_model(model_id_).remove(placement_);
    }
}

std::shared_ptr<Object>
TileObjectInstance::get_object() {
    return object_type_.lock();
}

std::shared_ptr<const Object>
TileObjectInstance::get_object() const {
    return object_type_.lock();
}

gui::render::ModelController&
TileObject::get_model(size_t mesh_id) {
    if (mesh_id >= model_meshes_.size()) {
        throw std::invalid_argument("ID out of range");
    }
    return model_meshes_[mesh_id];
}

size_t
TileObject::num_models() const noexcept {
    return model_meshes_.size();
}

std::vector<gui::render::ModelController>::iterator
TileObject::begin() noexcept {
    return model_meshes_.begin();
}

std::vector<gui::render::ModelController>::iterator
TileObject::end() noexcept {
    return model_meshes_.end();
}

TileObject::TileObject(
    const object_t& object_data, const manifest::descriptor_t& identification_data
) : name_(object_data.name), identification_(identification_data.identification) {
    for (const model_t& model_data : object_data.models) {
        // each object may have multiple models
        std::filesystem::path object_path_copy = identification_data.path;
        std::filesystem::path file_path =
            object_path_copy.remove_filename() / model_data.path;

        // generate a model from the given filepath
        voxel_utility::VoxelObject model(files::get_data_path() / file_path);

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
TileObject::update() {
    for (auto& mesh : model_meshes_) {
        mesh.update();
    }
}

} // namespace entity

} // namespace world
