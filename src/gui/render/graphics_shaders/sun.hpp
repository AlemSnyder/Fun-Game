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

#include "../data_structures/screen_data.hpp"
#include "../uniform_types.hpp"
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

namespace render {

/**
 * @brief Renders the sky background
 *
 * @details SunRenderer renders the sun. It handles sun position.
 *
 */
class SunRenderer : public render_to::FrameBuffer, public OpenGLProgramExecuter {
 private:
    data_structures::ScreenData screen_data_;            // screen data
    std::shared_ptr<render::LightEnvironment> lighting_; // lighting

    // sun program uniforms
    GLuint view_projection_UID_;  // view projection matrix for sun program
    GLuint pixel_projection_UID_; // ID of view to pixel space matrix
    GLuint sun_position_UID_;
    GLuint sunlight_color_UID_;

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    SunRenderer(
        std::shared_ptr<render::LightEnvironment> lighting,
        shader::Program& shader_program
    );

    virtual void reload_program() override;

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
    render_frame_buffer(screen_size_t width, screen_size_t height, GLuint frame_buffer)
        const override {
        return render(width, height, frame_buffer);
    }
};

} // namespace render

} // namespace gui
