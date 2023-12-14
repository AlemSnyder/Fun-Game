#include "sky.hpp"

#include "../../../util/files.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "../../shader.hpp"
#include "../data_structures/sky_data.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <filesystem>

namespace gui {

namespace render {

SkyRenderer::SkyRenderer(
    scene::Environment_Cycle& environment, ShaderHandler shader_handler
) :
    sky_data_(files::get_data_path() / "stars.json"),
    environment_(environment) {
    programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Sky.vert",
        files::get_resources_path() / "shaders" / "Sky.frag"
    );
    sun_programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Sun.vert",
        files::get_resources_path() / "shaders" / "Sun.frag"
    );
    // ---- set uniforms ----
    matrix_view_projection_ID_ = glGetUniformLocation(programID_, "MVP");
    pixel_matrix_ID_ = glGetUniformLocation(programID_, "pixel_projection");
    sky_matrix_ID_ = glGetUniformLocation(programID_, "sky_matrix");

    sun_matrix_view_projection_ID_ = glGetUniformLocation(sun_programID_, "MVP");
    sun_pixel_matrix_ID_ = glGetUniformLocation(sun_programID_, "pixel_projection");
    sun_sky_position_ID_ = glGetUniformLocation(sun_programID_, "sun_position");
}

SkyRenderer::~SkyRenderer() {
    glDeleteProgram(programID_);
}

void
SkyRenderer::render(screen_size_t width, screen_size_t height, GLuint frame_buffer)
    const {
    // Bind framebuffer
    FrameBufferHandler::instance().bind_fbo(frame_buffer);
    // Draw over everything
    glDisable(GL_CULL_FACE);
    // The sky has no depth
    glDepthMask(GL_FALSE);

    glViewport(0, 0, width, height);

    // Use our shader
    glUseProgram(programID_);

    // Compute the MVP matrix from keyboard and mouse input
    // clang-format off
    glm::mat4 pixel_window = {
        1.0 / static_cast<float>(width), 0, 0, 0,
        0, 1.0 / static_cast<float>(height), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};
    // clang-format on

    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 view_matrix = controls::get_view_matrix();
    glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

    glm::mat4 sky_rotation_matrix = environment_.get_sky_rotation();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_view_projection_ID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(pixel_matrix_ID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniformMatrix4fv(sky_matrix_ID_, 1, GL_FALSE, &sky_rotation_matrix[0][0]);

    // 1st attribute buffer : positions
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, sky_data_.get_star_positions());
    glVertexAttribPointer(
        0,        // attribute
        4,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0  // array buffer offset
    );

    // 2nd attribute buffer : age
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, sky_data_.get_star_age_());
    glVertexAttribPointer(
        1,        // attribute
        1,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0  // array buffer offset
    );

    // 3rd attribute buffer : vertices
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, sky_data_.get_star_shape());
    glVertexAttribPointer(
        2,        // attribute
        2,        // size
        GL_FLOAT, // type
        GL_FALSE, // normalized?
        0,        // stride
        (void*)0  // array buffer offset
    );

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    // Draw the triangles !
    glDrawArraysInstanced(
        GL_TRIANGLE_STRIP,        // mode
        0,                        // start
        4,                        // number of vertices
        sky_data_.get_num_stars() // number of models

    );

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // glDisableVertexAttribArray(2);

    // Use our shader
    glUseProgram(sun_programID_);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(sun_matrix_view_projection_ID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(sun_pixel_matrix_ID_, 1, GL_FALSE, &pixel_window[0][0]);

    auto light_direction = environment_.get_light_direction();

    glUniform3f(
        sun_sky_position_ID_, light_direction.x, light_direction.y, light_direction.z
    );

    // Draw the triangles !
    glDrawArraysInstanced(
        GL_TRIANGLE_STRIP,        // mode
        0,                        // start
        4,                        // number of vertices
        sky_data_.get_num_stars() // number of models

    );

    glDisableVertexAttribArray(2);

    // Use our shader
    //glUseProgram(sun_programID_);
}

} // namespace render

} // namespace gui
