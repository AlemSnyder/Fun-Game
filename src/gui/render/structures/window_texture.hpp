#pragma once

#include "gui/render/gpu_data/texture.hpp"
#include "screen_data.hpp"

#include <filesystem>

namespace gui {
namespace render {

class WindowTexture : public virtual gpu_data::ScreenData {
 private:
    //    gpu_data::Texture2D texture;

    std::array<uint8_t, 36> sub_texture_locations;

 public:
    inline WindowTexture(){}; // std::filesystem::path file);
    inline virtual ~WindowTexture(){};
};

} // namespace render
} // namespace gui