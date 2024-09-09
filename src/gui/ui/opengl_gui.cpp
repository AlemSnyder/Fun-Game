#include "opengl_gui.hpp"

#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"
#include "logging.hpp"
#include "opengl_setup.hpp"
#include "scene_setup.hpp"
#include "util/files.hpp"
#include "world/climate.hpp"
#include "world/entity/mesh.hpp"
#include "world/world.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <vector>

namespace gui {

int
opengl_entry(GLFWwindow* window, world::World& world, world::Climate& climate) {
    screen_size_t window_width;
    screen_size_t window_height;
    screen_size_t shadow_map_size = 4096;

    glfwGetWindowSize(window, &window_width, &window_height);

    shader::ShaderHandler shader_handler;

    shader::Program quad_program = shader_handler.load_program(
        "Quad Renderer", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "overlay" / "SimpleTexture.frag"
    );

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    Scene main_scene(mode->width, mode->height, shadow_map_size);
    setup(main_scene, shader_handler, world, climate);

    do {
        controls::computeMatricesFromInputs(window);
        glfwGetWindowSize(window, &window_width, &window_height);

        main_scene.update_light_direction();
        main_scene.update(window_width, window_width);

        main_scene.copy_to_window(window_width, window_height);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window) == 0);

    glfwDestroyWindow(window);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

} // namespace gui
