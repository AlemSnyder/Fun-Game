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
 * @file sky.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI  RENDER
 *
 */
#pragma once

#include "../../scene/environment.hpp"
#include "../../shader.hpp"
#include "../data_structures/screen_data.hpp"
#include "../data_structures/sky_data.hpp"
#include "gui_render_types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace gui {

namespace render {

/**
 * @brief Renders the meshes to the screen
 *
 * @details SkyRenderer renders the meshes given to it to the screen.
 * this class handles the light direction, applied the meshes, and loading
 * shaders.
 *
 */
class SkyRenderer :
    public render_to::FrameBufferMultisample,
    public render_to::FrameBuffer {
 private:
    data_structures::SkyData star_data_;      // star data
    data_structures::ScreenData screen_data_; // screen data
    scene::Environment_Cycle& environment_;   // light environment

    GLuint star_PID_; // ID of star program
    GLuint sun_PID_;  // ID of sun program
    GLuint sky_PID_;  // ID of sky program

    // star program uniforms
    GLuint view_projection_star_UID_;  // view projection matrix for star program
    GLuint pixel_projection_star_UID_; // ID of view to pixel space matrix
    GLuint star_rotation_star_UID_;    // ID of Sky matrix (rotates stars)
    GLuint sun_position_star_UID_;     // ID of Sun position for star renderer

    // sun program uniforms
    GLuint view_projection_sun_UID_;  // view projection matrix for sun program
    GLuint pixel_projection_sun_UID_; // ID of view to pixel space matrix
    GLuint sun_position_sun_UID_;
    GLuint sunlight_color_sun_UID_;

    // sky program uniforms
    GLuint view_projection_sky_UID_;  // view projection matrix for sky program
    GLuint pixel_projection_sky_UID_; // ID of view to pixel space matrix
    GLuint sun_position_sky_UID_;     // sun position/light direction vector
    GLuint sunlight_color_sky_UID_;   // sunlight color vector

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    explicit SkyRenderer(
        scene::Environment_Cycle& environment,
        ShaderHandler shader_handler = ShaderHandler()
    );

    ~SkyRenderer();

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     */
    void render(screen_size_t width, screen_size_t height, GLuint frame_buffer) const;

    /**
     * @brief Renders sky to given framebuffer
     *
     * @param window OpenGL window
     * @param frame_buffer frame buffer id
     */
    void
    render_frame_buffer_multisample(
        screen_size_t width, screen_size_t height, GLuint frame_buffer
    ) const override {
        return render(width, height, frame_buffer);
    }

    /**
     * @brief Renders sky to given framebuffer
     *
     * @param window OpenGL window
     * @param frame_buffer frame buffer id
     */
    void
    render_frame_buffer(screen_size_t width, screen_size_t height, GLuint frame_buffer)
        const override {
        return render(width, height, frame_buffer);
    }
};

} // namespace render

} // namespace gui
