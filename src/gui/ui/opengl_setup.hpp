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
 * @file opengl_setup.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines setup_opengl function
 *
 * @ingroup GUI
 *
 */

#pragma once

#include "../../logging.hpp"
#include "../gui_logging.hpp"
#include "../../types.hpp"

#include <optional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

[[nodiscard]] std::optional<GLFWwindow*> setup_opengl(screen_size_t width, screen_size_t height);

int setup_opengl_logging();

} // namespace gui
