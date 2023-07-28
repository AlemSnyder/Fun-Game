#pragma once

#include "../../logging.hpp"
#include "../gui_logging.hpp"
#include "../../types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

[[nodiscard]] GLFWwindow* setup_opengl(screen_size_t width, screen_size_t height);

int setup_logging();

} // namespace gui
