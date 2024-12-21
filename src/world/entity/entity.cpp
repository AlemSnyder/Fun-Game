#include "entity.hpp"

#include "util/files.hpp"

namespace world {

namespace entity {

Entity::Entity(const Mesh& mesh) :
    mesh_and_positions_(std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(
        mesh, std::vector<glm::mat4>()
    )) {}

Entity::Entity(
    const object_t& object_data, const manifest::descriptor_t& identification_data
) :
    name_(object_data.name),
    identification_(identification_data.identification) {
    const auto& model_data = object_data.models[0];
    // read mesh from path
    std::filesystem::path object_path_copy = identification_data.path;

    voxel_utility::VoxelObject model(
        files::get_data_path() / object_path_copy.remove_filename() / model_data.path
    );

    auto mesh = ambient_occlusion_mesher(model);

    mesh_and_positions_ =
        std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(mesh);
}

bool
Entity::remove(size_t index) {
    if (index >= local_positions_.size()) [[unlikely]] {
        return false;
    }
    local_positions_.erase(local_positions_.begin() + index);
    // for (){}
    //  TODO update entity data_position
    return true;
}

bool
Entity::assign(size_t index, glm::mat4& data) {
    if (index >= local_positions_.size()) [[unlikely]] {
        return false;
    }
    local_positions_[index] = data;
    return true;
}

std::string
Entity::identification() const {
    return identification_;
}

void
Entity::init_render(render_programs_t& programs) const {
    programs.entity_render_program->data.push_back(mesh_and_positions_.get());
}

void
Entity::update() {
    mesh_and_positions_->update_transforms_array(local_positions_, 0);
}

EntityInstance::EntityInstance(std::shared_ptr<Entity> entity_type) :
    entity_type_(entity_type), data_position_(entity_type->add()) {}

EntityInstance::~EntityInstance() {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        entity_type->remove(data_position_);
    }
}

void
EntityInstance::update(glm::mat4&& data) {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        entity_type->assign(data_position_, data);
    }
}

void
EntityInstance::destroy() {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        entity_type->remove(data_position_);
    }
    data_position_ = -1; // ya I know assigning -1 to size_t
}

glm::vec3
EntityInstance::get_position() const {
    if (std::shared_ptr<Entity> entity_type = entity_type_.lock()) {
        // entity_type->;
        return {1, 1, 1};
    }

    return {0, 0, 0};
}

std::shared_ptr<Object>
EntityInstance::get_object() {
    return entity_type_.lock();
}

std::shared_ptr<const Object>
EntityInstance::get_object() const {
    return entity_type_.lock();
}

} // namespace entity

} // namespace world
