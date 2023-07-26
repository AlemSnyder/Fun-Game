#include "main_gui.hpp"

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../util/files.hpp"
#include "../../world.hpp"
#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "../render/gui_models/quad_renderer.hpp"
#include "../scene/scene.hpp"
#include "../shader.hpp"
#include "controls.hpp"
#include "scene.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cstdlib>
#include <vector>

namespace gui {

int
GUITest(World& world) {
    LOG_INFO(logging::opengl_logger, "End of World::get_mesh_greedy");

    glEnable(GL_MULTISAMPLE);

    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);               // anti-aliasing of 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set Major
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // and Minor version
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    ); // To make MacOS happy; should not be needed
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    ); // somehow turning on core profiling

    // initialize logging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    // Open a window and create its OpenGL context
    // We would expect width and height to be 1024 and 768
    int windowFrameWidth = 1024;
    int windowFrameHeight = 768;
    GLFWwindow* window = glfwCreateWindow(
        windowFrameWidth, windowFrameHeight, "Mane Window", NULL, NULL
    );
    if (window == NULL) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to open GLFW window");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get
    // the actual framebuffer size:
    glfwGetFramebufferSize(window, &windowFrameWidth, &windowFrameHeight);

    LOG_INFO(logging::opengl_logger, "Window initialized");

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLEW");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited movement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, windowFrameWidth / 2, windowFrameHeight / 2);

    // Turn on VSync
    glfwSwapInterval(1);

    // Light blue background
    glClearColor(0.02f, 0.06f, 0.1f, 0.0f);

    // initialize logging
    GLint context_flag;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flag);
    if (context_flag & GL_CONTEXT_FLAG_DEBUG_BIT) {
        LOG_INFO(logging::opengl_logger, "GLFW Logging with debug");
        try {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            // set gl message call back function
            glDebugMessageCallback(message_callback, 0);
            glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE
            );
            LOG_INFO(logging::opengl_logger, "GLFW Logging initialized");
        } catch (...) {
            LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
            getchar();
            glfwTerminate();
            return -1;
        }
    }

    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    // No idea why this is necessary, but it is
    // I do know why, just don't want to explain
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    ShaderHandeler shader_handeler = ShaderHandeler();

    render::QuadRenderer QR;

    world.update_all_chunks_mesh();
    //  auto mesh = world.get_chunks_mesh();

    gui::Scene main_scene(windowFrameWidth, windowFrameHeight, 4096);

    //std::vector<std::shared_ptr<gui::data_structures::TerrainMesh>> 
    auto terrain_mesh = world.get_chunks_mesh();

    models::IndividualIntRenderer<data_structures::TerrainMesh> chunk_renderer(
        shader_handeler
    );

    // chunk_renderer.add_mesh()

    for (const auto& chunk_mesh : terrain_mesh) {
        chunk_mesh->set_color_texture(terrain::TerrainColorMapping::get_color_texture());
        chunk_renderer.add_mesh(chunk_mesh);
    }

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f)) // direction
        * 128.0f;                                       // length

    glm::mat4 depth_projection_matrix =
        glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    // Renders the Shadow depth map
    chunk_renderer.set_light_direction(light_direction);
    chunk_renderer.set_depth_projection_matrix(depth_projection_matrix);
    
    chunk_renderer.set_depth_texture(main_scene.get_depth_texture());

    main_scene.set_shadow_light_direction(light_direction);
    main_scene.set_shadow_depth_projection_matrix(depth_projection_matrix);

    main_scene.frame_buffer_multisample_attach(
        std::make_shared<
            models::IndividualIntRenderer<data_structures::TerrainMesh>>(
            chunk_renderer
        )
    );

    main_scene.shadow_attach(std::make_shared<models::IndividualIntRenderer<
                                  data_structures::TerrainMesh>>(chunk_renderer));

    // main_scene.add render ()
    //  n more lines after this

    do {
        controls::computeMatricesFromInputs(window);
        main_scene.update(window);

        // bind the the screen
        gui::FrameBufferHandler::bind_fbo(0);
        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render to the screen
        QR.render(windowFrameWidth, windowFrameHeight, main_scene.get_scene(), 0);

        if (controls::show_shadow_map(window)) {
            QR.render(512, 512, main_scene.get_depth_texture(), 0);
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
