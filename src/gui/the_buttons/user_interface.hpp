#pragma once

#include "../render/graphics_shaders/shader_program.hpp"
#include "frame.hpp"
#include "gui/render/structures/uniform_types.hpp"
#include "widget.hpp"

#include <list>
#include <memory>

namespace gui {
namespace the_buttons {

class UserInterface {
 private:
    // uniform

    // widget renderer
    std::shared_ptr<shader::ShaderProgram_Standard> window_pipeline_;

    std::list<std::shared_ptr<Widget>> frames_;

 public:
    UserInterface(shader::ShaderHandler& shader_handler);

    void update(screen_size_t width, screen_size_t height);

    inline void
    add(std::shared_ptr<Widget> frame) {
        auto pos = frames_.begin();
        frames_.insert(pos, frame);
        window_pipeline_->data.push_back(frame.get());
    }
};

} // namespace the_buttons
} // namespace gui