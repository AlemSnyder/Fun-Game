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
    star_data_(files::get_data_path() / "stars.json"),
    environment_(environment) {
    star_programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Stars.vert",
        files::get_resources_path() / "shaders" / "Stars.frag"
    );
    sun_programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Sun.vert",
        files::get_resources_path() / "shaders" / "Sun.frag"
    );

    sky_programID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Sky.vert",
        files::get_resources_path() / "shaders" / "Sky.frag"
    );

    // ---- set uniforms ----
    star_view_projection_ID_ = glGetUniformLocation(star_programID_, "MVP");
    pixel_matrix_ID_ = glGetUniformLocation(star_programID_, "pixel_projection");
    star_rotation_ID_ = glGetUniformLocation(star_programID_, "sky_matrix");

    sun_view_projection_ID_ = glGetUniformLocation(sun_programID_, "MVP");
    sun_pixel_matrix_ID_ = glGetUniformLocation(sun_programID_, "pixel_projection");
    sun_sky_position_ID_ = glGetUniformLocation(sun_programID_, "sun_position");
    sun_sunlight_color_ID_ = glGetUniformLocation(sun_programID_, "sunlight_color");

    sky_view_projection_ID_ = glGetUniformLocation(sky_programID_, "MVIP");
    sky_pixel_matrix_ID_ = glGetUniformLocation(sky_programID_, "pixel_projection");
    sky_sky_position_ID_ = glGetUniformLocation(sky_programID_, "sun_position");
    sky_sunlight_color_ID_ = glGetUniformLocation(sky_programID_, "sunlight_color");
}

SkyRenderer::~SkyRenderer() {
    glDeleteProgram(star_programID_);
}

void
SkyRenderer::render(screen_size_t width, screen_size_t height, GLuint frame_buffer)
    const {
    // these matrices are used by all programs

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

    glm::vec3 light_direction = environment_.get_light_direction();

    glm::vec3 light_color = environment_.get_specular_light();

    // Bind framebuffer
    FrameBufferHandler::instance().bind_fbo(frame_buffer);
    // Draw over everything
    glDisable(GL_CULL_FACE);
    // The sky has no depth
    glDepthMask(GL_FALSE);

    glViewport(0, 0, width, height);

    // Draw the sky (blue)
    glUseProgram(sky_programID_);

    glm::mat4 sky_rotation = glm::inverse(MVP);

    // "pixel_projection" uniform
    glUniformMatrix4fv(sky_pixel_matrix_ID_, 1, GL_FALSE, &pixel_window[0][0]);

    // "MVIP" uniform I for inverse
    glUniformMatrix4fv(sky_view_projection_ID_, 1, GL_FALSE, &sky_rotation[0][0]);

    // the sun direction
    glUniform3f(
        sky_sky_position_ID_, light_direction.x, light_direction.y, light_direction.z
    );

    // the sun color
    glUniform3f(sky_sunlight_color_ID_, light_color.x, light_color.y, light_color.z);

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

    // Use our shader
    glUseProgram(star_programID_);

    glm::mat4 sky_rotation_matrix = environment_.get_sky_rotation();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(star_view_projection_ID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(pixel_matrix_ID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniformMatrix4fv(star_rotation_ID_, 1, GL_FALSE, &sky_rotation_matrix[0][0]);

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
    // glDisableVertexAttribArray(2);

    // Use our shader
    glUseProgram(sun_programID_);

    glUniform3f(sun_sunlight_color_ID_, light_color.x, light_color.y, light_color.z);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(sun_view_projection_ID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(sun_pixel_matrix_ID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniform3f(
        sun_sky_position_ID_, light_direction.x, light_direction.y, light_direction.z
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
