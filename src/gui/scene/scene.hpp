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

#include "../render/data_structures/frame_buffer_multisample.hpp"
#include "../render/data_structures/shadow_map.hpp"
#include "../render/graphics_shaders/gui_render_types.hpp"
#include "../render/graphics_shaders/instanced_i_mesh_renderer.hpp"
#include "../render/graphics_shaders/non_instanced_i_mesh_renderer.hpp"
#include "../render/graphics_shaders/quad_renderer_multisample.hpp"
#include "../render/graphics_shaders/sky.hpp"

#include <GLFW/glfw3.h>

#pragma once

#define SAMPLES 4

namespace gui {

class Scene {
 private:
    data_structures::FrameBufferMultisample frame_buffer_multisample_;
    data_structures::ShadowMap shadow_map_;

    // background
    render::SkyRenderer sky_renderer_;

    // "mid" ground
    std::vector<std::shared_ptr<render_to::FrameBuffer>> mid_ground_frame_buffer_;
    std::vector<std::shared_ptr<render_to::FrameBufferMultisample>>
        mid_ground_frame_buffer_multisample_;
    std::vector<std::shared_ptr<render_to::ShadowMap>> mid_ground_shadow_;

    // foreground, maybe

    // other
    render::QuadRendererMultisample quad_renderer_multisample_;

 public:
    inline Scene(
        screen_size_t window_width, screen_size_t window_height,
        uint32_t shadow_map_width_height
    ) :
        frame_buffer_multisample_(window_width, window_height, SAMPLES),
        shadow_map_(shadow_map_width_height, shadow_map_width_height), sky_renderer_(),
        quad_renderer_multisample_() {}

    inline GLuint
    get_scene() {
        return frame_buffer_multisample_.get_single_sample_texture();
    }

    inline GLuint
    get_depth_texture() {
        return shadow_map_.get_depth_texture();
    }

    inline uint32_t
    get_shadow_width() {
        return shadow_map_.get_shadow_width();
    }

    inline uint32_t
    get_shadow_height() {
        return shadow_map_.get_shadow_height();
    }

    void update(GLFWwindow* window);

    // model attach

    inline void
    shadow_attach(const std::shared_ptr<render_to::ShadowMap>& shadow) {
        mid_ground_shadow_.push_back(shadow);
    }

    inline void
    frame_buffer_attach(const std::shared_ptr<render_to::FrameBuffer>& render) {
        mid_ground_frame_buffer_.push_back(render);
    }

    inline void
    frame_buffer_multisample_attach(
        const std::shared_ptr<render_to::FrameBufferMultisample>& render
    ) {
        mid_ground_frame_buffer_multisample_.push_back(render);
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
};

} // namespace gui
