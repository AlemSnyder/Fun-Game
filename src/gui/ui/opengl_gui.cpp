#include "opengl_gui.hpp"

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../util/files.hpp"
#include "../../world.hpp"
#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "../render/graphics_shaders/quad_renderer.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"
#include "../shader.hpp"
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


    std::optional<GLFWwindow*> window = setup_opengl(window_width, window_height);
    if (!window)
        return 1;
    setup_opengl_logging();

    // Vertex Arrays act like frame buffers.
    // A vertex array exists to hold all vertex data. One is needed to send
    // vertex data to the gpu. I don't know why one needs to create one like
    // this. Can I make multiple vertex arrays? If so why should I?
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    render::QuadRenderer QR;

    ShaderHandler shader_handler = ShaderHandler();
    
    world.update_all_chunks_mesh();
    Scene main_scene(window_width, window_height, shadow_map_size);
    setup(main_scene, world);

    do {
        controls::computeMatricesFromInputs(window.value());
        main_scene.update(window.value());

        // bind the the screen
        FrameBufferHandler::getInstance().bind_fbo(0); // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render to the screen
        QR.render(window_width, window_height, main_scene.get_scene(), 0);

        if (controls::show_shadow_map(window.value())) {
            QR.render(512, 512, main_scene.get_depth_texture(), 0);
        }

        // Swap buffers
        glfwSwapBuffers(window.value());
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window.value(), GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window.value()) == 0);

    // Cleanup VBO and shader
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwDestroyWindow(window.value());

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

} // namespace gui
