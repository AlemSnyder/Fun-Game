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
 * @file quad_renderer.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI  RENDER
 *
 */
#pragma once

#include "../../../types.hpp"
#include "opengl_program.hpp"
#include "shader.hpp"
#include "gui_render_types.hpp"

#include <GL/glew.h>


namespace gui {

namespace render {

/**
 * @brief Renders the meshes to the screen
 *
 * @details QuadRenderer renders the meshes given to it to the screen.
 * this class handles the light direction, applied the meshes, and loading
 * shaders.
 *
 */
class QuadRenderer : public OpenGLProgramExecuter {
 protected:
    GLuint quad_vertexbuffer_;
    GLuint texID;

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     * @param ShaderHandler
     */
    QuadRenderer(shader::Program& shader_program);

    virtual ~QuadRenderer();

    /**
     * @brief renders the given meshes
     *
     * @param screen_size_t window height
     * @param screen_size_t window width
     * @param GLuint texture to render from
     * @param GLuint framebuffer to render to
     */
    void render(
        screen_size_t height, screen_size_t width, GLuint window_render_texture,
        GLuint frame_buffer = 0
    ) const;

 protected:
    /**
     * @brief Helper function bind texture and set view pot size.
     *
     * @param width framebuffer width
     * @param height framebuffer height
     * @param window_render_texture texture to be rendered
     * @param frame_buffer frame buffer id
     */
    void setup(
        screen_size_t width, screen_size_t height, GLuint window_render_texture,
        GLuint frame_buffer
    ) const;

    inline void
    reload_program() override {
        texID = glGetUniformLocation(get_program_ID(), "texture_id");
    }

    /**
     * @brief Helper function to call program.
     */
    void draw() const;
};

} // namespace render

} // namespace gui
