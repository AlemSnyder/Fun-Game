#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "../Entity/mesh.hpp"
#include "../Terrain/static_mesh.hpp"
#include "../Terrain/terrain_mesh.hpp"
#include "../world.hpp"
#include "controls.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "shadow_map.hpp"

namespace GUI {

int GUITest(World world) {
    std::vector<std::uint16_t> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_colors;
    std::vector<glm::vec3> indexed_normals;

    world.get_mesh_greedy(indices, indexed_vertices, indexed_colors,
                          indexed_normals);

    std::vector<std::uint16_t> indices_tree;
    std::vector<glm::vec3> indexed_vertices_tree;
    std::vector<glm::vec3> indexed_colors_tree;
    std::vector<glm::vec3> indexed_normals_tree;
    Mesh mesh("../data/Models/DefaultTree.qb");
    mesh.get_mesh(indices_tree, indexed_vertices_tree, indexed_colors_tree,
                  indexed_normals_tree);

    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);                // anti-aliasing of 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // set Major
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);  // and Minor version
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,
                   GL_TRUE);  // To make MacOS happy; should not be needed
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);  // somehow turning on core profiling

    // Open a window and create its OpenGL context
    // We would expect width and height to be 1024 and 768
    int windowFrameWidth = 1024;
    int windowFrameHeight = 768;
    GLFWwindow* window = glfwCreateWindow(windowFrameWidth, windowFrameHeight,
                                          "Mane Window", NULL, NULL);
    if (window == NULL) {
        std::cerr
            << "Failed to open GLFW window. If you have an Intel GPU, they are "
               "not 3.3 compatible. Try the 2.1 version of the tutorials."
            << std::endl;
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // But on MacOS X with a retina screen it'll be 1024*2 and 768*2, so we get
    // the actual framebuffer size:
    glfwGetFramebufferSize(window, &windowFrameWidth, &windowFrameHeight);

    // Initialize GLEW
    glewExperimental = true;  // Needed for core profile
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

    // Dark blue background
    glClearColor(0.42f, 0.79f, 0.94f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    TerrainMesh terrain_mesh(indices, indexed_vertices, indexed_colors,
                             indexed_normals);

    // The above is for the wold the below is for trees

    std::vector<glm::vec3> model_matrices;

    for (int x = 0; x < world.terrain_main.get_X_MAX(); x += 40)
        for (int y = 0; y < world.terrain_main.get_Y_MAX(); y += 40) {
            int z = world.terrain_main.get_Z_solid(x, y) + 1;
            if (z!= 1) {
                glm::vec3 model(x, y, z);
                model_matrices.push_back(model);
            }
        }

    std::cout << "Number of models: " << model_matrices.size() << std::endl;

    StaticMesh treesMesh(indices_tree, indexed_vertices_tree,
                         indexed_colors_tree, indexed_normals_tree,
                         model_matrices);

    // The quad's FBO. Used only for visualizing the shadow map.
    static const GLfloat g_quad_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f,  0.0f, 1.0f, -1.0f, 0.0f, 1.0f,  1.0f, 0.0f,
    };

    GLuint quad_vertexbuffer;
    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data),
                 g_quad_vertex_buffer_data, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    GLuint quad_programID =
        LoadShaders("../src/GUI/Shaders/Passthrough.vert",
                    "../src/GUI/Shaders/SimpleTexture.frag");
    GLuint texID = glGetUniformLocation(quad_programID, "texture");

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f))  // direction
        * 128.0f;                                        // length

    glm::mat4 depth_projection_matrix = glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    ShadowMap SM(4096, 4096);
    SM.set_light_direction(light_direction);
    SM.set_depth_projection_matrix(depth_projection_matrix);
    SM.add_mesh(std::make_shared<TerrainMesh>(terrain_mesh));
    SM.add_mesh(std::make_shared<StaticMesh>(treesMesh));

    MainRenderer MR;
    MR.set_light_direction(light_direction);
    MR.set_depth_projection_matrix(depth_projection_matrix);
    MR.add_mesh(std::make_shared<TerrainMesh>(terrain_mesh));
    MR.add_mesh(std::make_shared<StaticMesh>(treesMesh));
    MR.set_depth_texture(SM.get_depth_texture());
    do {
        SM.render_shadow_depth_buffer();
        MR.render(window);

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
        glVertexAttribPointer(0,  // attribute 0. No particular reason for 0,
                                  // but must match the layout in the shader.
                              3,         // size
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,         // stride
                              (void*)0   // array buffer offset
        );

        // Draw the triangle !
        // You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see
        glDrawArrays(GL_TRIANGLES, 0, 6);  // 2*3 indices starting
        // at 0 -> 2 triangles
        glDisableVertexAttribArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteProgram(quad_programID);

    glDeleteBuffers(1, &quad_vertexbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

}  // namespace GUI
