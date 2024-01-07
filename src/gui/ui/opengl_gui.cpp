#include "opengl_gui.hpp"

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../util/files.hpp"
#include "../../world.hpp"
#include "../gui_logging.hpp"
#include "../handler.hpp"
//#include "../render/graphics_shaders/quad_renderer.hpp"
//#include "../render/graphics_shaders/shader.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"
#include "opengl_setup.hpp"
#include "scene_setup.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdlib>
#include <vector>

namespace gui {

int
opengl_entry(World& world) {
    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;
    screen_size_t shadow_map_size = 4096;

    std::optional<GLFWwindow*> opt_window = setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    GLFWwindow* window = opt_window.value();
    setup_opengl_logging();

    // Vertex Arrays act like frame buffers.
    // A vertex array exists to hold all vertex data. One is needed to send
    // vertex data to the gpu. I don't know why one needs to create one like
    // this. Can I make multiple vertex arrays? If so why should I?
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);

    shader::ShaderHandler shader_handler;

    shader::Program quad_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTexture.frag"
    );

//    render::QuadRenderer QR(quad_program); TODO



    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    Scene main_scene(mode->width, mode->height, shadow_map_size);
    setup(main_scene, shader_handler, world);

    do {
        controls::computeMatricesFromInputs(window);
        glfwGetWindowSize(window, &window_width, &window_height);

        main_scene.update_light_direction();
        main_scene.update(window_width, window_width);

        main_scene.copy_to_window(window_width, window_height);

        if (controls::show_shadow_map(window)) {
      //      QR.render(512, 512, main_scene.get_depth_texture(), 0);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwDestroyWindow(window);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

} // namespace gui
