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
    star_PID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "background" / "Stars.vert",
        files::get_resources_path() / "shaders" / "background" / "Stars.frag"
    );
    sun_PID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "background" / "Sun.vert",
        files::get_resources_path() / "shaders" / "background" / "Sun.frag"
    );

    sky_PID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "background" / "Sky.frag"
    );

    // ---- set uniforms ----
    // star
    view_projection_star_UID_ = glGetUniformLocation(star_PID_, "MVP");
    pixel_projection_star_UID_ = glGetUniformLocation(star_PID_, "pixel_projection");
    star_rotation_star_UID_ = glGetUniformLocation(star_PID_, "star_rotation");
    sun_position_star_UID_ = glGetUniformLocation(star_PID_, "sun_position");

    // sun
    view_projection_sun_UID_ = glGetUniformLocation(sun_PID_, "MVP");
    pixel_projection_sun_UID_ = glGetUniformLocation(sun_PID_, "pixel_projection");
    sun_position_sun_UID_ = glGetUniformLocation(sun_PID_, "sun_position");
    sunlight_color_sun_UID_ = glGetUniformLocation(sun_PID_, "sunlight_color");

    // sky
    view_projection_sky_UID_ = glGetUniformLocation(sky_PID_, "MVIP");
    pixel_projection_sky_UID_ = glGetUniformLocation(sky_PID_, "pixel_projection");
    sun_position_sky_UID_ = glGetUniformLocation(sky_PID_, "sun_position");
    sunlight_color_sky_UID_ = glGetUniformLocation(sky_PID_, "sunlight_color");
}

SkyRenderer::~SkyRenderer() {
    glDeleteProgram(star_PID_);
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
    /***************
     * Sky Program *
     ***************/
    glUseProgram(sky_PID_);

    glm::mat4 sky_rotation = glm::inverse(MVP);

    // "pixel_projection" uniform
    glUniformMatrix4fv(pixel_projection_sky_UID_, 1, GL_FALSE, &pixel_window[0][0]);

    // "MVIP" uniform I for inverse
    glUniformMatrix4fv(view_projection_sky_UID_, 1, GL_FALSE, &sky_rotation[0][0]);

    // the sun direction
    glUniform3f(
        sun_position_sky_UID_, light_direction.x, light_direction.y, light_direction.z
    );

    // the sun color
    glUniform3f(sunlight_color_sky_UID_, light_color.x, light_color.y, light_color.z);

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

    /****************
     * Star Program *
     ****************/

    // Use our shader
    glUseProgram(star_PID_);

    // the sun direction
    glUniform3f(
        sun_position_star_UID_, light_direction.x, light_direction.y, light_direction.z
    );

    glm::mat4 sky_rotation_matrix = environment_.get_sky_rotation();

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(view_projection_star_UID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(pixel_projection_star_UID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniformMatrix4fv(
        star_rotation_star_UID_, 1, GL_FALSE, &sky_rotation_matrix[0][0]
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
    // glDisableVertexAttribArray(2);

    // Use our shader
    glUseProgram(sun_PID_);

    glUniform3f(sunlight_color_sun_UID_, light_color.x, light_color.y, light_color.z);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(view_projection_sun_UID_, 1, GL_FALSE, &MVP[0][0]);
    // in the "pixel_projection" uniform
    glUniformMatrix4fv(pixel_projection_sun_UID_, 1, GL_FALSE, &pixel_window[0][0]);

    glUniform3f(
        sun_position_sun_UID_, light_direction.x, light_direction.y, light_direction.z
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
