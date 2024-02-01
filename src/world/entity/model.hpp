
#pragma once

#include "gui/render/array_buffer/array_buffer.hpp"
#include "gui/render/array_buffer/static_mesh.hpp"
#include "json/json.h"
#include "placement.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <set>
#include <vector>

namespace entity {

// Think of this class as rose bush
// there are multiple rose bush models
// all model no positions
// Own file
class ObjectData {
 private:
    // std::vector<voxel_utility::VoxelObject> models_;

    std::vector<gui::data_structures::StaticMesh> model_meshes_;

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
    std::set<Placement> placements_;

    // multiple array buffers

    // ObjectData object_data_;

    // array buffer generated from the model
    // and one from the transforms

    gui::data_structures::StaticMesh* model_mesh_;

    // scale const int how much the model is scaled

 public:
    // some way to generate an instanced Int mesh renderer, except with scale

    void add(Placement placement);

    void remove(Placement placement);
};

} // namespace entity
