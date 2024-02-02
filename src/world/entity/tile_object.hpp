// holds true false voxel

// will need to make a data handler object so that json/lua can know how to find objects

#pragma once

#include "gui/render/array_buffer/array_buffer.hpp"
#include "gui/render/array_buffer/static_mesh.hpp"
#include "json/json.h"
#include "model.hpp"
#include "placement.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <mutex>
#include <set>
#include <vector>

namespace world {

namespace entity {

// the actual object in the world
class TileObject {
 private:
    //    const ObjectData& data_;

    Placement placement_;

    ModelController& controller_;

 public:
    inline TileObject(ModelController& controller, Placement placement) :
        placement_(placement), controller_(controller) {
        controller_.add(placement_);
    }

    ~TileObject() { controller_.remove(placement_); }
};

} // namespace entity

} // namespace world
