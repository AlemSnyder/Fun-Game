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
 * @file render.hpp
 *
 * @brief Defines MainRender class.
 *
 * @ingroup GUI
 *
 */
#pragma once

#include "../meshloader.hpp"
#include "../shader.hpp"

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
 * @details QuadRenderer renders the meshes given to it to the screen.
 * this class handles the light direction, applied the meshes, and loading
 * shaders.
 *
 */
class QuadRenderer {
 private:
    GLuint programID_; // ID of non-indexed mesh Program
    GLuint quad_vertexbuffer;
    GLuint texID;

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    QuadRenderer(ShaderHandeler shader_handeler = ShaderHandeler());

    ~QuadRenderer();

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     */
    void render(
        int height, int width, GLuint window_render_texture, GLuint frame_buffer = 0
    ) const;
};

} // namespace render

} // namespace gui
