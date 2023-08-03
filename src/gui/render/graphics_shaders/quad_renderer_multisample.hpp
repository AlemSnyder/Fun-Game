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

#include "../../meshloader.hpp"
#include "quad_renderer.hpp"
#include "../../shader.hpp"
#include "gui_render_types.hpp"

#include "../../../types.hpp"

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
    GLuint width_id_;       // uniform ID for width of reading texture
    GLuint height_id_;      // uniform ID for height or reading texture
    GLuint tex_samples_id_; // uniform ID for number pixel samples

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    explicit QuadRendererMultisample(ShaderHandler shader_handler = ShaderHandler());

    inline ~QuadRendererMultisample() {
        glDeleteBuffers(1, &quad_vertexbuffer_);
        glDeleteProgram(program_id_);
    }

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     */
    void render(
        screen_size_t width, screen_size_t height, uint32_t samples, GLuint window_render_texture,
        GLuint frame_buffer = 0
    ) const;

};

} // namespace render

} // namespace gui
