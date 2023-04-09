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
 * @param window The current OpenGL window (duh)
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

inline bool
show_shadow_map(GLFWwindow* window) {
    return glfwGetKey(window, GLFW_KEY_TAB);
}
} // namespace controls
