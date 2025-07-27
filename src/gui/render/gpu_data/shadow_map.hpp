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
 * @file shadow_map.hpp
 *
 * @brief Defines ShadowMap class.
 *
 * @ingroup GUI
 *
 */
#pragma once

#include "types.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace gui {

namespace gpu_data {

/**
 * @brief Contains a shadow depth buffer that can be rendered to.
 *
 * @details ShadowMap holds the depth texture. When added to a scene object
 * shadows are cast to this depth texture, and used when rendering the scene.
 */
class ShadowMap {
 private:
    GLuint depth_texture_id_; // ID of depth texture
    GLuint frame_buffer_id_;  // ID of frame buffer
    // ------ the below are added to the class ------
    glm::vec3 light_direction_;         // direction of sunlight
    screen_size_t shadow_width_;        // width of depth texture
    screen_size_t shadow_height_;       // height of depth texture
    glm::mat4 depth_projection_matrix_; // projection matrix of the light source
    glm::mat4 depth_view_matrix_;       // convert a point in world space
                                        // to depth in light direction
    glm::mat4 inverse_view_projection_;

 public:
    /**
     * @brief Construct a new Shadow Map object
     *
     * @param w the width of the area hit by light
     * @param h the height of the area hit by light
     */
    ShadowMap(screen_size_t w, screen_size_t h);

    ~ShadowMap() {
        glDeleteFramebuffers(1, &frame_buffer_id_);
        glDeleteTextures(1, &depth_texture_id_);
    }

    /**
     * @brief Get the depth texture ID
     *
     * @return GLuint& reference to depth texture ID
     */
    [[nodiscard]] inline GLuint
    get_depth_texture() const {
        return depth_texture_id_;
    }

    /**
     * @brief Bind texture to given texture index
     *
     * @param texture_index to bind texture to
     */
    inline void
    bind(uint texture_index) const {
        glActiveTexture(GL_TEXTURE0 + texture_index);
        glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
    }

    /**
     * @brief Get the frame buffer ID
     *
     * @return GLuint& reference to frame buffer ID
     */
    [[nodiscard]] inline GLuint&
    get_frame_buffer_id() {
        return frame_buffer_id_;
    }

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

    void set_inverse_view_projection(glm::mat4 inverse_view_projection);

    /**
     * @brief Get the light direction used for this shadow map
     *
     * @return glm::vec3 the light direction
     */
    [[nodiscard]] inline const glm::vec3&
    get_light_direction() const {
        return light_direction_;
    }

    /**
     * @brief Return the depth projection from world space to shadow depth space
     *
     * @return glm::mat4 the depth projection matrix
     */
    [[nodiscard]] inline const glm::mat4&
    get_depth_projection_matrix() const {
        return depth_projection_matrix_;
    }

    /**
     * @brief
     * @return
     */
    [[nodiscard]] inline const glm::mat4&
    get_depth_view_matrix() const {
        return depth_view_matrix_;
    }

    /**
     * @brief Get shadow width in pixels
     *
     * @return screen_size_t width of shadow map in pixels
     */
    [[nodiscard]] inline screen_size_t
    get_shadow_width() const {
        return shadow_width_;
    }

    /**
     * @brief Get shadow height in pixels
     *
     * @return screen_size_t height of shadow map in pixels
     */
    [[nodiscard]] inline screen_size_t
    get_shadow_height() const {
        return shadow_height_;
    }

    /**
     * @brief Updates the depth projection matrix from saved values.
     *
     */

    void update_depth_projection_matrix();
};

} // namespace gpu_data

} // namespace gui
