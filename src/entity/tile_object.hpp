// holds true false voxel

// will need to make a data handler object so that json/lua can know how to find objects

#pragma once

#include "gui/render/array_buffer/static_mesh.hpp"
#include "json/json.h"
#include "types.hpp"
#include "util/voxel.hpp"

#include <mutex>
#include <set>
#include <vector>

namespace entity {

// Think of this class as rose bush
// there are multiple rose bush models
class ObjectData {
 private:
    // std::vector<voxel_utility::VoxelObject> models_;

    // some size/off set center. Models must have the same size.

 public:
    ObjectData(Json::Value object_json);
};

// there is a model controller for each voxel object model
class ModelController {
 private:
    // vector of positions
    std::set<TerrainOffset3> positions_

        // multiple array buffers

        // ObjectData object_data_;

        // array buffer generated from the model
        // and one from the transforms

        gui::array_buffer::StaticMesh model_mesh_

        // scale const int how much the model is scaled

        public :
        // some way to generate an instanced Int mesh renderer, except with scale

        void
        add(TerrainOffset3 position);

    void remove(TerrainOffset3 position);
};

class TileObject {
 private:
    //    const ObjectData& data_;

    TerrainOffset3 position_;

    ModelController& controller_;

 public:
    inline TileObject(ModelController& controller, TerrainOffset3 position)
        position_(position) {
        controller_.add(position_).
    }

    ~TileObject() { controller_.remove(position_); }
};

} // namespace entity
