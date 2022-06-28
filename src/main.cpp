#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "terrain.hpp"
#include "world.hpp"
#include "GUI/controls.hpp"
#include "GUI/shader.hpp"

int test1(){

    //const char * home_path = "C:/Users/haile/Documents/School/Comp Sci but C/gcc/terrain_generation";

    Json::Value materials_json;
    std::ifstream materials_file("../data/materials.json", std::ifstream::in);
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;

    const char * path = "../SavedTerrain/save.qb";

    World world(materials_json, biome_data);

    world.terrain_main.qb_save_debug(path, world.get_materials());

    return 0;
}

int test2(){

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;


    Terrain::generate_macro_map(64,64, biome_data["Biome_1"]["Terrain_Data"]);

    return 1;
}

int savetest(){
    const char * path = "../SavedTerrain/save.qb";
    const char * save_path = "../SavedTerrain/save_test.qb";

    World world(path);

    world.terrain_main.qb_save(save_path);

    return 0;
}

void save_terrain(Json::Value biome_data, std::string biome_name){
    World world;

    std::cout << "Saving " << biome_data["Tile_Data"].size() << " tile types" << std::endl;

    //std::cout << biome_data["Tile_Data"] << std::endl;

    for (unsigned int i = 0; i < biome_data["Tile_Data"].size(); i++){
        world.terrain_main.init(3,3, 32, 128, 5, (int) i, world.get_materials(), biome_data);
        std::string path("../SavedTerrain/");
        path += biome_name;
        path += "/biome_";
        path += std::to_string(i);
        path += ".qb";
        world.terrain_main.qb_save(path.c_str());
    }
    //Json::Value biome_data;
    //std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    //biome_file >> biome_data;
}

void save_all_terrain(Json::Value biome_data){
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end(); biome_type++){
        save_terrain((*biome_type), biome_type.key().asCString());
    }
}

int pathfindertest(){
    const char * path = "../SavedTerrain/pathfinder_input.qb";
    const char * save_path = "../SavedTerrain/pathfinder_output.qb";

    World world(path);

    std::pair<Tile *, Tile *> start_end = world.terrain_main.get_start_end_test();

    std::vector<Tile*> tile_path = world.terrain_main.get_path_Astar(start_end.first, start_end.second);

    if (tile_path.size() == 0){
        std::cout << "no path" << std::endl;
        world.terrain_main.qb_save_debug(path, world.get_materials());
    }

    for (auto it = tile_path.begin(); it != tile_path.end(); ++it){
        (*it)->set_material(&world.get_materials()->at(6),1);
    }

    world.terrain_main.qb_save(save_path);

    return 0;
}

static void wait_for_input(void) {
    std::cerr << "Press any key to exit." << std::endl;
    getchar();  // Wait for input
}

int GUITest(){

    const char * path = "../SavedTerrain/pathfinder_input.qb";
    World world(path);
    
    // Initialise GLFW
    // glm::glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        wait_for_input();
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE,
                   GLFW_OPENGL_CORE_PROFILE);  // We don't want the old OpenGL

    // Open a window and create its OpenGL context
    // (In the accompanying source code, this variable is global for simplicity)
    window = glfwCreateWindow(1024, 768, "Mane Window", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to open GLFW window. If you have an Intel GPU, "
                  << "they are not 3.3 compatible. Try the 2.1 version of the "
                  << "tutorials." << std::endl;
        wait_for_input();

        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);  // Initialize GLEW
    glewExperimental = true;         // Needed in core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        wait_for_input();

        glfwTerminate();
        exit(-1);
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // GLuint programID = LoadShaders( "../src/SimpleVertexShader.vertexshader",
    // "../src/SimpleFragmentShader.fragmentshader" );

    GLuint programID = LoadShaders("../src/GUI/VertexShader.vertexshader",
                                   "../src/GUI/FragmentShader.fragmentshader");

    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint SunlightID = glGetUniformLocation(programID, "lightDir");

    std::vector<MoreVectors::vector3> vertices;
    //std::vector<glm::vec3> vertices;
    std::vector<MoreVectors::vector4> faces;
    world.terrain_main.Get_Mesh_Greedy(vertices, faces);

    std::vector<GLfloat> terrain_buffer_data;
    std::vector<GLfloat> terrain_color_buffer_data;
    std::vector<GLfloat> terrain_buffer_normal_data;
    for (MoreVectors::vector4 face : faces){
        terrain_buffer_data.push_back(vertices[face.x].x); terrain_buffer_data.push_back(vertices[face.x].y); terrain_buffer_data.push_back(vertices[face.x].z);
        terrain_buffer_data.push_back(vertices[face.y].x); terrain_buffer_data.push_back(vertices[face.y].y); terrain_buffer_data.push_back(vertices[face.y].z);
        terrain_buffer_data.push_back(vertices[face.z].x); terrain_buffer_data.push_back(vertices[face.z].y); terrain_buffer_data.push_back(vertices[face.z].z);

        uint8_t color_id = uint16_t(face.q) & 0xFF;
        uint8_t material_id = (uint16_t(face.q) >> 8) & 0xFF;
        uint32_t color = world.get_material(material_id)->color[color_id].second;


        terrain_color_buffer_data.push_back( float( ( color >> 24 ) & 0xFF ) / 255.0);
        terrain_color_buffer_data.push_back( float( ( color >> 16 ) & 0xFF ) / 255.0);
        terrain_color_buffer_data.push_back( float( ( color >> 8  ) & 0xFF ) / 255.0);

        terrain_color_buffer_data.push_back( float( ( color >> 24 ) & 0xFF ) / 255.0);
        terrain_color_buffer_data.push_back( float( ( color >> 16 ) & 0xFF ) / 255.0);
        terrain_color_buffer_data.push_back( float( ( color >> 8  ) & 0xFF ) / 255.0);

        terrain_color_buffer_data.push_back( float( ( color >> 24 ) & 0xFF ) / 255.0);
        terrain_color_buffer_data.push_back( float( ( color >> 16 ) & 0xFF ) / 255.0);
        terrain_color_buffer_data.push_back( float( ( color >> 8  ) & 0xFF ) / 255.0);

        glm::vec3 vertex1 = {vertices[face.x].x,vertices[face.x].y,vertices[face.x].z};
        glm::vec3 vertex2 = {vertices[face.y].x,vertices[face.y].y,vertices[face.y].z};
        glm::vec3 vertex3 = {vertices[face.z].x,vertices[face.z].y,vertices[face.z].z};

        glm::vec3 normal = glm::cross(vertex1 - vertex2, vertex1 - vertex3);

        //int normal_packed = static_cast<uint8_t>(normal.x < 0) << 2 | static_cast<uint8_t>(normal.y < 0) << 1 | static_cast<uint8_t>(normal.z < 0) | static_cast<uint8_t>(normal.x > 0) << 5 | static_cast<uint8_t>(normal.y > 0) << 4 | static_cast<uint8_t>(normal.z > 0) << 3;

        //if (normal_packed == 0){
        //    std::cout << (int) normal_packed << std::endl;
        //}

        //std::cout << normal_packed << std::endl;

        terrain_buffer_normal_data.push_back(normal.x); terrain_buffer_normal_data.push_back(normal.y); terrain_buffer_normal_data.push_back(normal.z);
        terrain_buffer_normal_data.push_back(normal.x); terrain_buffer_normal_data.push_back(normal.y); terrain_buffer_normal_data.push_back(normal.z);
        terrain_buffer_normal_data.push_back(normal.x); terrain_buffer_normal_data.push_back(normal.y); terrain_buffer_normal_data.push_back(normal.z);
    }

    //std::vector<GLfloat> terrain_color_buffer_data(terrain_buffer_data.size(), .5);

    // This will identify our vertex buffer
    GLuint vertexbuffer;
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * terrain_buffer_data.size(),
                &terrain_buffer_data[0], GL_STATIC_DRAW);

    std::cout <<"Size of color buffer data: " << (int) sizeof(float) * terrain_color_buffer_data.size();

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * terrain_color_buffer_data.size(),
                &terrain_color_buffer_data[0], GL_STATIC_DRAW);

    GLuint normalbuffer;
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * terrain_buffer_normal_data.size(),
                &terrain_buffer_normal_data[0], GL_STATIC_DRAW);

    do {
        // Clear the screen. It's not mentioned before Tutorial 02, but it can
        // cause flickering, so it's there nonetheless.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0);
        glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniform3f(SunlightID, 1,2,6 );

        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0,  // attribute 0. No particular reason for 0,
                                  // but must match the layout in the shader.
                              3,         // size
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,         // stride
                              (void*)0   // array buffer offset
        );
        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1,  // attribute. No particular reason for 1, but
                                  // must match the layout in the shader.
                              3,         // size
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,         // stride
                              (void*)0   // array buffer offset
        );
        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
        glVertexAttribPointer(2,  // attribute. No particular reason for 2, but
                                  // must match the layout in the shader.
                              3,         // size
                              GL_FLOAT,  // type
                              GL_FALSE,  // normalized?
                              0,         // stride
                              (void*)0   // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(
            GL_TRIANGLES, 0,
            terrain_buffer_data.size() / 3);  // Starting from vertex 0; 3 vertices total -> 1 triangle

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }  // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

int main( int argc, char** argv ){
    if (argc == 1){
        return pathfindertest();
    } else if (std::string(argv[1]) == "--TerrainTypes"){
        Json::Value biome_data;
        std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
        biome_file >> biome_data;
        if (argc == 3){
            save_terrain(biome_data[argv[2]],argv[2]);
        }
        else{
            save_all_terrain(biome_data);
        }
    } else if (std::string(argv[1]) == "--MacroMap"){
        return test2();
    }
    else if (std::string(argv[1]) == "--SaveTest"){
        return savetest();
    }
    else if (std::string(argv[1]) == "--pathfinder"){
        return pathfindertest();
    }
    else if (std::string(argv[1]) == "--GUITest"){
        return GUITest();
    }
}
