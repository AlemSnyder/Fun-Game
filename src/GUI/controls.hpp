#ifndef __GUI_CONTROLS_HPP__
#define __GUI_CONTROLS_HPP__

namespace controls{

void computeMatricesFromInputs(GLFWwindow * window);
glm::mat4 get_view_matrix();
glm::mat4 get_projection_matrix();

} // namespace controls

#endif
