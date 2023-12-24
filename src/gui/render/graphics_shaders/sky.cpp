#include "sky.hpp"

#include "../../../util/files.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "opengl_program.hpp"
#include "shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>

namespace gui {

namespace render {

SkyRenderer::SkyRenderer(
    std::shared_ptr<render::LightEnvironment> lighting, shader::Program& shader_program
) :
    OpenGLProgramExecuter(shader_program), lighting_(lighting) {
    reload_program();
}

void
SkyRenderer::reload_program() {
    view_projection_UID_ = get_uniform("MVP");
    pixel_projection_UID_ = get_uniform("pixel_projection");
    sun_position_UID_ = get_uniform("sun_position");
    sunlight_color_UID_ = get_uniform("sunlight_color");
}

void
SkyRenderer::render(screen_size_t width, screen_size_t height, GLuint frame_buffer)
    const {
    // these matrices are used by all programs

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

    // Draw the sky (blue)
    /***************
     * Sky Program *
     ***************/
    glUseProgram(get_program_ID());

    glm::mat4 sky_rotation = glm::inverse(MVP);

    // "pixel_projection" uniform
    glUniformMatrix4fv(pixel_projection_UID_, 1, GL_FALSE, &pixel_window[0][0]);

    // "MVIP" uniform I for inverse
    glUniformMatrix4fv(view_projection_UID_, 1, GL_FALSE, &sky_rotation[0][0]);

    // the sun direction
    glUniform3f(
        sun_position_UID_, light_direction.x, light_direction.y, light_direction.z
    );

    // the sun color
    glUniform3f(sunlight_color_UID_, light_color.x, light_color.y, light_color.z);

    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, screen_data_.get_vertex_buffer());
    glVertexAttribPointer(
        0,        // attribute
        3,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        nullptr   // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_data_.get_element_buffer());

    // Draw the triangles @!
    glDrawElements(
        GL_TRIANGLE_STRIP,               // mode
        screen_data_.get_num_vertices(), // count
        GL_UNSIGNED_SHORT,               // type
        nullptr                          // element array buffer offset
    );

    glDisableVertexAttribArray(0);
}

} // namespace render

} // namespace gui
