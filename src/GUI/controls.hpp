#ifndef __GUI_CONTROLS_HPP__
#define __GUI_CONTROLS_HPP__

namespace controls{

/**
 * @brief Computes the view, and projection matrix using the size of the given
 * window
 * 
 * @param window 
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
 * @return glm::mat4 homographic projection depending on the FOV
 */
glm::mat4 get_projection_matrix();

} // namespace controls

#endif
