#include "entity.hpp"

#include "../util/voxel.hpp"
#include "../util/voxel_io.hpp"

namespace entity {

Entity::Entity(const Json::Value& entity_data) : bones_(entity_data["bones"]) {
    // load qb file
    // voxel_utility::VoxelObject(entity_data[""])

    // TODO update from qb to give voxels for every layer

    // corelate mesh
}

glm::mat4
Entity::bone_transform(size_t bone_index) {
    return bones_.get_position(bone_index);
}

void Entity::update_bones(){
    // TODO need to find
    std::vector<glm::vec3> angles;
    // set everything to 0
    angles.resize(bones_.size());
    bones_.update(std::move(angles));
}

// TODO write all the other getters, and setters

} // namespace entity
