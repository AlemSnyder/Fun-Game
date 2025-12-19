#include "frame.hpp"

#include "logging.hpp"
#include "types.hpp"
#include "user_interface.hpp"
#include "widget.hpp"

#include <glm/fwd.hpp>

#include <memory>

namespace gui {

namespace the_buttons {

std::weak_ptr<const WidgetInterface>
FrameBase::get_child_at_position(screen_size_t x, screen_size_t y) const {
    for (const auto& child : children) {
        if (child->is_interior(x, y)) {
            if (child->has_children()) {
                return child->get_child_at_position(x - position_.x, y - position_.y);
            } else {
                return child;
            }
        }
    }

    LOG_WARNING(logging::main_logger, "Failed to find frame.");
    return {};
}

bool
FrameBase::is_interior(screen_size_t x, screen_size_t y) const {
    const auto bounding_box = get_bounding_box();
    if (x < bounding_box[0] || y < bounding_box[1] || x > bounding_box[2]
        || y > bounding_box[3]) {
        return false;
    }

    //clang-format off
    /*
     *    (previous_position)
     *    *
     *                        * (current position)
     *         x
     *       (x, y)
     *    |----|--------------|
     *     w1          w2
     *
     *             w2 * previous_position.y + w1 * current_position.y
     * y_line =  ------------------------------------------------------
     *                                   w1 + w2
     *
     * If there are an odd number of lines above the position then it is on the
     * interior.
     */
    //clang-format on

    bool interior_flag = false;
    glm::ivec2 previous_position = exterior_points_.back();

    for (const auto& current_position : exterior_points_) {
        int x_weight_1 = std::abs(previous_position.x - x);
        int x_weight_2 = std::abs(current_position.x - x);

        if (y * (x_weight_1 + x_weight_2)
            > previous_position.y * x_weight_2 + current_position.y * x_weight_1) {
            interior_flag = !interior_flag;
        }

        previous_position = current_position;
    }

    return interior_flag;
}

void
FrameBase::render_children(
    const UserInterface* user_interface, screen_size_t x_position,
    screen_size_t y_position
) const {
    for (const auto& child : children) {
        user_interface->render_frame(
            child, x_position + position_.x, y_position + position_.y
        );
    }
}

} // namespace the_buttons
} // namespace gui
