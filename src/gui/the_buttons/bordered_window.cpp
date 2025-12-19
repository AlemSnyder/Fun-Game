#include "bordered_window.hpp"

#include "user_interface.hpp"

namespace gui {

namespace the_buttons {

BorderedWindow::BorderedWindow(
    std::shared_ptr<render::WindowTexture> data, glm::ivec2 position,
    glm::ivec2 frame_size
) :
    FrameBase(
        position, frame_size,
        {position, glm::ivec2(position.x + frame_size.x, position.y),
         position + frame_size, glm::ivec2(position.x, position.y + frame_size.y)}
    ),
    data_(data) {
    data_->update_position(get_bounding_box());
}

void
BorderedWindow::user_interface_render(
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
