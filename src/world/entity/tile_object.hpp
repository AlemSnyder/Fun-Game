// holds true false voxel

// will need to make a data handler object so that json/lua can know how to find objects

#pragma once

#include "gui/render/gpu_data/array_buffer.hpp"
#include "gui/render/gpu_data/static_mesh.hpp"
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

struct TileObjectOrder;

// the actual object in the world
class TileObject {
    friend TileObjectOrder;

 private:
    //    const ObjectData& data_;

    Placement placement_;

    ModelController& controller_;

 public:
    inline TileObject(ModelController& controller, Placement placement) :
        placement_(placement), controller_(controller) {
        controller_.insert(placement_);
    }

    ~TileObject() { controller_.remove(placement_); }

    [[nodiscard]] inline bool
    operator==(const TileObject& other) const {
        return placement_ == other.placement_;
    }
};

struct TileObjectOrder {
    size_t
    operator()(const TileObject& tile_entity) const noexcept {
        const auto& pos = tile_entity.placement_;
        size_t result = 0;

        // Position
        utils::hash_combine(result, pos.x);
        utils::hash_combine(result, pos.y);
        utils::hash_combine(result, pos.z);

        // The last index is rotation, and this is irreverent to position.
        return result;
    }
};

} // namespace entity

} // namespace world
