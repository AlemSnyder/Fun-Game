#include "bordered_window.hpp"

namespace gui {

namespace the_buttons {

BorderedWindow::BorderedWindow(
    std::shared_ptr<render::WindowTexture> data, glm::ivec2 position,
    glm::ivec2 frame_size
) :
    Frame(
        position, frame_size,
        {position, glm::ivec2(position.x + frame_size.x, position.y),
         position + frame_size, glm::ivec2(position.x, position.y + frame_size.y)}
    ),
    data_(data) {
    data_->update_position(get_bounding_box());
}

} // namespace the_buttons
} // namespace gui
