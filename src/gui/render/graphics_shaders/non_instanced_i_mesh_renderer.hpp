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
#include "../../shader.hpp"
#include "../graphics_data/non_instanced_i_mesh.hpp"
#include "gui_render_types.hpp"
#include "../uniform_types.hpp"

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
    GLuint render_PID_; // ID of render program
    GLuint shadow_PID_; // ID of shadow program

    // ----- uniforms ---------
    // Render Program uniforms
    GLuint transform_matrix_render_UID_; // uniform ID of transform matrix
    GLuint view_matrix_render_UID_; // ID of view projection matrix for indexed meshes
    GLuint depth_bias_render_UID_;  // ID of depth projection matrix for indexed meshes
    GLuint shadow_texture_render_UID_;  // ID of the shadow map for indexed meshes
    GLuint material_color_texture_UID_;   // ID of the color map for indexed meshes
    GLuint light_direction_render_UID_; // ID of the light direction uniform for indexed
    GLuint diffuse_light_color_render_UID_; // ID of the diffuse light color
    GLuint direct_light_color_render_UID_; // ID of light color uniform
    // Shadow program uniforms
    GLuint depth_bias_shadow_UID_; // ID of depth projection matrix for indexed meshes

    // TODO change to interface
    const std::shared_ptr<render::LightEnvironment> lighting_;
    // ------ the below are added to the class ------
    const data_structures::ShadowMap* shadow_map_;

    std::vector<std::shared_ptr<T>> meshes_;

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     * @param ShaderHandler
     */
    explicit NonInstancedIMeshRenderer(
        const std::shared_ptr<render::LightEnvironment> lighting,
        ShaderHandler shader_handler = ShaderHandler()
    );

    virtual ~NonInstancedIMeshRenderer() {}

    /**
     * @brief Add a non-indexed mesh to shadow set of shadow casters.
     *
     * @param mesh the mesh to add
     */
    void add_mesh(std::shared_ptr<T> mesh);

    virtual void set_shadow_map(const data_structures::ShadowMap* shadow_map) override;

    /**
     * @brief Render the previously given meshes.
     *
     * @param width width of frame buffer
     * @param height height of frame buffer
     * @param GLuint frame buffer to render to
     */
    void render_frame_buffer(
        screen_size_t width, screen_size_t height, GLuint frame_buffer = 0
    ) const override;

    /**
     * @brief Renders the previously given meshes to multisample frame buffer.
     *
     * @param width width of frame buffer
     * @param height height of frame buffer
     * @param GLuint frame buffer to render to
     */
    void render_frame_buffer_multisample(
        screen_size_t width, screen_size_t height, GLuint frame_buffer = 0
    ) const override;

    /**
     * @brief Renders the shadow texture.
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
NonInstancedIMeshRenderer<T>::NonInstancedIMeshRenderer(
    const std::shared_ptr<render::LightEnvironment> lighting, ShaderHandler shader_handler
) :
    lighting_(lighting) {
    // non-indexed program
    render_PID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.vert",
        files::get_resources_path() / "shaders" / "scene" / "ShadowMapping.frag"
    );
    // indexed program
    shadow_PID_ = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.vert",
        files::get_resources_path() / "shaders" / "scene" / "DepthRTT.frag"
    );
    // ------ indexed program ------
    transform_matrix_render_UID_ = glGetUniformLocation(render_PID_, "MVP");
    view_matrix_render_UID_ = glGetUniformLocation(render_PID_, "view_matrix");
    depth_bias_render_UID_ = glGetUniformLocation(render_PID_, "depth_MVP");
    shadow_texture_render_UID_ = glGetUniformLocation(render_PID_, "shadow_texture");
    material_color_texture_UID_ = glGetUniformLocation(render_PID_, "material_color_texture");
    light_direction_render_UID_ =
        glGetUniformLocation(render_PID_, "light_direction");
    direct_light_color_render_UID_ = glGetUniformLocation(render_PID_, "direct_light_color");

    diffuse_light_color_render_UID_ =
        glGetUniformLocation(render_PID_, "diffuse_light_color");

    depth_bias_shadow_UID_ = glGetUniformLocation(shadow_PID_, "depth_MVP");

}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::add_mesh(std::shared_ptr<T> mesh) {
    meshes_.push_back(std::move(mesh));
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::set_shadow_map(
    const data_structures::ShadowMap* shadow_map
) {
    shadow_map_ = shadow_map;
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
    glUniform1i(material_color_texture_UID_, 2);
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

    const glm::mat4& depth_projection_matrix =
        shadow_map_->get_depth_projection_matrix();
    const glm::mat4& depth_view_matrix = shadow_map_->get_depth_view_matrix();

    // matrix to calculate the length of a light ray in model space
    glm::mat4 depthMVP = depth_projection_matrix * depth_view_matrix;

    // Compute the MVP matrix from keyboard and mouse input
    // controls::computeMatricesFromInputs(window);
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 view_matrix = controls::get_view_matrix();
    // glm::mat4 ModelMatrix = glm::mat4(1.0);
    glm::mat4 MVP = projection_matrix * view_matrix; // Model View Projection

    // Shadow bias matrix of-sets the shadows
    // converts -1,1 to 0,1 to go from opengl render region to texture
    glm::mat4 bias_matrix(
        0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0
    );

    glm::mat4 depth_bias_MVP = bias_matrix * depthMVP;

    /******************
     * Render Program *
     ******************/

    // Use our shader
    glUseProgram(render_PID_);

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(transform_matrix_render_UID_, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(view_matrix_render_UID_, 1, GL_FALSE, &view_matrix[0][0]);
    glUniformMatrix4fv(depth_bias_render_UID_, 1, GL_FALSE, &depth_bias_MVP[0][0]);

    const glm::vec3 sunlight_color = lighting_->get_specular_light();

    glUniform3f(
        direct_light_color_render_UID_, sunlight_color.r, sunlight_color.g, sunlight_color.b
    );

    const glm::vec3 diffuse_light_color = lighting_->get_diffuse_light();

    glUniform3f(
        diffuse_light_color_render_UID_, diffuse_light_color.r, diffuse_light_color.g,
        diffuse_light_color.b
    );

    const glm::vec3& light_direction = shadow_map_->get_light_direction();

    // set the light direction uniform
    glUniform3f(
        light_direction_render_UID_, light_direction.x, light_direction.y,
        light_direction.z
    );

    GLuint depth_texture = shadow_map_->get_depth_texture();

    // Bind Shadow Texture to Texture Unit 1
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glUniform1i(shadow_texture_render_UID_, 1);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::setup_shadow() const {
    // Cull back-facing triangles -> draw only front-facing triangles
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    /******************
     * Shadow Program *
     ******************/

    glUseProgram(shadow_PID_);

    const glm::mat4& depth_projection_matrix =
        shadow_map_->get_depth_projection_matrix();
    const glm::mat4& depth_view_matrix = shadow_map_->get_depth_view_matrix();

    // matrix to calculate the length of a light ray in model space
    glm::mat4 depthMVP = depth_projection_matrix * depth_view_matrix;

    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform (Model View Projection)
    glUniformMatrix4fv(depth_bias_shadow_UID_, 1, GL_FALSE, &depthMVP[0][0]);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::render_frame_buffer_multisample(
    screen_size_t width, screen_size_t height, GLuint frame_buffer
) const {
    render_frame_buffer(width, height, frame_buffer);
}

template <data_structures::NonInstancedIMeshGPUDataType T>
void
NonInstancedIMeshRenderer<T>::render_frame_buffer(
    screen_size_t width, screen_size_t height, GLuint frame_buffer
) const {
    // Render to the screen
    gui::FrameBufferHandler::instance().bind_fbo(frame_buffer);

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
