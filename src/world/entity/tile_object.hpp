#pragma once

#include "gui/render/gpu_data/array_buffer.hpp"
#include "gui/render/gpu_data/static_mesh.hpp"
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

/**
 * @brief An object placed in the world.
 *
 * @details Anything with a position and a model.
 */
class TileObject {
    friend TileObjectOrder;

 private:
    //    const ObjectData& data_;

    // position and rotation
    Placement placement_;

    // object model
    ModelController& controller_;

 public:
    /**
     * @brief Construct a new TileObject
     *
     * @param ModelController& object model
     * @param Placement position and rotation of object
     */
    inline TileObject(ModelController& controller, Placement placement) :
        placement_(placement), controller_(controller) {
        controller_.insert(placement_);
    }

    ~TileObject() { controller_.remove(placement_); }

    /**
     * @brief Equivalents operator
     *
     * @details Used in sets. No two objects should be at the same position.
     */
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
