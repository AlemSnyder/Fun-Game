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
 * @file quad_renderer_multisample.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI  RENDER
 *
 */
#pragma once

#include "../../../types.hpp"
#include "../../shader.hpp"
#include "gui_render_types.hpp"
#include "quad_renderer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <vector>

namespace gui {

namespace render {

/**
 * @brief Renders the meshes to the screen
 *
 * @details QuadRendererMultisample renders a multisampled meshes to the
 * screen.
 *
 */
class QuadRendererMultisample : public QuadRenderer {
 private:
    GLuint width_UID_;       // uniform ID for width of reading texture
    GLuint height_UID_;      // uniform ID for height or reading texture
    GLuint num_samples_UID_; // uniform ID for number pixel samples

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    explicit QuadRendererMultisample(ShaderHandler shader_handler = ShaderHandler());

    /**
     * @brief renders the given meshes
     *
     * @param screen_size_t window height
     * @param screen_size_t window width
     * @param GLuint texture to render from
     * @param GLuint framebuffer to render to
     */
    void inline render(
        screen_size_t width, screen_size_t height, uint32_t samples,
        GLuint window_render_texture, GLuint frame_buffer = 0
    ) const {
        QuadRenderer::setup(width, height, window_render_texture, frame_buffer);
        glUniform1ui(width_UID_, width);
        glUniform1ui(height_UID_, height);
        glUniform1ui(num_samples_UID_, samples);
        QuadRenderer::draw();
    }
};

} // namespace render

} // namespace gui
