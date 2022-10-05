#include "config.h"
#include "entity/mesh.hpp"
#include "gui/controls.hpp"
#include "gui/main_gui.hpp"
#include "gui/shader.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"
#include "world.hpp"

#include <argh.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <stdint.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#define INITIAL_WINDOW_WIDTH  1024
#define INITIAL_WINDOW_HEIGHT 768

int
GenerateTerrain(const std::string path) {
    // const char * home_path = "C:/Users/haile/Documents/School/Comp Sci but
    // C/gcc/terrain_generation";

    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    World world(materials_json, biome_data, 6, 6);

    world.terrain_main.qb_save(path);

    return 0;
}

int
MacroMap() {
    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    terrain::Terrain::generate_macro_map(64, 64, biome_data["Biome_1"]["Terrain_Data"]);

    return 1;
}

int
save_test(const std::string path, const std::string save_path) {
    // path = "../SavedTerrain/pathfinder_input_4.qb";
    // save_path = "../SavedTerrain/save_test.qb";

    World world(path);

    world.terrain_main.qb_save_debug(save_path);

    return 0;
}

void
save_terrain(Json::Value biome_data, std::string biome_name) {
    World world;

    std::cout << "Saving " << biome_data["Tile_Data"].size() << " tile types"
              << std::endl;

    // std::cout << biome_data["Tile_Data"] << std::endl;

    for (unsigned int i = 0; i < biome_data["Tile_Data"].size(); i++) {
        world.terrain_main.init(
            3, 3, World::macro_tile_size, World::height, 5, static_cast<int>(i), world.get_materials(), biome_data
        );
        std::filesystem::path save_path = files::get_root_path() / "SavedTerrain";
        save_path /= biome_name;
        save_path /= "biome_";
        save_path += std::to_string(i);
        save_path += ".qb";
        world.terrain_main.qb_save(save_path.string());
    }
    // Json::Value biome_data;
    // std::ifstream biome_file("./data/biome_data.json", std::ifstream::in);
    // biome_file >> biome_data;
}

void
save_all_terrain(Json::Value biome_data) {
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end();
         biome_type++) {
        save_terrain(*biome_type, biome_type.key().asString());
    }
}

int
path_finder_test(const std::string path, std::string save_path) {
    // path = "../SavedTerrain/pathfinder_input.qb";
    // save_path = "../SavedTerrain/pathfinder_output.qb";

    World world(path);

    std::pair<terrain::Tile*, terrain::Tile*> start_end =
        world.terrain_main.get_start_end_test();

    std::cout << "Start: " << start_end.first->get_x() << ", "
              << start_end.first->get_y() << ", " << start_end.first->get_z()
              << std::endl;
    std::cout << "End:   " << start_end.second->get_x() << ", "
              << start_end.second->get_y() << ", " << start_end.second->get_z()
              << std::endl;

    std::vector<const terrain::Tile*> tile_path =
        world.terrain_main.get_path_Astar(start_end.first, start_end.second);

    std::cout << "    " << static_cast<int>(tile_path.size()) << std::endl;
    if (tile_path.size() == 0) {
        std::cout << "no path" << std::endl;
        world.terrain_main.qb_save_debug(save_path);
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

entity::Mesh
get_mesh(const std::string path) {
    World world(path);
    // World world(path);
    std::cout << "read from file" << std::endl;

    return world.get_mesh_greedy();
}

int StressTest() {

    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    World world(materials_json, biome_data, 2, 2);

    return gui::GUITest(world);
}

int
GUITest(const std::string path) {
    std::vector<std::uint16_t> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_colors;
    std::vector<glm::vec3> indexed_normals;
    World world(path);

    return gui::GUITest(world);
}

inline int
GUITest(const std::filesystem::path path) {
    return GUITest(path.string());
}

int
main(int argc, char** argv) {
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

    std::cout << "Running: " << run_function << ", with path in = " << path_in << ", and
    path out = " << path_out << std::endl;
    */

    std::cout << "FunGame v" << VERSION_MAJOR << "." << VERSION_MINOR << "."
              << VERSION_PATCH << std::endl;
    std::cout << "Running from " << files::get_root_path() << "." << std::endl;

    if (argc == 1) {
        return GUITest(files::get_data_path() / "models" / "DefaultTree.qb");
    } else if (run_function == "TerrainTypes") {
        Json::Value biome_data;
        std::ifstream biome_file = files::open_data_file("biome_data.json");
        biome_file >> biome_data;
        std::string biome_name;
        cmdl("biome-name", "Biome_1") >> biome_name;
        if (!cmdl[{"-a", "--all"}]) {
            save_terrain(biome_data[biome_name], biome_name);
        } else {
            save_all_terrain(biome_data);
        }
    } else if (run_function == "GenerateTerrain") {
        return GenerateTerrain(path_in);
    } else if (run_function == "MacroMap") {
        return MacroMap();
    } else if (run_function == "StressTest") {
        return StressTest();
    } else if (run_function == "SaveTest") {
        return save_test(path_in, path_out);
    } else if (run_function == "PathFinder") {
        return path_finder_test(path_in, path_out);
    } else if (run_function == "GUITest") {
        return GUITest(path_in);
    }
}
