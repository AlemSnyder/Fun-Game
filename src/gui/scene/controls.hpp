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
 * @file controls.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines functions to get game position from inputs
 *
 * @ingroup CONTROLS
 *
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace controls {

/**
 * @brief Computes the view, and projection matrix using the size of the given
 * window
 *
 * @param window The current OpenGL window
 */
void computeMatricesFromInputs(GLFWwindow* window);

/**
 * @brief Get the view matrix
 *
 * @return glm::mat4 projection of world space to camera space
 */
glm::mat4 get_view_matrix();

/**
 * @brief Get the projection matrix
 *
 * @return glm::mat4 homographic projection of world onto view depending on the
 * FOV
 */
glm::mat4 get_projection_matrix();

/**
 * @brief Get the position vector
 * 
 * @return glm::vec3 camera position in world
*/
glm::vec3 get_position_vector();

/**
 * @brief True if the shadow map should be shown
 *
 * @param window current window
 * @return true the shadow map should be shown
 * @return false the shadow map should not be shown
 */
inline bool
show_shadow_map(GLFWwindow* window) {
    return glfwGetKey(window, GLFW_KEY_TAB);
}

} // namespace controls
