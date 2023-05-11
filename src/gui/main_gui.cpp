#include "main_gui.hpp"

#include "../entity/mesh.hpp"
#include "../entity/static_mesh.hpp"
#include "../entity/terrain_mesh.hpp"
#include "../logging.hpp"
#include "../util/files.hpp"
#include "../world.hpp"
#include "controls.hpp"
#include "gui_logging.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "shadow_map.hpp"

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
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        // set gl message call back function
        glDebugMessageCallback(message_callback, 0);
        glDebugMessageControl(
            GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE
        );
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
        fprintf(stderr, "Failed to initialize GLEW\n");
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
    glClearColor(0.42f, 0.79f, 0.94f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    //  The mesh of the terrain
    world.update_all_chunk_mesh();
    const std::vector<std::shared_ptr<terrain::TerrainMesh>>& chunks_mesh = world.get_chunks_mesh();

    LOG_INFO(logging::opengl_logger, "Chunk meshes sent to graphics buffer.");

    // The above is for the world the below is for trees

    std::vector<glm::ivec3> model_matrices;
    // generate positions of trees
    for (size_t x = 0; x < world.get_terrain_main().get_X_MAX(); x += 40)
        for (size_t y = 0; y < world.get_terrain_main().get_Y_MAX(); y += 40) {
            size_t z = world.get_terrain_main().get_Z_solid(x, y) + 1;
            if (z != 1) { // if the position of the ground is not zero
                glm::ivec3 model(x, y, z);
                model_matrices.push_back(model);
            }
        }

    LOG_INFO(logging::opengl_logger, "Number of models: {}", model_matrices.size());
    // static because the mesh does not have moving parts
    // this generates the buffer that holds the mesh data
    terrain::StaticMesh treesMesh(mesh_trees, model_matrices);

    // The quad's FBO. Used only for visualizing the shadow map.
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    };

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data,
        GL_STATIC_DRAW
    );

    // Create and compile our GLSL program from the shaders
    GLuint quad_programID = load_shaders(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "SimpleTexture.frag"
    );
    GLuint texID = glGetUniformLocation(quad_programID, "texture");

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f)) // direction
        * 128.0f;                                       // length

    glm::mat4 depth_projection_matrix =
        glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    // Renders the Shadow depth map
    ShadowMap SM(4096, 4096);
    SM.set_light_direction(light_direction);
    SM.set_depth_projection_matrix(depth_projection_matrix);

    for (auto& m : chunks_mesh)
        SM.add_mesh(m);
    SM.add_mesh(std::make_shared<terrain::StaticMesh>(treesMesh));

    // renders the world scene
    MainRenderer MR;
    MR.set_light_direction(light_direction);
    MR.set_depth_projection_matrix(depth_projection_matrix);

    for (auto& m : chunks_mesh)
        MR.add_mesh(m);

    MR.add_mesh(std::make_shared<terrain::StaticMesh>(treesMesh));
    MR.set_depth_texture(SM.get_depth_texture());

    unsigned int frame_id = 0;
    bool do_set_tile_material = false;

    do {
        SM.render_shadow_depth_buffer();
        MR.render(window);

        if (controls::show_shadow_map(window)) {
            glViewport(0, 0, 512, 512);

            // Use our shader
            glUseProgram(quad_programID);

            // Bind our texture in Texture Unit 0
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, SM.get_depth_texture());
            // Set our "renderedTexture" sampler to use Texture Unit 0
            glUniform1i(texID, 0);

            // first attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
            glVertexAttribPointer(
                0,        // attribute 0. No particular reason for 0,
                          // but must match the layout in the shader.
                3,        // size
                GL_FLOAT, // type
                GL_FALSE, // normalized?
                0,        // stride
                (void*)0  // array buffer offset
            );

            // Draw the triangle !
            // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see
            glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting
            // at 0 -> 2 triangles
            glDisableVertexAttribArray(0);
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

        // run with debug, to set to true
        if (do_set_tile_material){
            world.set_tile(frame_id, world.get_material(7), 0);
            do_set_tile_material = false;
            frame_id ++;
        }

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteProgram(quad_programID);

    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

} // namespace gui
