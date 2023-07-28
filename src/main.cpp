#include "config.h"
#include "entity/mesh.hpp"
#include "gui/scene/controls.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/shader.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "logging.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"
#include "util/voxel_io.hpp"
#include "world.hpp"

#include <argh.h>
#include <json/json.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <imgui/imgui.h>
#include <quill/Quill.h>
#include <stdint.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define INITIAL_WINDOW_WIDTH  1024
#define INITIAL_WINDOW_HEIGHT 768

int
GenerateTerrain(const std::string path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    World world(materials_json, biome_data, 6, 6);

    world.qb_save(path);

    return 0;
}

int
MacroMap() {
    quill::Logger* logger = quill::get_logger();

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    // test terrain generation in a region of 64 by 64
    auto map = terrain::TerrainBase::generate_macro_map(
        64, 64, biome_data["Biome_1"]["Terrain_Data"]
    );

    LOG_INFO(logger, "Map: {}", map);

    return 0;
}

int
save_test(const std::string path, const std::string save_path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

    world.qb_save_debug(save_path);

    return 0;
}

void
save_terrain(
    Json::Value materials_json, Json::Value biome_data, std::string biome_name
) {
    quill::Logger* logger = quill::get_logger();

    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    LOG_INFO(logger, "Saving {} tile types", biome_data["Tile_Data"].size());

    for (unsigned int i = 0; i < biome_data["Tile_Data"].size(); i++) {
        World world(materials_json, biome_data, i);
        std::filesystem::path save_path = files::get_root_path() / "SavedTerrain";
        save_path /= biome_name;
        save_path /= "biome_";
        save_path += std::to_string(i);
        save_path += ".qb";
        world.qb_save(save_path.string());
    }
}

void
save_all_terrain(const Json::Value& materials_json, const Json::Value& biome_data) {
    for (auto biome_type = biome_data.begin(); biome_type != biome_data.end();
         biome_type++) {
        save_terrain(materials_json, *biome_type, biome_type.key().asString());
    }
}

int
path_finder_test(const std::string& path, const std::string& save_path) {
    quill::Logger* logger = quill::get_logger();

    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

    auto start_end = world.get_terrain_main().get_start_end_test();

    LOG_INFO(
        logger, "Start: {}, {}, {}", start_end.first->get_x(), start_end.first->get_y(),
        start_end.first->get_z()
    );

    LOG_INFO(
        logger, "End: {}, {}, {}", start_end.second->get_x(), start_end.second->get_y(),
        start_end.second->get_z()
    );

    std::vector<const terrain::Tile*> tile_path =
        world.get_terrain_main().get_path_Astar(start_end.first, start_end.second);

    LOG_INFO(logger, "Path length: {}", tile_path.size());

    if (tile_path.size() == 0) {
        LOG_INFO(logger, "No path");
        world.qb_save_debug(save_path);
        return 1;
    }

    for (const terrain::Tile* tile : tile_path) {
        world.get_terrain_main()
            .get_tile(world.get_terrain_main().pos(tile))
            ->set_material(&world.get_materials()->at(7), 5);
    }

    world.qb_save(save_path);

    return 0;
}

int
imgui_entry_main() {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    // Create world object from material data, biome data, and the number of
    // chunks in the x,y direction. Here the size is 2,2.
    World world(materials_json, biome_data, 2, 2);

    return gui::imgui_entry(world);
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

    return gui::opengl_entry(world);
}

int
opengl_entry(const std::string& path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    World world(materials_json, path);

    return gui::opengl_entry(world);
}

inline int
opengl_entry(const std::filesystem::path& path) {
    return opengl_entry(path.string());
}

inline int
LogTest() {
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

    cmdl.add_params({
        "-v", "--verbose", // Verbosity
        "-c", "--console"  // Enable console logging
    });
    cmdl.add_param("biome-name");
    cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);

    std::string run_function = cmdl(1).str();
    std::string path_in = cmdl(2).str();
    std::string path_out = cmdl(3).str();

    // init logger
    logging::set_thread_name("MainThread");

    // TODO(nino): need a better arg parser, but allow -vvvv (for example)
    bool console_log = cmdl[{"-c", "--console"}];
    if (cmdl[{"-v", "--verbose"}])
        logging::init(console_log, quill::LogLevel::TraceL3, false);
    else
        logging::init(console_log);

    quill::Logger* logger = logging::get_logger();

    LOG_INFO(logger, "FunGame v{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    LOG_INFO(logger, "Running from {}.", files::get_root_path().string());

    if (argc == 1) {
        return opengl_entry(files::get_data_path() / "models" / "DefaultTree.qb");
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
    } else if (run_function == "UI-opengl") {
        return opengl_entry(path_in);
    } else if (run_function == "Logging") {
        return LogTest();
    } else if (run_function == "UI-imgui") {
        return imgui_entry_main();
    } else {
        std::cout << "No known command" << std::endl;
        return 0;
    }
}
