#pragma once

#include "../gpu_data/texture.hpp"
#include <vector>
#include <memory>


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


};




class FontHandler {
    FontHandler(std::filesystem::path font_file);
};




}

}

} // namespace gui
