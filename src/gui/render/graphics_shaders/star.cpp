#include "star.hpp"

#include "../../../util/files.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "../data_structures/star_data.hpp"
#include "opengl_program.hpp"
#include "shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>

namespace gui {

namespace render {

StarRenderer::StarRenderer(
    std::shared_ptr<render::LightEnvironment> lighting,
    std::shared_ptr<render::StarRotation> star_rotation, shader::Program& shader_program
) :
    OpenGLProgramExecuter(shader_program),
    star_data_(files::get_data_path() / "stars.json"), lighting_(lighting),
    star_rotation_(star_rotation){
    reload_program();
}

void
StarRenderer::reload_program() {
    view_projection_UID_ = get_uniform("MVP");
    pixel_projection_UID_ = get_uniform("pixel_projection");
    star_rotation_UID_ = get_uniform("star_rotation");
    sun_position_UID_ = get_uniform("sun_position");
    sunlight_color_UID_ = get_uniform("sunlight_color");
}

void
StarRenderer::render(screen_size_t width, screen_size_t height, GLuint frame_buffer)
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

    /****************
     * Star Program *
     ****************/

    // Use our shader
    glUseProgram(get_program_ID());

    // the sun direction
    glUniform3f(
        sun_position_UID_, light_direction.x, light_direction.y, light_direction.z
    );
    // the sun color
        glUniform3f(
        sunlight_color_UID_, light_color.r, light_color.g, light_color.b
    );

    glm::mat4 sky_rotation_matrix = star_rotation_->get_sky_rotation();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(view_projection_UID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(pixel_projection_UID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniformMatrix4fv(
        star_rotation_UID_, 1, GL_FALSE, &sky_rotation_matrix[0][0]
    );

    // 1st attribute buffer : positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, star_data_.get_star_positions());
    glVertexAttribPointer(
        0,        // attribute
        4,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        nullptr   // array buffer offset
    );

    // 2nd attribute buffer : age
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, star_data_.get_star_age_());
    glVertexAttribPointer(
        1,        // attribute
        1,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        nullptr   // array buffer offset
    );

    // 3rd attribute buffer : vertices
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, star_data_.get_star_shape());
    glVertexAttribPointer(
        2,        // attribute
        2,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        nullptr   // array buffer offset
    );

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    // Draw the triangles !
    glDrawArraysInstanced(
        GL_TRIANGLE_STRIP,         // mode
        0,                         // start
        4,                         // number of vertices
        star_data_.get_num_stars() // number of models

    );

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

} // namespace render

} // namespace gui
