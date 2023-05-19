#include "entity.hpp"
#include "../util/voxel.hpp"
#include "../util/voxel_io.hpp"

entity::Entity::Entity(const Json::Value& entity_data) : bones_(entity_data["bones"]) {
    // load qb file
    //voxel_utility::VoxelObject(entity_data[""])

    // TODO update from qb to give voxels for every layer

    // corelate mesh
}
