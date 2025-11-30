

#include "user_interface.hpp"

#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/uniform_types.hpp"
#include "manifest/object_handler.hpp"

namespace gui {
namespace the_buttons {

UserInterface::UserInterface(shader::ShaderHandler& shader_handler) {
    shader::Program& window_render_program = shader_handler.load_program(
        "Windows", files::get_resources_path() / "shaders" / "overlay" / "Widget.vert",
        files::get_resources_path() / "shaders" / "overlay" / "FramedWindow.frag"
    );

    // Overwrites anything that was there before
    std::function<void()> render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    // uniforms
    // stars_program.set_uniform(matrix_view_projection_uniform, "MVP");

    // windows
    window_pipeline_ = std::make_shared<shader::ShaderProgram_Windows>(
        window_render_program, render_setup
    );
}

void
UserInterface::update(screen_size_t width, screen_size_t height) {
    // cascade update frames using width and height

    FrameBufferHandler::instance().bind_fbo(0); // the screen

    glClear(GL_DEPTH_BUFFER_BIT);

    for (const auto& frame : frames_) {
        if (!frame->do_render()) {
            continue;
        }
        // frame->update_position();
        const auto bounding_box = frame->get_bounding_box();
        window_pipeline_->render(
            bounding_box[0], bounding_box[1], bounding_box[2], bounding_box[3], 0,
            frame.get()
        );
    }
}

} // namespace the_buttons

} // namespace gui
