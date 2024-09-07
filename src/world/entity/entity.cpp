#include "entity.hpp"

namespace world {

namespace entity {

EntityHandler::EntityHandler(const Mesh& mesh) :
    mesh_and_positions_(std::make_shared<gui::gpu_data::FloatingInstancedIMeshGPU>(
        mesh, std::vector<glm::mat4>()
    )) {}

void
EntityHandler::reserve(size_t size) {
    local_positions_.resize(size);
}

void
EntityHandler::insert(glm::mat4&& data) {
    local_positions_.push_back(data);
}

bool
EntityHandler::remove(size_t index) {
    if (index >= local_positions_.size()) [[unlikely]] {
        return false;
    }
    local_positions_.erase(local_positions_.begin() + index);
    //for (){}
    // TODO update entity data_position
    return true;
}

bool
EntityHandler::assign(size_t index, glm::mat4& data) {
    if (index >= local_positions_.size()) [[unlikely]] {
        return false;
    }
    local_positions_[index] = data;
    return true;
}

void
EntityHandler::queue_sync() {
    mesh_and_positions_->update_transforms_array(local_positions_, 0);
}

Entity::Entity(EntityHandler& entity_type) :
    entity_type_(entity_type), data_position_(entity_type_.add()) {}

Entity::~Entity() {
    entity_type_.remove(data_position_);
}

void
Entity::update(glm::mat4&& data) {
    entity_type_.assign(data_position_, data);
}

} // namespace entity

} // namespace world
