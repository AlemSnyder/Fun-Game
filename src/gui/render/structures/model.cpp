#include "model.hpp"

#include "fmt/core.h"
#include "util/files.hpp"

#include <filesystem>
#include <iterator>

namespace gui {

namespace render {

void
ModelController::insert(Placement placement) {
    std::lock_guard<std::mutex> lock(mut_);
    auto [iter, successes] = placements_.insert(placement);

    // if insertion fails
    if (!successes) {
        LOG_WARNING(
            logging::opengl_logger, "Failed to insert placement. Don't know why this "
                                    "would happen. Probably not enough memory."
        );
        return;
    }
    // cpp crimes no more
    // I was wrong
    uint offset_of_last_insertion = std::distance(placements_.begin(), iter);

    if (offset_of_last_insertion < offset_) {
        offset_ = offset_of_last_insertion;
    }
}

void
ModelController::remove(Placement placement) {
    std::lock_guard<std::mutex> lock(mut_);
    // why would they do this?
    auto iter = placements_.erase(placements_.find(placement));
    uint offset_of_last_insertion = std::distance(placements_.begin(), iter);

    if (offset_of_last_insertion < offset_) {
        offset_ = offset_of_last_insertion;
    }
}

// call this once per frame
void
ModelController::update() {
    std::lock_guard<std::mutex> lock(mut_);
    if (offset_ == NO_UPDATE) {
        return;
    }

    std::vector<uint8_t> texture_data;
    // no conversion from position to ivec4
    // need to do ivec all the way down
    std::vector<glm::ivec4> data;
    auto iterator = placements_.begin();
    assert(offset_ < placements_.size() && "Something Something This will break");
    std::advance(iterator, offset_);

    for (; iterator != placements_.end(); iterator++) {
        data.push_back((*iterator).as_vec());
        texture_data.push_back((*iterator).texture_id);
    }

    // queueing three things on main thread. will eventually be run in this order.
    model_mesh_.update_transforms_array(data, offset_);
    texture_id_.update(texture_data, offset_);
    reset_offset();
}

} // namespace render

} // namespace gui
