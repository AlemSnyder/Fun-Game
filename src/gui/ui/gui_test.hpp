#pragma once

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../world.hpp"
#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "opengl_setup.hpp"
// #include "../render/graphics_shaders/shader.hpp"
#include "../render/data_structures/screen_data.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/uniform_types.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <stdio.h>

#include <functional>
#include <memory>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

namespace gui {

int revised_gui_test();

int stars_test();

} // namespace gui
