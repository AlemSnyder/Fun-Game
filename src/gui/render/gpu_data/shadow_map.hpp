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

#include "frame_buffer_color.hpp"
#include "frame_buffer_shadow_map.hpp"
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
    // GLuint depth_texture_id_; // ID of depth texture
    // GLuint frame_buffer_id_;  // ID of frame buffer
    //  ------ the below are added to the class ------
    glm::vec3 light_direction_; // direction of sunlight
    // screen_size_t shadow_width_;        // width of depth texture
    // screen_size_t shadow_height_;       // height of depth texture

    FrameBufferShadowMap front_faces_shadow;
    FrameBufferShadowMap back_faces_shadow;

    FrameBufferColor avg_shadow;

    glm::mat4 depth_projection_matrix_; // projection matrix of the light source
    glm::mat4 depth_view_matrix_;       // convert a point in world space
                                        // to depth in light direction

 public:
    /**
     * @brief Construct a new Shadow Map object
     *
     * @param w the width of the area hit by light
     * @param h the height of the area hit by light
     */
    ShadowMap(screen_size_t w, screen_size_t h) :
        front_faces_shadow(w, h), back_faces_shadow(w, h), avg_shadow(w, h){};

    /**
     * @brief Get the back face depth texture ID
     *
     * @return GLuint depth texture ID
     */
    [[nodiscard]] inline GLuint
    get_back_texture() const {
        return back_faces_shadow.get_depth_texture();
    }

    /**
     * @brief Get the front face depth texture ID
     *
     * @return GLuint depth texture ID
     */
    [[nodiscard]] inline GLuint
    get_front_texture() const {
        return front_faces_shadow.get_depth_texture();
    }

    /**
     * @brief Get the final shadow depth texture ID
     *
     * @return GLuint depth texture ID
     */
    [[nodiscard]] inline GLuint
    get_final_texture() const {
        return avg_shadow.get_texture_name();
    }

    inline void
    bind(uint texture_index) const {
        glActiveTexture(GL_TEXTURE0 + texture_index);
        glBindTexture(GL_TEXTURE_2D, get_final_texture());
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

    [[nodiscard]] inline const glm::vec3&
    get_light_direction() const {
        return light_direction_;
    }

    [[nodiscard]] inline const glm::mat4&
    get_depth_projection_matrix() const {
        return depth_projection_matrix_;
    }

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
        return front_faces_shadow.get_width();
    }

    /**
     * @brief Get shadow height in pixels
     *
     * @return screen_size_t height of shadow map in pixels
     */
    [[nodiscard]] inline screen_size_t
    get_shadow_height() const {
        return front_faces_shadow.get_height();
    }

    /**
     * @brief Update the shadow map
     *
     * @details Changes the shadow map depth matrix. This allows the shadow map
     * to move with the camera.
     */
    void update();

    /**
     * @brief Update the shadow map
     *
     * @details Set the light direction then update the shadow map.
     */
    void
    update(glm::vec3 light_direction) {
        set_light_direction(light_direction);
        update();
    };

    [[nodiscard]] inline GLuint
    get_front_face_framebuffer_id() {
        return front_faces_shadow.get_frame_buffer_id();
    }

    [[nodiscard]] inline GLuint
    get_back_face_framebuffer_id() {
        return back_faces_shadow.get_frame_buffer_id();
    }

    [[nodiscard]] inline GLuint
    get_final_framebuffer_id() {
        return avg_shadow.get_frame_buffer_id();
    }
};

} // namespace gpu_data

} // namespace gui
