#include "config.h"
#include "entity/mesh.hpp"
#include "gui/controls.hpp"
#include "gui/main_gui.hpp"
#include "gui/shader.hpp"
#include "logging.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"
#include "world.hpp"

#include <argh.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <quill/Quill.h>
#include <stdint.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#define INITIAL_WINDOW_WIDTH  1024
#define INITIAL_WINDOW_HEIGHT 768

int
GenerateTerrain(const std::string path)
{
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
MacroMap()
{
    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    terrain::Terrain::generate_macro_map(64, 64, biome_data["Biome_1"]["Terrain_Data"]);

    return 1;
}

int
save_test(const std::string path, const std::string save_path)
{
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

    world.terrain_main.qb_save_debug(save_path);

    return 0;
}

void
save_terrain(Json::Value materials_json, Json::Value biome_data, std::string biome_name)
{
    quill::Logger* logger = quill::get_logger();

    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, biome_data, 0);

    LOG_INFO(logger, "Saving {} tile types", biome_data["Tile_Data"].size());

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
}

void
save_all_terrain(Json::Value materials_json, Json::Value biome_data)
{
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end();
         biome_type++) {
        save_terrain(materials_json, *biome_type, biome_type.key().asString());
    }
}

int
path_finder_test(const std::string path, std::string save_path)
{
    quill::Logger* logger = quill::get_logger();

    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

    std::pair<terrain::Tile*, terrain::Tile*> start_end =
        world.terrain_main.get_start_end_test();

    LOG_INFO(
        logger, "Start: {}, {}, {}", start_end.first->get_x(), start_end.first->get_y(),
        start_end.first->get_z()
    );

    LOG_INFO(
        logger, "End: {}, {}, {}", start_end.second->get_x(), start_end.second->get_y(),
        start_end.second->get_z()
    );

    std::vector<const terrain::Tile*> tile_path =
        world.terrain_main.get_path_Astar(start_end.first, start_end.second);

    LOG_INFO(logger, "Path length: {}", tile_path.size());

    if (tile_path.size() == 0) {
        LOG_INFO(logger, "No path");
        world.terrain_main.qb_save_debug(save_path);
        return 1;
    }

    for (auto it = tile_path.begin(); it != tile_path.end(); ++it) {
        world.terrain_main.get_tile(world.terrain_main.pos((*it)->sop()))
            ->set_material(&world.get_materials()->at(7), 5);
    }

    world.terrain_main.qb_save(save_path);

    return 0;
}

std::vector<entity::Mesh>
get_mesh(const std::string path)
{
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;
    World world(materials_json, path);

    return world.get_mesh_greedy();
}

int
StressTest()
{
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
GUITest(const std::string path)
{
    quill::Logger* logger = logging::get_logger();

    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World* world;
    try {
        world = new World(materials_json, path);
    } catch (const std::exception& e) {
        LOG_CRITICAL(logger, "Could not create world!");
        return 1;
    }

    return gui::GUITest(*world);
}

inline int
GUITest(const std::filesystem::path path)
{
    return GUITest(path.string());
}

inline int
LogTest()
{
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
main(int argc, char** argv)
{
    argh::parser cmdl;

    cmdl.add_params({
        "-i", "--path-in",   // Input file - DOC
        "-o", "--path-out",  // Output file - DOC
        "-v", "--verbose",   // Verbosity
        "-c", "--no-console" // Disable console logging
    });
    cmdl.add_param("biome-name");
    cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);

    std::string run_function = cmdl(1).str();
    std::string path_in = cmdl(2).str();
    std::string path_out = cmdl(3).str();

    // init logger
    logging::set_thread_name("MainThread");

    // TODO(nino): need a better arg parser, but allow -vvvv (for example)
    bool console_log = !cmdl[{"-c", "--no-console"}];
    if (cmdl[{"-v", "--verbose"}])
        logging::init(console_log, quill::LogLevel::TraceL3, false);
    else
        logging::init(console_log);

    quill::Logger* logger = logging::get_logger();

    LOG_INFO(logger, "FunGame v{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    LOG_INFO(logger, "Running from {}.", files::get_root_path().string());

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

        if (cmdl[{"-a", "--all"}])
            save_all_terrain(materials_json, biome_data);
        else
            save_terrain(biome_data[biome_name], materials_json, biome_name);
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
