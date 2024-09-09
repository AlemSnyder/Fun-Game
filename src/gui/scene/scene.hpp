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

#include "gui/render/gpu_data/frame_buffer_multisample.hpp"
#include "gui/render/gpu_data/shadow_map.hpp"
#include "gui/render/graphics_shaders/render_types.hpp"
#include "gui/render/structures/screen_data.hpp"
#include "helio.hpp"

#include <GLFW/glfw3.h>

#define SAMPLES 4

namespace gui {

/**
 * @brief Scene object to handel 3D rendering.
 */
class Scene {
 private:
    gpu_data::FrameBufferMultisample frame_buffer_multisample_;
    std::shared_ptr<scene::Helio> environment_;
    gpu_data::ShadowMap shadow_map_;

    gpu_data::ScreenData screen_data_;

    // background
    std::vector<std::shared_ptr<render_to::FrameBuffer>> background_frame_buffer_;

    // "mid" ground
    std::vector<std::shared_ptr<render_to::FrameBuffer>> mid_ground_frame_buffer_;

    // foreground
    std::vector<std::shared_ptr<render_to::FrameBuffer>> foreground_frame_buffer_;

    // shadow
    std::vector<std::shared_ptr<render_to::FrameBuffer>> mid_ground_shadow_;

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
        environment_(std::make_shared<scene::Helio>(.3, 5, 60, .3)),
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

    /**
     * @brief Get shadow map
     *
     * @return ShadowMap& shadow map used by this scene.
     */
    const gpu_data::ShadowMap&
    get_shadow_map() const {
        return shadow_map_;
    }

    /**
     * @brief Get the screen data
     *
     * @details Screen data is four vertices than can be mapped to the four corners of
     * the screen.
     */
    const auto*
    get_screen_data() const {
        return &screen_data_;
    }

    /**
     * @brief Get framebuffer id being rendered to.
     *
     * @return GLuint framebuffer id.
     */
    inline GLuint
    get_frame_buffer_id() {
        return frame_buffer_multisample_.get_frame_buffer_id();
    }

    /**
     * @brief Get scene frame buffer multisample id
     */
    void update(screen_size_t width, screen_size_t height);

    /**
     * @brief Attach shadow renderer.
     *
     * @param render object that can render to a shadow framebuffer.
     */
    inline void
    shadow_attach(const std::shared_ptr<render_to::FrameBuffer> shadow) {
        mid_ground_shadow_.push_back(shadow);
    }

    /**
     * @brief Attach renderer.
     *
     * @param render object that can render to a framebuffer.
     */
    inline void
    add_background_ground_renderer(const std::shared_ptr<render_to::FrameBuffer> render
    ) {
        background_frame_buffer_.push_back(render);
    }

    /**
     * @brief Attach renderer.
     *
     * @param render object that can render to a framebuffer.
     */
    inline void
    add_mid_ground_renderer(const std::shared_ptr<render_to::FrameBuffer>& render) {
        mid_ground_frame_buffer_.push_back(render);
    }

    /**
     * @brief Attach renderer.
     *
     * @param render object that can render to a framebuffer.
     */
    inline void
    add_foreground_renderer(const std::shared_ptr<render_to::FrameBuffer>& render) {
        foreground_frame_buffer_.push_back(render);
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
     * @brief Get the light direction vector
     *
     * @return glm::vec3 the direction of the light.
     */
    inline glm::vec3
    get_light_direction() {
        return shadow_map_.get_light_direction();
    }

    inline const std::shared_ptr<scene::Helio>
    get_lighting_environment() const {
        return environment_;
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
     * @brief Update light using Heliocentric model.
     *
     * @details This function also updates the light color and intensity
     * depending on the sun position.
     */
    inline void
    update_light_direction() {
        environment_->update();
        glm::vec3 light_direction =
            static_cast<float>(120.0) * environment_->get_light_direction();

        set_shadow_light_direction(light_direction);
    }

    /**
     * @brief Update lighting with given light direction
     *
     * @details This function also updates the light color and intensity
     * depending on the sun position.
     */
    inline void
    manual_update_light_direction(glm::vec3 light_direction_in) {
        glm::vec3 light_direction =
            static_cast<float>(120.0) * glm::normalize(light_direction_in);

        environment_->update_sunlight_color(light_direction);

        set_shadow_light_direction(light_direction);
    }

    /*
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
            GL_COLOR_BUFFER_BIT, GL_NEAREST           // copy the color
        );
    }
};

} // namespace gui
