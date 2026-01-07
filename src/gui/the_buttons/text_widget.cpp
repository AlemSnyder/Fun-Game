#include "text_widget.hpp"

#include "user_interface.hpp"

namespace gui {
namespace the_buttons {
void
TextWidget::update_text_data(bool differed) {
    std::vector<glm::uvec4> data = generate_data();

    // data.push_back(glm::ivec4(2, 2, 100, 15));
    // data.push_back(glm::ivec4(2, 10, 100, 0));
    // data.push_back(glm::ivec4(28, 2, 115, 15));
    // data.push_back(glm::ivec4(28, 10, 115, 0));

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

std::vector<glm::uvec4>
TextWidget::generate_data() const {
    std::vector<glm::uvec4> data;

    int advance = 0;

    int line_spacing = 25; // eed to read this from the font
    int line_advance = line_spacing;

    for (char character : text_) {
        const render::structures::Character* character_data =
            font_->get_character(character);
        if (!character_data) {
            LOG_WARNING(
                logging::main_logger,
                "Could not find character \"{}\" with value \"{}\" in font. Required "
                "for string \"{}\".",
                std::string(1, character), static_cast<int>(character), text_
            );
        }

        // if (advance + character_data->advance > frame width or character is new_line)
        // { advance = 0; line_advance += line_spacing}

        glm::ivec4 position_in_texture = character_data->position_in_texture;

        glm::ivec2 bearing = character_data->bearing;
        glm::ivec2 size = character_data->size;

        data.push_back(glm::ivec4(
            bearing.x, line_advance - bearing.y + size.y, position_in_texture.x,
            position_in_texture.y
        ));
        data.push_back(glm::ivec4(
            bearing.x, line_advance - bearing.y, position_in_texture.x,
            position_in_texture.w
        ));
        data.push_back(glm::ivec4(
            bearing.x + size.x, line_advance - bearing.y + size.y,
            position_in_texture.z, position_in_texture.y
        ));
        data.push_back(glm::ivec4(
            bearing.x + size.x, line_advance - bearing.y, position_in_texture.z,
            position_in_texture.w
        ));

        advance += character_data->advance;
    }

    return data;
}

} // namespace the_buttons

} // namespace gui
