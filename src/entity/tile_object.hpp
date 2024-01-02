// holds true false voxel

// will need to make a data handler object so that json/lua can know how to find objects

#pragma once

#include "../util/voxel.hpp"
#include "json/json.h"

#include <vector>
#include <mutex>

namespace entity {


// Think of this class as rose bush
// there are multiple rose bush models
class ObjectData {
 private:
    std::vector<voxel_utility::VoxelObject> models_;

    // some size/off set center. Models must have the same size.

 public:
    ObjectData(Json::Value object_json);
};

// there is a model controller for each voxel object model
class ModelController{
 private:

    // vector of positions
    // multiple array buffers

    // array buffer generated from the model
    // and one from the transforms

    // scale const int how much the model is scaled

 public:
    // some way to generate an instanced Int mesh renderer, except with scale
};

class TileObject {
    const ObjectData& data_;

    TileObject();
};

} // namespace entity
