#include "config.h"
#include "entity/mesh.hpp"
#include "gui/controls.hpp"
#include "gui/main_gui.hpp"
#include "gui/shader.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"
#include "world.hpp"

#include <quill/Quill.h>
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

    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

    world.terrain_main.qb_save_debug(save_path);

    return 0;
}

void
save_terrain(Json::Value materials_json, Json::Value biome_data, std::string biome_name) {
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, biome_data, 0);

    std::cout << "Saving " << biome_data["Tile_Data"].size() << " tile types"
              << std::endl;

    // std::cout << biome_data["Tile_Data"] << std::endl;

    for (unsigned int i = 0; i < biome_data["Tile_Data"].size(); i++) {
        world.terrain_main.init(
            3, 3, World::macro_tile_size, World::height, 5, static_cast<int>(i),
            world.get_materials(), biome_data
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
save_all_terrain(Json::Value materials_json, Json::Value biome_data) {
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end();
         biome_type++) {
        save_terrain(materials_json, *biome_type, biome_type.key().asString());
    }
}

int
path_finder_test(const std::string path, std::string save_path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

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

std::vector<entity::Mesh>
get_mesh(const std::string path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;
    World world(materials_json, path);
    std::cout << "read from file" << std::endl;

    return world.get_mesh_greedy();
}

int
StressTest() {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    // Create world object from material data, biome data, and the number of
    // chunks in the x,y direction. Here the size is 2,2.
    World world(materials_json, biome_data, 2, 2);

    return gui::GUITest(world);
}

int
GUITest(const std::string path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;
    std::vector<std::uint16_t> indices;
    std::vector<glm::vec3> indexed_vertices;
    std::vector<glm::vec3> indexed_colors;
    std::vector<glm::vec3> indexed_normals;
    World world(materials_json, path);

    return gui::GUITest(world);
}

inline int
GUITest(const std::filesystem::path path) {
    return GUITest(path.string());
}

inline int
LogTest(){
    //quill::enable_console_colours();
    // Does not work a quill config object is required
    // Get the stdout file handler
    quill::Handler* file_handler = quill::stdout_handler();

    // Set a custom formatter for this handler
    file_handler->set_pattern("%(ascii_time) [%(process)] [%(thread)] - %(filename):%(function_name):%(lineno) - %(message)", // format
                                "%D %H:%M:%S.%Qms",     // timestamp format
                                quill::Timezone::GmtTime); // timestamp's timezone

    // Config using the custom ts class and the stdout handler
    quill::Config cfg;
    cfg.default_handlers.emplace_back(file_handler);
    cfg.enable_console_colours = true;
    quill::configure(cfg);
    quill::start();

    quill::Logger* logger = quill::get_logger();
    logger->set_log_level(quill::LogLevel::TraceL3);

    // enable a backtrace that will get flushed when we log CRITICAL
    logger->init_backtrace(2, quill::LogLevel::Critical);

    LOG_BACKTRACE(logger, "Backtrace log {}", 1);
    LOG_BACKTRACE(logger, "Backtrace log {}", 2);

    LOG_INFO(logger, "Welcome to Quill!");
    LOG_ERROR(logger, "An error message. error code {}", 123);
    LOG_WARNING(logger, "A warning message.");
    LOG_CRITICAL(logger, "A critical error.");
    LOG_DEBUG(logger, "Debugging foo {}", 1234);
    LOG_TRACE_L1(logger, "{:>30}", "right aligned");
    LOG_TRACE_L2(logger, "Positional arguments are {1} {0} ", "too", "supported");
    LOG_TRACE_L3(logger, "Support for floats {:03.2f}", 1.23456);
    return 0;
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

#if 0
    std::cout << argc << std::endl;

    std::cout << "Positional args: " << std::endl;
    for (auto& pos_arg : cmdl.pos_args())
        std::cout << '\t' << pos_arg << std::endl;

    std::cout << "Parameters: " << std::endl;
    for (auto& param : cmdl.params())
        std::cout << '\t' << param.first << " : " << param.second << std::endl;

    std::cout << "Running: " << run_function << ", with path in = " << path_in << ", and path out = " << path_out << std::endl;
#endif

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
        Json::Value materials_json;
        std::ifstream materials_file = files::open_data_file("materials.json");
        materials_file >> materials_json;
        
        cmdl("biome-name", "Biome_1") >> biome_name;
        if (!cmdl[{"-a", "--all"}]) {
            save_terrain(biome_data[biome_name], materials_json, biome_name);
        } else {
            save_all_terrain(materials_json, biome_data);
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
    } else if (run_function == "Logging") {
        return LogTest();
    }
}
