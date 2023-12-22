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
 * @file non_instanced_i_mesh_renderer.hpp
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
#include "../graphics_data/non_instanced_i_mesh.hpp"
#include "gui_render_types.hpp"
#include "opengl_program.hpp"
#include "shader.hpp"

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
 * @details NonInstancedIMeshShadow renders the meshes given to the screen.
 * This class handles IndividualInt data structures and classes that derive from
 * IndividualInt.
 */
template <data_structures::NonInstancedIMeshGPUDataType T>
class NonInstancedIMeshShadow :
    public render_to::ShadowMap,
    public OpenGLProgramExecuter {
 protected:
//    GLuint program_id_shadow_; // ID of shadow program

    GLuint depth_bias_id_shadow_; // ID of depth projection matrix for indexed meshes
    // ------ the below are added to the class ------
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
    NonInstancedIMeshShadow(shader::Program shadow_program);

    virtual ~NonInstancedIMeshShadow() {}

    virtual void reload_program() override;

    /**
     * @brief adds a non-indexed mesh so it will cast a shadow
     *
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<T> mesh);

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

    void setup_shadow() const;
};

template <data_structures::NonInstancedIMeshGPUDataType T>
NonInstancedIMeshShadow<T>::NonInstancedIMeshShadow(shader::Program shader_program) :
    OpenGLProgramExecuter(shader_program) {
    reload_program();
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void NonInstancedIMeshShadow<T>::reload_program() {
    depth_bias_id_shadow_ = get_uniform("depthMVP");
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshShadow<T>::add_mesh(std::shared_ptr<T> mesh) {
    meshes_.push_back(std::move(mesh));
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshShadow<T>::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = light_direction;
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshShadow<T>::set_depth_projection_matrix(
    glm::mat4 depth_projection_matrix
) {
    depth_projection_matrix_ = depth_projection_matrix;
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshShadow<T>::load_vertex_buffer(std::shared_ptr<T> mesh) const {
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
NonInstancedIMeshShadow<T>::setup_shadow() const {
    // Cull back-facing triangles -> draw only front-facing triangles
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    // Clear the screen
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(get_program_ID());

    // matrix to calculate the length of a light ray in model space
    glm::mat4 depthMVP = depth_projection_matrix_ * depth_view_matrix_;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform (Model View Projection)
    glUniformMatrix4fv(depth_bias_id_shadow_, 1, GL_FALSE, &depthMVP[0][0]);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshShadow<T>::render_shadow_map(
    screen_size_t shadow_width_, screen_size_t shadow_height_, GLuint frame_buffer_name_
) const {
    gui::FrameBufferHandler::instance().bind_fbo(frame_buffer_name_);
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
