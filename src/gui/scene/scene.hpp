// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file scene.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Scene class
 *
 * @ingroup GUI
 *
 */
#pragma once

#include "../render/data_structures/frame_buffer_multisample.hpp"
#include "../render/data_structures/shadow_map.hpp"
#include "../render/graphics_shaders/gui_render_types.hpp"
#include "../render/graphics_shaders/instanced_i_mesh_renderer.hpp"
#include "../render/graphics_shaders/non_instanced_i_mesh_renderer.hpp"
#include "../render/graphics_shaders/quad_renderer_multisample.hpp"
#include "../render/graphics_shaders/sky.hpp"

#include <GLFW/glfw3.h>


#define SAMPLES 4

namespace gui {

/**
 * @brief Scene object to handel 3D rendering.
 */
class Scene {
 private:
    data_structures::FrameBufferMultisample frame_buffer_multisample_;
    data_structures::ShadowMap shadow_map_;

    // background
//    render::SkyRenderer sky_renderer_;
    std::vector<std::shared_ptr<render_to::FrameBufferMultisample>>
        background_frame_buffer_;

    // "mid" ground
    std::vector<std::shared_ptr<render_to::FrameBufferMultisample>> mid_ground_frame_buffer_;
    std::vector<std::shared_ptr<render_to::ShadowMap>> mid_ground_shadow_;

    // foreground, maybe

    // other

 public:
    /**
     * @brief Scene constructor
     *
     * @param window_width with of scene
     * @param window_height height of scene
     * @param shadow_map_width_height side length of shadow map
     */
    inline Scene(
        screen_size_t window_width, screen_size_t window_height,
        uint32_t shadow_map_width_height
    ) :
        frame_buffer_multisample_(window_width, window_height, SAMPLES),
        shadow_map_(shadow_map_width_height, shadow_map_width_height) {}

    /**
     * @brief Get scene shadow mat depth texture id
     *
     * @return GLuint shadow mat depth texture id
     */
    inline GLuint
    get_depth_texture() {
        return shadow_map_.get_depth_texture();
    }

    /**
     * @brief Get shadow map width
     *
     * @return screen_size_t width
     */
    inline screen_size_t
    get_shadow_width() {
        return shadow_map_.get_shadow_width();
    }

    /**
     * @brief Get shadow map height
     *
     * @return screen_size_t height
     */
    inline screen_size_t
    get_shadow_height() {
        return shadow_map_.get_shadow_height();
    }

    inline GLuint
    get_frame_buffer_id() {
        return frame_buffer_multisample_.get_frame_buffer_id();
    }

    /**
     * @brief Get scene frame buffer multisample id
     */
    void update(screen_size_t width, screen_size_t height);

    // model attach

    /**
     * @brief Attach shadow renderer.
     *
     * @param render object that can render to a shadow framebuffer.
     */
    inline void
    shadow_attach(const std::shared_ptr<render_to::ShadowMap>& shadow) {
        mid_ground_shadow_.push_back(shadow);
    }

    /**
     * @brief Attach renderer.
     *
     * @param render object that can render to a framebuffer.
     */
    inline void
    add_mid_ground_renderer(const std::shared_ptr<render_to::FrameBufferMultisample>& render) {
        mid_ground_frame_buffer_.push_back(render);
    }

    /**
     * @brief Set the light direction vector
     *
     * @param light_direction the direction of the light
     */
    inline void
    set_shadow_light_direction(glm::vec3 light_direction) {
        shadow_map_.set_light_direction(light_direction);
    }

    /**
     * @brief Set the depth projection matrix
     *
     * @param depth_projection_matrix the projection matrix
     */
    inline void
    set_shadow_depth_projection_matrix(glm::mat4 depth_projection_matrix) {
        shadow_map_.set_depth_projection_matrix(depth_projection_matrix);
    }

    /**
     * @brief Copy the framebuffer to the screen.
     * 
     * @details Only a screen-sized portion of this framebuffer is rendered to.
     * In this call that portion is rendered to the screen.
    */
    inline void
    copy_to_window(screen_size_t width, screen_size_t height) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, get_frame_buffer_id());
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // window framebuffer
        glBlitFramebuffer(
            0, 0, width, height, 0, 0, width, height, // region of framebuffer
            GL_COLOR_BUFFER_BIT, GL_NEAREST // copy the color
        );
    }
};

} // namespace gui
