#pragma once

#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/uniform_types.hpp"
#include "frame.hpp"
#include "gui/render/structures/uniform_types.hpp"

#include <list>
#include <memory>

namespace gui {
namespace the_buttons {

class UserInterface {
 private:
    // uniform
    std::shared_ptr<render::FrameSizeUniform> frame_size_uniform_;
    std::shared_ptr<render::UIScaleUniform> ui_scale_uniform_;
    std::shared_ptr<render::TextureUniform> frame_texture_uniform_;
    std::shared_ptr<render::TextureRegionsUniform> texture_regions_;

    // widget renderer
    std::shared_ptr<shader::ShaderProgram_Windows> window_pipeline_;

    std::list<std::shared_ptr<Frame>> frames_;

 public:
    UserInterface(shader::ShaderHandler& shader_handler, uint8_t ui_scale);

    void update(screen_size_t width, screen_size_t height);

    inline void
    set_ui_scale(uint8_t ui_scale) {
        ui_scale_uniform_->set_ui_scale(ui_scale);
    }

    inline void
    add(std::shared_ptr<Frame> frame) {
        auto pos = frames_.begin();
        frames_.insert(pos, frame);
    }
};

} // namespace the_buttons

} // namespace gui
