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
 * @file instanced_int_renderer.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI  MODELS
 *
 */
#pragma once

#include "../../shader.hpp"
#include "../graphics_data/instanced_int.hpp"
#include "gui_render_types.hpp"
#include "individual_int_renderer.hpp"
#include "../../../types.hpp"

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
 * @details MainRenderer renders the meshes given to it to the screen.
 * this class handles the light direction, applied the meshes, and loading
 * shaders.
 *
 */

// template <gui::data_structures::InstancedIntLike T>
template <class T>
class InstancedIntRenderer : public IndividualIntRenderer<T> {
 private:
    // forgot what to put here
 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    InstancedIntRenderer(ShaderHandler shader_handler = ShaderHandler());

    ~InstancedIntRenderer();

    void load_transforms_buffer(std::shared_ptr<T> mesh) const;

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     */
    void render_frame_buffer(GLFWwindow* window, GLuint frame_buffer = 0) const;

    void
    render_frame_buffer_multisample(GLFWwindow* window, GLuint frame_buffer = 0) const;

    void
    render_shadow_map(screen_size_t shadow_width_, screen_size_t shadow_height_, GLuint frame_buffer) const;
};

template <class T>
InstancedIntRenderer<T>::InstancedIntRenderer(ShaderHandler shader_handler) {
    // TODO should pass these paths
    // non-indexed program
    this->programID_render_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "ShadowMappingInstanced.vert",
        files::get_resources_path() / "shaders" / "ShadowMapping.frag"
    );
    // indexed program
    this->programID_shadow_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "DepthRTTInstanced.vert",
        files::get_resources_path() / "shaders" / "DepthRTT.frag"
    );
}

template <class T>
void
InstancedIntRenderer<T>::load_transforms_buffer(std::shared_ptr<T> mesh) const {
    // 4nd attribute buffer : transform
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->get_model_transforms());
    glVertexAttribIPointer(
        3,       // attribute
        3,       // size
        GL_INT,  // type
        0,       // stride
        (void*)0 // array buffer offset
    );
    glVertexAttribDivisor(3, 1);
    // to undo the vertex attribute divisor do
    // glVertexAttribDivisor(3, 0);
}

template <class T>
void
InstancedIntRenderer<T>::render_frame_buffer(GLFWwindow* window, GLuint frame_buffer)
    const {
    // Render to the screen
    gui::FrameBufferHandler::getInstance().bind_fbo(frame_buffer);

    // get he window size
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    IndividualIntRenderer<T>::setup_render();

    for (std::shared_ptr<T> mesh : this->meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        load_vertex_buffer(mesh);
        load_color_buffers(mesh);

        load_transforms_buffer();

        // Draw the triangles !
        glDrawElementsInstanced(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0,                 // element array buffer offset
            mesh->get_num_models()
        );
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
    }
}

template <class T>
void
InstancedIntRenderer<T>::render_frame_buffer_multisample(
    GLFWwindow* window, GLuint frame_buffer
) const {
    render_frame_buffer(window, frame_buffer);
}

template <class T>
void
InstancedIntRenderer<T>::render_shadow_map(
    screen_size_t shadow_width_, screen_size_t shadow_height_, GLuint frame_buffer_name_
) const {
    gui::FrameBufferHandler::getInstance().bind_fbo(frame_buffer_name_);
    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, shadow_width_, shadow_height_);

    IndividualIntRenderer<T>::setup_shadow();

    // draw the indexed meshes
    for (std::shared_ptr<T> mesh : this->meshes_) {
        if (!mesh->do_render()) {
            continue;
        }

        IndividualIntRenderer<T>::load_vertex_buffer(mesh);

        load_transforms_buffer(mesh);

        // Draw the triangles !
        glDrawElementsInstanced(
            GL_TRIANGLES,             // mode
            mesh->get_num_vertices(), // count
            GL_UNSIGNED_SHORT,        // type
            (void*)0,                 // element array buffer offset
            mesh->get_num_models()
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(3);
    }
}

} // namespace models

} // namespace gui
