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

#include "../data_structures/sky_data.hpp"
#include "../meshloader.hpp"

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
class SkyRenderer {
 private:
    GLuint programID_;                  // ID of Program
    GLuint matrix_view_projection_ID_;  // ID of world to camera space transform matrix
    GLuint pixel_matrix_ID_;            // ID of view to pixel space matrix
    data_structures::SkyData sky_data_; // star data

 public:
    /**
     * @brief Construct a new Main Renderer object
     *
     */
    SkyRenderer();

    ~SkyRenderer();

    /**
     * @brief renders the given meshes
     *
     * @param window the OpenGL window
     */
    void render(GLFWwindow* window, GLuint frame_buffer) const;
};

} // namespace render

} // namespace gui
