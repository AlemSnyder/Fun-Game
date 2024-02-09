
#pragma once

#include "gui/render/gpu_data/array_buffer.hpp"
#include "gui/render/gpu_data/static_mesh.hpp"
#include "json/json.h"
#include "placement.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <unordered_set>
#include <vector>

namespace world {

namespace entity {

// Think of this class as rose bush
// there are multiple rose bush models
// all model no positions
// Own file
class ObjectData {
 private:
    // std::vector<voxel_utility::VoxelObject> models_;

    std::vector<gui::gpu_data::StaticMesh> model_meshes_;

    // some size/off set center. Models must have the same size.

 public:
    ObjectData(Json::Value object_json);
};

// TODO move this to GPU
// mostly position
// there is a model controller for each voxel object model
class ModelController {
 private:
    // vector of placements
    std::unordered_set<Placement> placements_;

    // multiple array buffers

    // ObjectData object_data_;

    // array buffer generated from the model
    // and one from the transforms

    gui::gpu_data::StaticMesh* model_mesh_;

    // scale const int how much the model is scaled

 public:
    // some way to generate an instanced Int mesh renderer, except with scale

    void insert(Placement placement);

    void remove(Placement placement);
};

} // namespace entity

} // namespace world
