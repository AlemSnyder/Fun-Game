#include "bordered_widget.hpp"

#include "user_interface.hpp"

namespace gui {
namespace the_buttons {

void
BorderedWidget::user_interface_render(
    const UserInterface* user_interface, screen_size_t x_position,
    screen_size_t y_position
) const {
    user_interface->render_frame(this, x_position, y_position);

    for (const auto& child : children) {
        child->user_interface_render(
            user_interface, x_position + position_.x, y_position + position_.y
        );
    }
}
} // namespace the_buttons
} // namespace gui