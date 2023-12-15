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
#include "../data_structures/sky_data.hpp"
#include "../data_structures/screen_data.hpp"
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
    data_structures::SkyData star_data_; // star data
    data_structures::ScreenData screen_data_; // screen data
    scene::Environment_Cycle& environment_; // light environment

    GLuint star_programID_; // ID of star program
    GLuint sun_programID_;  // ID of sun program
    GLuint sky_programID_;  // ID of sky program

    GLuint star_view_projection_ID_;     // ID of world to camera space transform matrix
    GLuint pixel_matrix_ID_;             // ID of view to pixel space matrix
    GLuint star_rotation_ID_;            // ID of Sky matrix (rotates stars)

    GLuint sun_view_projection_ID_; // ID of world to camera space transform matrix
    GLuint sun_pixel_matrix_ID_;    // ID of view to pixel space matrix

    GLuint sun_sky_position_ID_;
    GLuint sun_sunlight_color_ID_;

    GLuint sky_view_projection_ID_; // ID of camera space to world space transform matrix
    GLuint sky_pixel_matrix_ID_;    // ID of view to pixel space matrix

    GLuint sky_sky_position_ID_;
    GLuint sky_sunlight_color_ID_;


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
