#include "individual_int_renderer.hpp"

#include "../../../util/files.hpp"
#include "../../shader.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "../data/individual_int.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

namespace models {

template <class T>
IndividualIntRenderer<T>::IndividualIntRenderer(ShaderHandeler shader_handler) {
    // non-indexed program
    programID_render_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "ShadowMapping.frag"
    );
    // indexed program
    programID_shadow_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "DepthRTT.vert",
        files::get_resources_path() / "shaders" / "DepthRTT.frag"
    );
    // ------ indexed program ------
    matrix_ID_render_ = glGetUniformLocation(programID_render_, "MVP");
    view_matrix_ID_render_ = glGetUniformLocation(programID_render_, "V");
    depth_bias_ID_render_ = glGetUniformLocation(programID_render_, "DepthBiasMVP");
    shadow_map_ID_render_ = glGetUniformLocation(programID_render_, "shadowMap");
    color_map_ID_render_ = glGetUniformLocation(programID_render_, "meshColors");
    light_direction_ID_render_ =
        glGetUniformLocation(programID_render_, "LightInvDirection_worldspace");

    matrix_ID_shadow_ = glGetUniformLocation(programID_shadow_, "MVP");
    view_matrix_ID_shadow_ = glGetUniformLocation(programID_shadow_, "V");
    depth_bias_ID_shadow_ = glGetUniformLocation(programID_shadow_, "DepthBiasMVP");
    shadow_map_ID_shadow_ = glGetUniformLocation(programID_shadow_, "shadowMap");
    color_map_ID_shadow_ = glGetUniformLocation(programID_shadow_, "meshColors");
    light_direction_ID_shadow_ =
        glGetUniformLocation(programID_shadow_, "LightInvDirection_worldspace");
}

template <class T>
IndividualIntRenderer<T>::~IndividualIntRenderer() {
    glDeleteProgram(programID_render_);
    glDeleteProgram(programID_shadow_);
}

template <class T>
void
IndividualIntRenderer<T>::add_mesh(std::shared_ptr<T> mesh) {
    meshes_.push_back(std::move(mesh));
}

template <class T>
void
IndividualIntRenderer<T>::set_depth_texture(GLuint texture_id) {
    depth_texture_ = texture_id;
}

template <class T>
void
IndividualIntRenderer<T>::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = light_direction;
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

template <class T>
void
IndividualIntRenderer<T>::set_depth_projection_matrix(glm::mat4 depth_projection_matrix
) {
    depth_projection_matrix_ = depth_projection_matrix;
}

template <class T>
void
IndividualIntRenderer<T>::load_vertex_buffer(std::shared_ptr<T> mesh) const {
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_vertex_buffer());
    glVertexAttribIPointer(
        0,       // attribute
        3,       // size
        GL_INT,  // type
        0,       // stride
        (void*)0 // array buffer offset
    );

    // Index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->get_element_buffer());
}

template <class T>
void
IndividualIntRenderer<T>::load_color_buffers(std::shared_ptr<T> mesh) const {
    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_color_buffer());
    glVertexAttribIPointer(
        1,                 // attribute
        1,                 // size
        GL_UNSIGNED_SHORT, // type
        0,                 // stride
        (void*)0           // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_normal_buffer());
    glVertexAttribIPointer(
        2,       // attribute
        3,       // size
        GL_BYTE, // type
        0,       // stride
        (void*)0 // array buffer offset
    );

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_1D, mesh->get_color_texture());
    glUniform1i(color_map_ID_render_, 2);
}

template <class T>
void
IndividualIntRenderer<T>::setup_render() const {
    // Cull back-facing triangles -> draw only front-facing triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Enable the depth test, and enable drawing to the depth texture
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glm::mat4 depthMVP = depth_projection_matrix_ * depth_view_matrix_;

    // Compute the MVP matrix from keyboard and mouse input
    // controls::computeMatricesFromInputs(window);
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 view_matrix = controls::get_view_matrix();
    // glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

    // Shadow bias matrix of-sets the shadows
    glm::mat4 bias_matrix(
        0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 depth_bias_MVP = bias_matrix * depthMVP;

    // Use our shader
    glUseProgram(programID_render_);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_ID_render_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(view_matrix_ID_render_, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(depth_bias_ID_render_, 1, GL_FALSE, &depth_bias_MVP[0][0]);

    // set the light direction uniform
    glUniform3f(
        light_direction_ID_render_, light_direction_.x, light_direction_.y,
        light_direction_.z
    );

    // Bind Shadow Texture to Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_texture_);
    glUniform1i(shadow_map_ID_render_, 1);
}

template <class T>
void
IndividualIntRenderer<T>::setup_shadow() const {


    // matrix to calculate the length of a light ray in model space
    glm::mat4 depthMVP = depth_projection_matrix_ * depth_view_matrix_;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform (Model View Projection)
    glUniformMatrix4fv(depth_bias_ID_shadow_, 1, GL_FALSE, &depthMVP[0][0]);
    // Cull back-facing triangles -> draw only front-facing triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID_shadow_);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_ID_shadow_, 1, GL_FALSE, &depthMVP[0][0]);
}

template <class T>
void
IndividualIntRenderer<T>::render_frame_buffer(GLFWwindow* window, GLuint frame_buffer)
    const {
    // Render to the screen
    gui::FrameBufferHandler::bind_fbo(frame_buffer);

    // get he window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    setup_render();

    for (std::shared_ptr<T> mesh : meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        load_vertex_buffer();

        load_color_buffers();

        // Draw the triangles !
        glDrawElements(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0                  // element array buffer offset
        );
    }
}

template <class T>
void
IndividualIntRenderer<T>::render_shadow_map(
    int shadow_width_, int shadow_height_, GLuint frame_buffer_name_
) const {
    gui::FrameBufferHandler::bind_fbo(frame_buffer_name_);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, shadow_width_, shadow_height_);

    setup_shadow();

    // draw the indexed meshes
    for (std::shared_ptr<T> mesh : meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        load_vertex_buffer();

        // Draw the triangles !
        glDrawElements(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0                  // element array buffer offset
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
}

} // namespace models

} // namespace gui
