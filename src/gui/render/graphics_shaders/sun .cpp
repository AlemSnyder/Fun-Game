#include "../../../util/files.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "opengl_program.hpp"
#include "shader.hpp"
#include "sun.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>

namespace gui {

namespace render {

SunRenderer::SunRenderer(
    std::shared_ptr<render::LightEnvironment> lighting,
    std::shared_ptr<render::StarRotation> star_rotation, shader::Program& shader_program
) :
    OpenGLProgramExecuter(shader_program) {
    reload_program();
}

void
SunRenderer::reload_program() {
    view_projection_UID_ = get_uniform("MVP");
    pixel_projection_UID_ = get_uniform("pixel_projection");
    sun_position_UID_ = get_uniform("sun_position");
    sunlight_color_UID_ = get_uniform("sunlight_color");
}

void
SunRenderer::render(screen_size_t width, screen_size_t height, GLuint frame_buffer)
    const {
    glViewport(0, 0, width, height);

    // Use our shader
    glUseProgram(get_program_ID());

    // Compute the MVP matrix from keyboard and mouse input
    // clang-format off
    glm::mat4 pixel_window = {
        1.0 / static_cast<float>(width), 0, 0, 0,
        0, 1.0 / static_cast<float>(height), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    // clang-format on

    // Compute the MVP matrix from keyboard and mouse input
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 view_matrix = controls::get_view_matrix();
    glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

    glm::vec3 light_direction = lighting_->get_light_direction();

    glm::vec3 light_color = lighting_->get_specular_light();

    // Bind framebuffer
    FrameBufferHandler::instance().bind_fbo(frame_buffer);
    // Draw over everything
    glDisable(GL_CULL_FACE);
    // The sky has no depth
    glDepthMask(GL_FALSE);

    glViewport(0, 0, width, height);

    // glDisableVertexAttribArray(2);

    // Use our shader
    glUseProgram(get_program_ID());

    glUniform3f(sunlight_color_UID_, light_color.x, light_color.y, light_color.z);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(view_projection_UID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(pixel_projection_UID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniform3f(
        sun_position_UID_, light_direction.x, light_direction.y, light_direction.z
    );

    // Draw the triangles !
    glDrawArraysInstanced(
        GL_TRIANGLE_STRIP, // mode
        0,                 // start
        4,                 // number of vertices
        1                  // number of models (there is one sun)

    );

    glDisableVertexAttribArray(2);
}

} // namespace render

} // namespace gui
