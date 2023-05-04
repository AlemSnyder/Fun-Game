#include "main_gui.hpp"

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../util/files.hpp"
#include "../../world.hpp"
#include "controls.hpp"
#include "../data_structures/screen_data.hpp"
#include "../data_structures/sky_data.hpp"
#include "../data_structures/static_mesh.hpp"
#include "../data_structures/terrain_mesh.hpp"
#include "../gui_logging.hpp"
#include "../render/quad_renderer.hpp"
#include "../render/renderer.hpp"
#include "../render/shadow_map.hpp"
#include "../render/sky.hpp"
#include "../shader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <cstdlib>
#include <vector>

namespace gui {

int
GUITest(World world) {
    auto mesh = world.get_mesh_greedy();

    LOG_INFO(logging::opengl_logger, "End of World::get_mesh_greedy");

    voxel_utility::VoxelObject default_trees_voxel(
        files::get_data_path() / "models" / "DefaultTree.qb"
    );

    auto mesh_trees = entity::generate_mesh(default_trees_voxel);

    // initialize logging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
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
        } catch (...) {
            LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
            getchar();
            glfwTerminate();
            return -1;
        }
    }

    LOG_INFO(logging::opengl_logger, "GLFW Logging initialized");

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

    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    // No idea why this is necessary, but it is
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //  The mesh of the terrain
    std::vector<terrain::TerrainMesh> chunk_meshes;
    chunk_meshes.resize(mesh.size());
    for (size_t i = 0; i < chunk_meshes.size(); i++) {
        chunk_meshes[i].init(mesh[i]);
    }

    LOG_INFO(logging::opengl_logger, "Chunk meshes sent to graphics buffer.");

    // The above is for the wold the below is for trees

    std::vector<glm::ivec3> model_matrices;
    // generate positions of trees
    for (size_t x = 0; x < world.terrain_main.get_X_MAX(); x += 40)
        for (size_t y = 0; y < world.terrain_main.get_Y_MAX(); y += 40) {
            size_t z = world.terrain_main.get_Z_solid(x, y) + 1;
            if (z != 1) { // if the position of the ground is not zero
                glm::ivec3 model(x, y, z);
                model_matrices.push_back(model);
            }
        }

    LOG_INFO(logging::opengl_logger, "Number of models: {}", model_matrices.size());
    // static because the mesh does not have moving parts
    // this generates the buffer that holds the mesh data
    terrain::StaticMesh treesMesh(mesh_trees, model_matrices);

    // generates a frame buffer, screen texture, and and a depth buffer
    GLuint window_frame_buffer = 0;
    glGenFramebuffers(1, &window_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, window_frame_buffer);

    GLuint window_render_texture;
    glGenTextures(1, &window_render_texture);

    glBindTexture(GL_TEXTURE_2D, window_render_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, windowFrameWidth, windowFrameHeight, 0, GL_RGB,
        GL_UNSIGNED_BYTE, 0
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint window_depth_buffer;
    glGenRenderbuffers(1, &window_depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, window_depth_buffer);
    glRenderbufferStorage(
        GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowFrameWidth, windowFrameHeight
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, window_depth_buffer
    );

    glFramebufferTexture(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, window_render_texture, 0
    );
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return -1;
    }
    // end
    // all of the above needs to be in imgui

    QuadRenderer QR;

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f)) // direction
        * 128.0f;                                       // length

    glm::mat4 depth_projection_matrix =
        glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    // Renders the Shadow depth map
    ShadowMap SM(4096, 4096);
    SM.set_light_direction(light_direction);
    SM.set_depth_projection_matrix(depth_projection_matrix);

    for (auto& m : chunk_meshes) {
        SM.add_mesh(std::make_shared<terrain::TerrainMesh>(m));
    }
    SM.add_mesh(std::make_shared<terrain::StaticMesh>(treesMesh));

    // renders the world scene
    MainRenderer MR;
    MR.set_light_direction(light_direction);
    MR.set_depth_projection_matrix(depth_projection_matrix);

    for (auto& m : chunk_meshes) {
        MR.add_mesh(std::make_shared<terrain::TerrainMesh>(m));
    }
    MR.add_mesh(std::make_shared<terrain::StaticMesh>(treesMesh));
    MR.set_depth_texture(SM.get_depth_texture());

    sky::SkyRenderer SR;

    do {
        controls::computeMatricesFromInputs(window);
        SM.render_shadow_depth_buffer();
        // clear the frame buffer each frame
        glBindFramebuffer(GL_FRAMEBUFFER, window_frame_buffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render the sky to the frame buffer
        SR.render(window, window_frame_buffer);
        // render the sene to the frame buffer
        MR.render(window, window_frame_buffer);

        // bind the the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // render to the screen
        QR.render(windowFrameWidth, windowFrameHeight, window_frame_buffer, 0);

        if (controls::show_shadow_map(window)) {
            QR.render(512, 512, SM.get_depth_texture(), 0);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteRenderbuffers(1, &window_depth_buffer);
    glDeleteTextures(1, &window_render_texture);
    glDeleteFramebuffers(1, &window_frame_buffer);

    glfwDestroyWindow(window);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

} // namespace gui
