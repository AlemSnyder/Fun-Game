#include "text_widget.hpp"

#include "global_context.hpp"
#include "user_interface.hpp"

namespace gui {
namespace the_buttons {
void
TextWidget::update_text_data() {
    // just run this on a back end thread
    std::vector<glm::uvec4> data = generate_data();

    // data.push_back(glm::ivec4(2, 2, 100, 15));
    // data.push_back(glm::ivec4(2, 10, 100, 0));
    // data.push_back(glm::ivec4(28, 2, 115, 15));
    // data.push_back(glm::ivec4(28, 10, 115, 0));

    text_data_.insert(data, 0, text_data_.size());

    std::vector<uint16_t> elements_data;
    for (size_t i = 0; i < num_characters_; i++) {
        elements_data.push_back(4 * i + 0);
        elements_data.push_back(4 * i + 1);
        elements_data.push_back(4 * i + 2);
        elements_data.push_back(4 * i + 2);
        elements_data.push_back(4 * i + 1);
        elements_data.push_back(4 * i + 3);
    }

    element_array_.insert(elements_data, 0, element_array_.size());
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


    // TODO I want to put all of this into a class. Some sort of text alignment
    // iterator.
    unsigned int text_scale = text_scale_;
    bool wrap_text = wrap_text_;

    unsigned int line_spacing = font_->get_text_height();
    unsigned int baseline = font_->get_ascender_height();

    unsigned int text_width = frame_size_.y / text_scale;
    int text_height = frame_size_.y / text_scale;

    unsigned int character_advance = 0;
    int line_advance = text_height - baseline;
    bool previous_char = false;
    bool white_space_after_char = false;
    size_t words_checked_until = 0;

    LOG_BACKTRACE(logging::main_logger, "Generating data for string \"{}\".", text_);

    for (size_t character_position = 0; character_position < text_.size();
         character_position++) {
        char character = text_.at(character_position);
        if (character == '\n') {
            character_advance = 0;
            line_advance -= line_spacing;
            previous_char = false;
            white_space_after_char = false;
            continue;
        } else if (character == '\t') {
            character_advance = character_advance + (40 - character_advance % 40);
            if (previous_char) {
                white_space_after_char = true;
            }
            continue;
        }
        // ''
        else {
            if (wrap_text) {
                if (character_position >= words_checked_until) {
                    // check that the entirety of the word is safe
                    if (white_space_after_char) {
                        // this is not the first word on the line
                        // check next word
                        unsigned int local_advance = character_advance;
                        size_t check_word_position = character_position;
                        for (; check_word_position < text_.size();
                             check_word_position++) {
                            char check_character = text_.at(check_word_position);

                            // if the character is white space
                            if (check_character == ' ' || check_character == '\n'
                                || check_character == '\t') {
                                break;
                            }
                            const render::structures::Character* check_character_data =
                                font_->get_character(check_word_position);
                            if (!check_character_data) {
                                continue;
                            }
                            local_advance += check_character_data->size.x;
                        }
                        if (local_advance > text_width) {
                            character_advance = 0;
                            line_advance -= line_spacing;
                            previous_char = false;
                            white_space_after_char = false;

                            if (local_advance < text_width * 2) {
                                words_checked_until = check_word_position;
                            }
                        }
                    } else {
                        const render::structures::Character* check_character_data =
                            font_->get_character(character);
                        if (!check_character_data) {
                            continue;
                        }

                        if (character_advance + check_character_data->size.x
                            > text_width) {
                            if (check_character_data->size.x > text_width) {
                                LOG_WARNING(
                                    logging::main_logger,
                                    "Character \'{}\' has width {}, but TextWidth has "
                                    "width {}.",
                                    std::string(1, character),
                                    check_character_data->size.x, text_width
                                );
                            } else {
                                // newline
                                character_advance = 0;
                                line_advance -= line_spacing;
                                previous_char = false;
                                white_space_after_char = false;
                            }
                        }
                        unsigned int local_advance = character_advance;
                        size_t check_word_position = character_position;
                        for (; check_word_position < text_.size();
                             check_word_position++) {
                            char check_character = text_.at(check_word_position);

                            // if the character is white space
                            if (check_character == ' ' || check_character == '\n'
                                || check_character == '\t') {
                                break;
                            }
                            const render::structures::Character* check_character_data =
                                font_->get_character(check_word_position);
                            if (!check_character_data) {
                                continue;
                            }
                            local_advance += check_character_data->size.x;
                            if (local_advance > text_width) {
                                check_word_position -= 1;
                                break;
                            }
                        }
                        words_checked_until = check_word_position;
                    }
                }
            }

            // if (advance + character_data->advance > frame width or character is
            // new_line) { advance = 0; line_advance += line_spacing} need to check for
            // word then break on word but also need to break within a word of the world
            // is too long

            // reading character data from font
            const render::structures::Character* character_data =
                font_->get_character(character);
            if (!character_data) {
                continue;
            }

            glm::ivec4 position_in_texture = character_data->position_in_texture;
            glm::ivec2 bearing = character_data->bearing;
            glm::ivec2 size = character_data->size;

            data.push_back(
                glm::ivec4(
                    character_advance + bearing.x, line_advance + bearing.y,
                    position_in_texture.x, position_in_texture.y
                )
            );
            data.push_back(
                glm::ivec4(
                    character_advance + bearing.x, line_advance + bearing.y - size.y,
                    position_in_texture.x, position_in_texture.w
                )
            );
            data.push_back(
                glm::ivec4(
                    character_advance + bearing.x + size.x, line_advance + bearing.y,
                    position_in_texture.z, position_in_texture.y
                )
            );
            data.push_back(
                glm::ivec4(
                    character_advance + bearing.x + size.x,
                    line_advance + bearing.y - size.y, position_in_texture.z,
                    position_in_texture.w
                )
            );

            character_advance += size.x;
        }
    }
    // I might want to suppress this as it might not be a problem
    if (line_advance > text_height) {
        LOG_WARNING(logging::main_logger, "Text larger then allocated size.");
    }
    return data;
}

} // namespace the_buttons

} // namespace gui
