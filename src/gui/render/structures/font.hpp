#pragma once

#include "../gpu_data/texture.hpp"

#include <memory>
#include <vector>

namespace gui {

namespace render {

namespace structures {

struct Character {
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
    glm::ivec4 position_in_texture;
};

class FontTexture {
 private:
    std::shared_ptr<gpu_data::Texture2D> texture_;

    std::unordered_map<char, Character> font_textures_;

 public:
    FontTexture(std::filesystem::path font_file);

    inline virtual void
    bind(GLuint texture_index) const {
        texture_->bind(texture_index);
    }
};

} // namespace structures

} // namespace render

} // namespace gui
