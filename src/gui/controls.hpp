#pragma once

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace controls{

/**
 * @brief Computes the view, and projection matrix using the size of the given
 * window
 * 
 * @param window The current OpenGL window (duh)
 */
void computeMatricesFromInputs(GLFWwindow * window);

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

} // namespace controls

