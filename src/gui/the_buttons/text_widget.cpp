#include "text_widget.hpp"

#include "user_interface.hpp"

namespace gui {
namespace the_buttons {
void
TextWidget::update_text_data(bool differed) {
    std::vector<glm::uvec4> data;

    data.push_back(glm::ivec4(2, 2, 100, 15));
    data.push_back(glm::ivec4(2, 10, 100, 0));
    data.push_back(glm::ivec4(28, 2, 115, 15));
    data.push_back(glm::ivec4(28, 10, 115, 0));

    text_data_.insert(data, 0, text_data_.size());
}

void
TextWidget::user_interface_render(
    const UserInterface* user_interface, screen_size_t x_position,
    screen_size_t y_position
) const {
    user_interface->render_frame(this, x_position, y_position);
}

void
TextWidget::attach_all() {
    text_data_.attach_to_vertex_attribute(0);
}

void
TextWidget::initialize() {
    vertex_array_object_.bind();
    attach_all();
    vertex_array_object_.release();
}
} // namespace the_buttons
} // namespace gui
