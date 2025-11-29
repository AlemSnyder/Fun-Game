

#include "user_interface.hpp"

#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/uniform_types.hpp"
#include "manifest/object_handler.hpp"

namespace gui {
namespace the_buttons {

UserInterface::UserInterface(shader::ShaderHandler& shader_handler) {
    shader::Program& window_render_program = shader_handler.load_program(
        "Windows", files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "Red.frag"
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
    window_pipeline_ = std::make_shared<shader::ShaderProgram_Standard>(
        window_render_program, render_setup
    );
}

void
UserInterface::update(screen_size_t width, screen_size_t height) {
    FrameBufferHandler::instance().bind_fbo(0 // the screen
    );

    glClear(GL_DEPTH_BUFFER_BIT);

    window_pipeline_->render(width, height, 0);
}
} // namespace the_buttons
} // namespace gui
