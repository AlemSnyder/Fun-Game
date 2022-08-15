#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow *window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Packages/argh.h"

#include "GUI/controls.hpp"
#include "GUI/shader.hpp"
#include "GUI/main_gui.hpp"
#include "Terrain/terrain.hpp"
#include "world.hpp"
#include "Entity/mesh.hpp"

#define INITIAL_WINDOW_WIDTH 1024
#define INITIAL_WINDOW_HEIGHT 768

int test1(const char *path)
{
    // const char * home_path = "C:/Users/haile/Documents/School/Comp Sci but
    // C/gcc/terrain_generation";

    Json::Value materials_json;
    std::ifstream materials_file("../data/materials.json", std::ifstream::in);
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;

    World world(materials_json, biome_data);

    world.terrain_main.qb_save(path);

    return 0;
}

int test2()
{

    Json::Value biome_data;
    std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    biome_file >> biome_data;

    Terrain::generate_macro_map(64, 64, biome_data["Biome_1"]["Terrain_Data"]);

    return 1;
}

int save_test(const char *path, const char *save_path)
{
    // path = "../SavedTerrain/pathfinder_input_4.qb";
    // save_path = "../SavedTerrain/save_test.qb";

    World world(path);

    world.terrain_main.qb_save_debug(save_path, world.get_materials());

    return 0;
}

void save_terrain(Json::Value biome_data, std::string biome_name)
{
    World world;

    std::cout << "Saving " << biome_data["Tile_Data"].size() << " tile types"
              << std::endl;

    // std::cout << biome_data["Tile_Data"] << std::endl;

    for (unsigned int i = 0; i < biome_data["Tile_Data"].size(); i++) {
        world.terrain_main.init(3, 3, 32, 128, 5, (int)i, world.get_materials(),
                                biome_data);
        std::string path("../SavedTerrain/");
        path += biome_name;
        path += "/biome_";
        path += std::to_string(i);
        path += ".qb";
        world.terrain_main.qb_save(path.c_str());
    }
    // Json::Value biome_data;
    // std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
    // biome_file >> biome_data;
}

void save_all_terrain(Json::Value biome_data)
{
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end();
         biome_type++) {
        save_terrain((*biome_type), biome_type.key().asCString());
    }
}

int path_finder_test(const char *path, const char *save_path)
{
    // path = "../SavedTerrain/pathfinder_input.qb";
    // save_path = "../SavedTerrain/pathfinder_output.qb";

    World world(path);

    std::pair<Tile *, Tile *> start_end =
        world.terrain_main.get_start_end_test();

    std::cout << "Start: " << start_end.first->get_x() << ", "
              << start_end.first->get_y() << ", " << start_end.first->get_z()
              << std::endl;
    std::cout << "End:   " << start_end.second->get_x() << ", "
              << start_end.second->get_y() << ", " << start_end.second->get_z()
              << std::endl;

    std::vector<const Tile *> tile_path =
        world.terrain_main.get_path_Astar(start_end.first, start_end.second);

    std::cout << "    " << (int)tile_path.size() << std::endl;
    if (tile_path.size() == 0) {
        std::cout << "no path" << std::endl;
        world.terrain_main.qb_save_debug(save_path, world.get_materials());
        return 1;
    }

    for (auto it = tile_path.begin(); it != tile_path.end(); ++it) {
        std::cout << "    " << (*it)->get_x() << " " << (*it)->get_y() << " "
                  << (*it)->get_z() << std::endl;
        world.terrain_main.get_tile(world.terrain_main.pos((*it)->sop()))
            ->set_material(&world.get_materials()->at(7), 5);
    }

    world.terrain_main.qb_save(save_path);

    return 0;
}

void get_mesh(const char * path,
    std::vector<std::uint16_t>& indices,
    std::vector<glm::vec3>& indexed_vertices,
    std::vector<glm::vec3>& indexed_colors,
    std::vector<glm::vec3>& indexed_normals){

    World world(path);
    //World world(path);
    std::cout << "read from file" << std::endl;

    world.get_mesh_greedy(indices,
            indexed_vertices,
            indexed_colors,
            indexed_normals);
    }

int GUITest(const char *path)
{
    std::vector<std::uint16_t> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_colors;
    std::vector<glm::vec3> indexed_normals;
    World world(path);

    return GUI::GUITest(world);
}

int main(int argc, char **argv)
{
    argh::parser cmdl;
    cmdl.add_params({"-pi", "--path-in", "-po", "--path-out"});
    cmdl.add_param("biome-name");
    cmdl.parse(argc, argv);
    std::string run_function = cmdl(1).str();
    std::string path_in = cmdl(2).str();
    std::string path_out = cmdl(3).str();


    /*
    std::cout << argc << std::endl;

    std::cout << "Positional args: " << std::endl;
    for (auto& pos_arg : cmdl.pos_args())
        std::cout << '\t' << pos_arg << std::endl;

    std::cout << "Parameters: " << std::endl;
    for (auto& param : cmdl.params())
        std::cout << '\t' << param.first << " : " << param.second << std::endl;

    std::cout << "Running: " << run_function << ", with path in = " << path_in << ", and path out = " << path_out << std::endl;
    */

    if (argc == 1) {
        return GUITest("../data/Models/DefaultTree.qb");
    } else if (run_function == "TerrainTypes") {
        Json::Value biome_data;
        std::ifstream biome_file("../data/biome_data.json", std::ifstream::in);
        biome_file >> biome_data;
        std::string biome_name;
        cmdl("biome-name", "Biome_1") >> biome_name;
        if (!cmdl[{ "-a", "--all" }]) {
            save_terrain(biome_data[biome_name], biome_name);
        } else {
            save_all_terrain(biome_data);
        }
    } else if (run_function == "GenerateTerrain") {
        return test1(&path_in[0]);
    } else if (run_function == "MacroMap") {
        return test2();
    } else if (run_function == "SaveTest") {
        return save_test(&path_in[0], &path_out[0]);
    } else if (run_function == "PathFinder") {
        return path_finder_test(&path_in[0], &path_out[0]);
    } else if (run_function == "GUITest") {
        return GUITest(&path_in[0]);
    } else if (run_function == "WorldGen") {
        return test1(&path_in[0]);
    }
}
