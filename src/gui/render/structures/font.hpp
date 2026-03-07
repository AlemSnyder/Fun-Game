#pragma once

#include "../gpu_data/texture.hpp"

#include <memory>
#include <vector>

namespace gui {

namespace render {

namespace structures {

struct Character {
    glm::uvec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
    glm::ivec4 position_in_texture;
};

class FontTexture {
 private:
    std::shared_ptr<gpu_data::Texture2D> texture_;

    std::unordered_map<char, Character> font_textures_;

    uint16_t text_height_;
    uint16_t ascender_height_;
    uint16_t
        descender_height_; // the distance from the baseline to the top of the character

 public:
    FontTexture(std::filesystem::path font_file);

    inline virtual void
    bind(GLuint texture_index) const {
        texture_->bind(texture_index);
    }

    [[nodiscard]] inline const Character*
    get_character(char character) const {
        const auto data = font_textures_.find(character);
        if (data != font_textures_.end()) {
            return &data->second;
        } else {
            LOG_WARNING(
                logging::main_logger,
                "Could not find character \"{}\" with value \"{}\" "
                "in font.",
                std::string(1, character), static_cast<int>(character)
            );
            return nullptr;
        }
    }

    [[nodiscard]] inline auto
    get_text_height() const {
        return text_height_;
    }

    [[nodiscard]] inline auto
    get_ascender_height() const {
        return ascender_height_;
    }

    [[nodiscard]] inline auto
    get_descender_height() const {
        return descender_height_;
    }
};

} // namespace structures

} // namespace render

} // namespace gui
