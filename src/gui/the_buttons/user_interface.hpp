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

    // widget renderer
    std::shared_ptr<shader::ShaderProgram_Windows> window_pipeline_;

    std::list<std::shared_ptr<Frame>> frames_;

 public:
    UserInterface(shader::ShaderHandler& shader_handler);

    void update(screen_size_t width, screen_size_t height);

    inline void
    add(std::shared_ptr<Frame> frame) {
        auto pos = frames_.begin();
        frames_.insert(pos, frame);
    }
};

} // namespace the_buttons
} // namespace gui