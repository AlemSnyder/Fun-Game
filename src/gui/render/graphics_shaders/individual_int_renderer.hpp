// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file individual_int_renderer.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI  MODELS
 *
 */
#pragma once

#include "../../../util/files.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"
#include "../../shader.hpp"
#include "../graphics_data/individual_int.hpp"
#include "gui_render_types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

namespace gui {

namespace models {

/**
 * @brief Renders the meshes to the screen
 *
 * @details NonInstancedIMeshRenderer renders the meshes given to the screen.
 * This class handles IndividualInt data structures and classes that derive from
 * IndividualInt.
 */
template <data_structures::NonInstancedIMeshGPUDataType T>
class NonInstancedIMeshRenderer :
    public render_to::FrameBufferMultisample,
    public render_to::FrameBuffer,
    public render_to::ShadowMap {
 protected:
    GLuint program_id_render_; // ID of render program
    GLuint program_id_shadow_; // ID of shadow program

    GLuint matrix_id_render_;      // uniform ID of transform matrix
    GLuint view_matrix_id_render_; // ID of view projection matrix for indexed meshes
    GLuint depth_bias_id_render_;  // ID of depth projection matrix for indexed meshes
    GLuint shadow_map_id_render_;  // ID of the shadow map for indexed meshes
    GLuint color_map_id_render_;   // ID of the color map for indexed meshes
    GLuint light_direction_id_render_; // ID of the light direction uniform for indexed

    GLuint depth_bias_id_shadow_; // ID of depth projection matrix for indexed meshes
    // ------ the below are added to the class ------
    GLuint depth_texture_;              // ID of the shadow depth texture
    glm::vec3 light_direction_;         // direction of sun light
    glm::mat4 depth_projection_matrix_; // projection matrix of the light source
    glm::mat4 depth_view_matrix_; // convert a point in world space to depth in light
    //  light direction
    std::vector<std::shared_ptr<T>> meshes_;

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     * @param ShaderHandler
     */
    NonInstancedIMeshRenderer(ShaderHandler shader_handler = ShaderHandler());

    ~NonInstancedIMeshRenderer();

    /**
     * @brief adds a non-indexed mesh so it will cast a shadow
     *
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<T> mesh);

    /**
     * @brief Set the depth texture ID
     *
     * @param texture_id the depth texture ID
     */
    void set_depth_texture(GLuint texture_id);

    /**
     * @brief Set the light direction vector
     *
     * @param light_direction the direction of the light
     */
    void set_light_direction(glm::vec3 light_direction);

    /**
     * @brief Set the depth projection matrix
     *
     * @param depth_projection_matrix the projection matrix
     */
    void set_depth_projection_matrix(glm::mat4 depth_projection_matrix);

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     * @param GLuint frame buffer to render to
     */
    void render_frame_buffer(GLFWwindow* window, GLuint frame_buffer = 0) const override;

    /**
     * @brief renders the given meshes to multisample frame buffer
     *
     * @param window the OpenGL window
     * @param GLuint frame buffer to render to
     */
    void render_frame_buffer_multisample(GLFWwindow* window, GLuint frame_buffer = 0)
        const override;

    /**
     * @brief renders the given meshes to a shadow map
     *
     * @param screen_size_t shadow map width
     * @param screen_size_t shadow map height
     */
    void render_shadow_map(
        screen_size_t shadow_width_, screen_size_t shadow_height_, GLuint frame_buffer
    ) const override;

 protected:
    void load_vertex_buffer(std::shared_ptr<T> mesh) const;

    void load_color_buffers(std::shared_ptr<T> mesh) const;

    void setup_render() const;

    void setup_shadow() const;
};

template <data_structures::NonInstancedIMeshGPUDataType T>
NonInstancedIMeshRenderer<T>::NonInstancedIMeshRenderer(ShaderHandler shader_handler) {
    // non-indexed program
    program_id_render_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "ShadowMapping.frag"
    );
    // indexed program
    program_id_shadow_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "DepthRTT.vert",
        files::get_resources_path() / "shaders" / "DepthRTT.frag"
    );
    // ------ indexed program ------
    matrix_id_render_ = glGetUniformLocation(program_id_render_, "MVP");
    view_matrix_id_render_ = glGetUniformLocation(program_id_render_, "V");
    depth_bias_id_render_ = glGetUniformLocation(program_id_render_, "DepthBiasMVP");
    shadow_map_id_render_ = glGetUniformLocation(program_id_render_, "shadowMap");
    color_map_id_render_ = glGetUniformLocation(program_id_render_, "meshColors");
    light_direction_id_render_ =
        glGetUniformLocation(program_id_render_, "LightInvDirection_worldspace");

    depth_bias_id_shadow_ = glGetUniformLocation(program_id_shadow_, "depthMVP");
}

template <data_structures::NonInstancedIMeshGPUDataType T>
NonInstancedIMeshRenderer<T>::~NonInstancedIMeshRenderer() {
    glDeleteProgram(program_id_render_);
    glDeleteProgram(program_id_shadow_);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::add_mesh(std::shared_ptr<T> mesh) {
    meshes_.push_back(std::move(mesh));
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::set_depth_texture(GLuint texture_id) {
    depth_texture_ = texture_id;
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = light_direction;
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::set_depth_projection_matrix(glm::mat4 depth_projection_matrix
) {
    depth_projection_matrix_ = depth_projection_matrix;
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::load_vertex_buffer(std::shared_ptr<T> mesh) const {
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

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::load_color_buffers(std::shared_ptr<T> mesh) const {
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
    glUniform1i(color_map_id_render_, 2);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::setup_render() const {
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
    glUseProgram(program_id_render_);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(matrix_id_render_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(view_matrix_id_render_, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(depth_bias_id_render_, 1, GL_FALSE, &depth_bias_MVP[0][0]);

    // set the light direction uniform
    glUniform3f(
        light_direction_id_render_, light_direction_.x, light_direction_.y,
        light_direction_.z
    );

    // Bind Shadow Texture to Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_texture_);
    glUniform1i(shadow_map_id_render_, 1);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::setup_shadow() const {
    // Cull back-facing triangles -> draw only front-facing triangles
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    // Clear the screen
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_id_shadow_);

    // matrix to calculate the length of a light ray in model space
    glm::mat4 depthMVP = depth_projection_matrix_ * depth_view_matrix_;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform (Model View Projection)
    glUniformMatrix4fv(depth_bias_id_shadow_, 1, GL_FALSE, &depthMVP[0][0]);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::render_frame_buffer_multisample(
    GLFWwindow* window, GLuint frame_buffer
) const {
    render_frame_buffer(window, frame_buffer);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::render_frame_buffer(GLFWwindow* window, GLuint frame_buffer)
    const {
    // Render to the screen
    gui::FrameBufferHandler::getInstance().bind_fbo(frame_buffer);

    // get the window size
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

        load_vertex_buffer(mesh);

        load_color_buffers(mesh);

        // Draw the triangles !
        glDrawElements(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0                  // element array buffer offset
        );
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::render_shadow_map(
    screen_size_t shadow_width_, screen_size_t shadow_height_, GLuint frame_buffer_name_
) const {
    gui::FrameBufferHandler::getInstance().bind_fbo(frame_buffer_name_);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, shadow_width_, shadow_height_);

    setup_shadow();

    // draw the indexed meshes
    for (std::shared_ptr<T> mesh : meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        load_vertex_buffer(mesh);

        // Draw the triangles !
        glDrawElements(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0                  // element array buffer offset
        );

        glDisableVertexAttribArray(0);
    }
}

} // namespace models

} // namespace gui
