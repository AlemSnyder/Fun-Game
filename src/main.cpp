#include "config.h"
#include "graphics_main.hpp"
#include "gui/render/graphics_shaders/program_handler.hpp"
#include "gui/scene/controls.hpp"
#include "gui/tests.hpp"
#include "gui/ui/gui_test.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "util/files.hpp"
#include "util/loading.hpp"
#include "util/lua/lua_logging.hpp"
#include "util/png_image.hpp"
#include "util/time.hpp"
#include "util/voxel_io.hpp"
#include "world/biome.hpp"
#include "world/entity/mesh.hpp"
#include "world/terrain/generation/terrain_map.hpp"
#include "world/terrain/terrain.hpp"
#include "world/world.hpp"

#include <argh.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#include <glaze/glaze.hpp>
#pragma clang diagnostic pop

#include <imgui/imgui.h>
#include <png.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

void
save_terrain(terrain::generation::biome_json_data biome_data) {
    quill::Logger* logger = logging::main_logger;

    LOG_INFO(logger, "Saving {} tile types", biome_data.biome_data.tile_data.size());

    for (MapTile_t i = 0; i < biome_data.biome_data.tile_data.size(); i++) {
        terrain::generation::Biome biome(biome_data, 5);

        MacroDim map_size = 3;
        Dim terrain_height = 128;
        auto macro_map = biome.single_tile_type_map(i);
        terrain::Terrain ter(
            map_size, map_size, world::World::macro_tile_size, terrain_height, biome,
            macro_map
        );

        std::filesystem::path save_path = files::get_root_path() / "SavedTerrain";
        save_path /= biome_data.biome_name;
        save_path /= "biome_";
        save_path += std::to_string(i);
        save_path += ".qb";
        ter.qb_save(save_path.string());
    }
}

int
TerrainTypes(const argh::parser& cmdl) {
    terrain::generation::biome_json_data biome_data;

    std::string biome_name;
    cmdl("biome-name", "-") >> biome_name;
    biome_data =
        terrain::generation::Biome::get_json_data(files::get_data_path() / biome_name);

    save_terrain(biome_data);

    return 0;
}

// reimplement
int
GenerateTerrain(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", 6) >> size;
    world::World world(BIOME_BASE_NAME, size, size, seed);

    std::filesystem::path path_out = files::get_argument_path(cmdl(3).str());

    world.qb_save(path_out);

    return 0;
}

int
MacroMap(const argh::parser& cmdl) {
    std::string biome_name;
    cmdl("biome-name", BIOME_BASE_NAME) >> biome_name;
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", 4) >> size;

    terrain::generation::Biome biome(biome_name, seed);

    // test terrain generation
    auto map = biome.get_map(size);

    std::vector<TileMacro_t> int_map;
    for (const auto& map_tile : map) {
        int_map.push_back(map_tile.get_type_id());
    }

    LOG_INFO(logging::main_logger, "Map: {}", int_map);

    return 0;
}

int
image_test(const argh::parser& cmdl) {
    if (cmdl.size() < 2) {
        // png::image<png::rgb_pixel> image(16,16);

        std::filesystem::path png_path =
            files::get_root_path() / "terrain_output_data" / "test.png";

        image::ImageTest image;

        image::write_result_t result = image::write_image(image, png_path);

        image::log_result(result, png_path);

        std::filesystem::path color_png_path =
            files::get_root_path() / "terrain_output_data" / "color_test.png";

        image::ColorImageTest color_image;

        result = image::write_image(color_image, color_png_path);

        image::log_result(result, color_png_path);

        return result;

    } else {
        std::string biome_name;
        cmdl("biome-name", BIOME_BASE_NAME) >> biome_name;
        size_t seed;
        cmdl("seed", SEED) >> seed;
        size_t size;
        cmdl("size", 64) >> size;

        terrain::generation::Biome biome(biome_name, seed);

        auto map = biome.get_map(size);

        std::filesystem::path png_save_path = files::get_argument_path(cmdl(3).str());

        if (!(map.get_height() == size)) {
            LOG_ERROR(logging::game_map_logger, "Error generating map.");
            return 1;
        }

        auto map_rep = terrain::generation::TerrainMapRepresentation(map);

        image::write_result_t result = image::write_image(map_rep, png_save_path);

        image::log_result(result, png_save_path);

        return result;
    }

    return 0;
}

// reimplement
int
ChunkDataTest() {
    world::World world(BIOME_BASE_NAME, 6, 6);

    const terrain::Chunk* chunk = world.get_terrain_main().get_chunk({0, 0, 0});

    if (!chunk)
        return 1;

    terrain::ChunkData chunk_data(*chunk);

    for (VoxelDim x = -1; x < terrain::Chunk::SIZE; x++) {
        for (VoxelDim y = -1; y < terrain::Chunk::SIZE; y++) {
            for (VoxelDim z = -1; z < terrain::Chunk::SIZE; z++) {
                MatColorId chunk_mat_color = chunk->get_voxel_color_id(x, y, z);
                MatColorId chunk_data_mat_color_id =
                    chunk_data.get_voxel_color_id(x, y, z);
                if (chunk_mat_color != chunk_data_mat_color_id) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int
NoiseTest() {
    quill::Logger* logger = logging::main_logger;

    terrain::generation::FractalNoise noise(1, 1, 3);

    LOG_INFO(logger, "Noise double: {}", noise.get_noise(1, 1));
    LOG_INFO(logger, "Noise double again: {}", noise.get_noise(1, 1));
    LOG_INFO(logger, "Noise double: {}", noise.get_noise(2, 1));
    LOG_INFO(logger, "Noise double: {}", noise.get_noise(3, 1));
    LOG_INFO(logger, "Noise double: {}", noise.get_noise(4, 1));

    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(1, 3, 3)
    );
    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(2, 3, 3)
    );
    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(3, 3, 3)
    );
    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(4, 3, 3)
    );
    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(5, 3, 3)
    );
    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(6, 3, 3)
    );
    LOG_INFO(
        logger, "Random double: {}", terrain::generation::Noise::get_double(7, 3, 3)
    );

    return 0;
}

// reimplement
int
save_test(const argh::parser& cmdl) {
    std::filesystem::path path_in = files::get_argument_path(cmdl(2).str());

    std::filesystem::path path_out = files::get_argument_path(cmdl(3).str());

    size_t seed;
    cmdl("seed", SEED) >> seed;
    util::load_manifest();
    world::World world(BIOME_BASE_NAME, path_in, seed);

    world.qb_save_debug(path_out);

    return 0;
}

// reimplement
int
path_finder_test(const argh::parser& cmdl) {
    std::filesystem::path path_in = files::get_argument_path(cmdl(3).str());

    std::filesystem::path path_out = files::get_argument_path(cmdl(4).str());
    quill::Logger* logger = logging::main_logger;

    size_t seed;
    cmdl("seed", SEED) >> seed;

    std::string biome_name;
    cmdl("biome-name", BIOME_BASE_NAME) >> biome_name;
    util::load_manifest();
    world::World world(biome_name, path_in, seed);

    auto start_end = world.get_terrain_main().get_start_end_test();

    TerrainOffset3 start = start_end.first;
    TerrainOffset3 end = start_end.second;

    LOG_INFO(logger, "Start: {}, {}, {}", start.x, start.y, start.z);

    LOG_INFO(logger, "End: {}, {}, {}", end.x, end.y, end.z);

    auto tile_path = world.get_terrain_main().get_path_Astar(start, end);

    if (!tile_path) {
        LOG_WARNING(logger, "NO PATH FOUND");
        world.qb_save_debug(path_out);
        return 1;
    }

    LOG_INFO(logger, "Path length: {}", tile_path.value().size());

    if (tile_path.value().size() == 0) {
        LOG_WARNING(logger, "NO PATH FOUND");
        world.qb_save_debug(path_out);
        return 1;
    }

    constexpr ColorId path_color_id = 5;
    const terrain::material_t* path_mat = world.get_material(DEBUG_MATERIAL);
    for (const TerrainOffset3 tile : tile_path.value()) {
        world.get_terrain_main().get_tile(tile)->set_material(path_mat, path_color_id);
    }

    world.qb_save(path_out);

    return 0;
}

int
path_finder_test() {
    quill::Logger* logger = logging::main_logger;

    // util::load_manifest();
    world::World world(BIOME_BASE_NAME, 4, 4, SEED);

    TerrainOffset3 start(20, 20, world.get_terrain_main().get_Z_solid(20, 20) + 1);
    TerrainOffset3 end(90, 90, world.get_terrain_main().get_Z_solid(90, 90) + 1);

    LOG_INFO(logger, "Start: {}, {}, {}", start.x, start.y, start.z);

    LOG_INFO(logger, "End: {}, {}, {}", end.x, end.y, end.z);

    auto tile_path = world.get_terrain_main().get_path_Astar(start, end);

    if (!tile_path) {
        LOG_WARNING(logger, "NO PATH FOUND");
        return 1;
    }

    LOG_INFO(logger, "Path length: {}", tile_path.value().size());

    if (tile_path.value().size() == 0) {
        LOG_WARNING(logger, "NO PATH FOUND");
        return 1;
    }

    return 0;
}

inline int
LogTest() {
    LOG_BACKTRACE(logging::terrain_logger, "Backtrace log {}", 1);
    LOG_BACKTRACE(logging::terrain_logger, "Backtrace log {}", 2);

    LOG_INFO(logging::main_logger, "Welcome to Quill!");
    LOG_ERROR(logging::terrain_logger, "An error message. error code {}", 123);
    LOG_WARNING(logging::terrain_logger, "A warning message.");
    LOG_CRITICAL(logging::terrain_logger, "A critical error.");
    LOG_DEBUG(logging::terrain_logger, "Debugging foo {}", 1234);
    LOG_TRACE_L1(logging::terrain_logger, "{:>30}", "right aligned");
    LOG_TRACE_L2(
        logging::terrain_logger, "Positional arguments are {1} {0} ", "too", "supported"
    );
    LOG_TRACE_L3(logging::terrain_logger, "Support for floats {:03.2f}", 1.23456);

    GlobalContext& context = GlobalContext::instance();

    auto future = context.submit_task([]() {
        LOG_INFO(logging::main_logger, "Log from backend thread");
    });

    LOG_INFO(
        logging::lua_logger, "Using Lua logger. The lua logger should not log the cpp "
                             "file, but instead the lua file."
    );

    LOG_INFO(
        logging::lua_logger, "[{}.lua:{}] - This is what a lua log should look like.",
        "example_file", 37
    );

    future.wait();

    return 0;
}

int
lua_log_test() {
    auto& lua = LocalContext::get_lua_state();

    sol::protected_function lua_log_critical = lua["Logging"]["LOG_CRITICAL"];

    auto result = lua_log_critical.call("Critical message from cpp!!");

    if (!result.valid()) {
        sol::error err = result; // who designed this?
        std::string what = err.what();
        LOG_DEBUG(logging::main_logger, "{}", what);
    }

    return 0;
}

int
lua_loadtime_test() {
    LOG_INFO(logging::main_logger, "Getting Local Lua State.");
    auto& lua = LocalContext::get_lua_state();
    LOG_INFO(logging::main_logger, "Got Local Lua State.");
    LOG_INFO(logging::main_logger, "Loading Lua File.");

    std::filesystem::path lua_script_path =
        files::get_resources_path() / "lua" / "is_prime_test.lua";

    sol::table result = lua.require_file("test", lua_script_path.string());

    if (!result.valid()) {
        LOG_WARNING(logging::main_logger, "is prime test failed to import.");
        return 1;
    }

    for (const auto& key : result) {
        if (key.first.is<std::string>()) {
            std::string string_key = key.first.as<std::string>();
            LOG_INFO(logging::main_logger, "{}", string_key);
        }
    }

    {
        sol::protected_function is_prime_function = result["tests"]["is_prime"];

        LOG_INFO(logging::main_logger, "Got Lua function, calling.");

        auto function_result = is_prime_function.call(97);

        if (!function_result.valid()) {
            sol::error err = function_result;
            std::string what = err.what();
            LOG_DEBUG(logging::main_logger, "{}", what);
            return 1;
        }

        int is_prime_result = function_result;

        std::string log_value = is_prime_result == 1 ? "correct" : "incorrect";
        LOG_INFO(
            logging::main_logger, "Got Lua {} from Lua function. Is {} value.",
            is_prime_result, log_value
        );
    }

    {
        std::vector<std::chrono::nanoseconds> load_times;
        std::vector<std::chrono::nanoseconds> run_times;

        for (size_t y = 0; y < 100; y++) {
            auto l_start = time_util::get_time_nanoseconds();

            std::filesystem::path prime_test_file_path =
                files::get_resources_path() / "lua" / "is_prime_test.lua";

            sol::table biome_library =
                lua.require_file("is_prime_test", prime_test_file_path.string(), false);

            sol::protected_function is_prime_function =
                biome_library["tests"]["is_prime"];

            auto l_end = time_util::get_time_nanoseconds();
            load_times.push_back(l_end - l_start);

            auto r_start = time_util::get_time_nanoseconds();

            auto function_result = is_prime_function.call(97);

            if (!function_result.valid()) {
                sol::error err = function_result;
                std::string what = err.what();
                LOG_DEBUG(logging::main_logger, "{}", what);
                return 1;
            }

            auto r_end = time_util::get_time_nanoseconds();

            run_times.push_back(r_end - r_start);
        }

        std::chrono::nanoseconds r_mean(0);
        for (const auto& duration : run_times) {
            r_mean += duration;
        }
        r_mean /= run_times.size();

        std::chrono::nanoseconds l_mean(0);
        for (const auto& duration : load_times) {
            l_mean += duration;
        }
        l_mean /= run_times.size();

        LOG_INFO(
            logging::main_logger, "Mean load time of {} samples is {}ns.",
            load_times.size(), int64_t(l_mean.count())
        );

        LOG_INFO(
            logging::main_logger, "Mean execution time of {} samples is {}ns.",
            run_times.size(), int64_t(r_mean.count())
        );
    }

    return 0;
}

int
lua_transfertime_test() {
    LOG_INFO(logging::main_logger, "Loading Lua File.");

    std::filesystem::path lua_script_path =
        files::get_resources_path() / "lua" / "is_prime_test.lua";

    GlobalContext& context = GlobalContext::instance();
    context.load_script_file(lua_script_path);

    {
        std::optional<sol::object> result = context.get_from_lua("tests\\is_prime");

        if (!result) {
            return 0;
        }
        if (!result->is<sol::protected_function>()) {
            return 0;
        }

        sol::protected_function is_prime_function =
            result->as<sol::protected_function>();

        auto function_result = is_prime_function.call(97);

        if (!function_result.valid()) {
            sol::error err = function_result;
            std::string what = err.what();
            LOG_DEBUG(logging::main_logger, "{}", what);
            return 1;
        }

        int is_prime_result = function_result;

        std::string log_value = is_prime_result == 1 ? "correct" : "incorrect";
        LOG_INFO(
            logging::main_logger, "Got Lua {} from Lua function. Is {} value.",
            is_prime_result, log_value
        );
    }

    std::future<int> future_result = context.submit_task([]() {
        
            std::vector<std::chrono::nanoseconds> load_times;
            std::vector<std::chrono::nanoseconds> run_times;

            for (size_t y = 0; y < 100; y++) {
                auto l_start = time_util::get_time_nanoseconds();

                LocalContext& local_context = LocalContext::instance();

                std::optional<sol::object> result = local_context.get_from_lua("tests\\is_prime");

                if (!result) {
                    return 0;
                }
                if (!result->is<sol::protected_function>()) {
                    return 0;
                }

                sol::protected_function is_prime_function =
                    result->as<sol::protected_function>();


                auto l_end = time_util::get_time_nanoseconds();

                load_times.push_back(l_end - l_start);

                auto r_start = time_util::get_time_nanoseconds();

                auto function_result = is_prime_function.call(97);

                if (!function_result.valid()) {
                    sol::error err = function_result;
                    std::string what = err.what();
                    LOG_DEBUG(logging::main_logger, "{}", what);
                    return 1;
                }

                auto r_end = time_util::get_time_nanoseconds();

                run_times.push_back(r_end - r_start);
            }

            std::chrono::nanoseconds r_mean(0);
            for (const auto& duration : run_times) {
                r_mean += duration;
            }
            r_mean /= run_times.size();

            std::chrono::nanoseconds l_mean(0);
            for (const auto& duration : load_times) {
                l_mean += duration;
            }
            l_mean /= run_times.size();

            LOG_INFO(
                logging::main_logger, "Mean load time of {} samples is {}ns.",
                load_times.size(), int64_t(l_mean.count())
            );

            LOG_INFO(
                logging::main_logger, "Mean execution time of {} samples is {}ns.",
                run_times.size(), int64_t(r_mean.count())
            );
        
        return 0;
    });

    future_result.wait();

    int subprocess_status = future_result.get();

    return subprocess_status;

}

// for tests. Probably should make a bash script to test each test
inline int
tests(const argh::parser& cmdl) {
    std::string run_function = cmdl(2).str();

    if (run_function == "TerrainTypes") {
        return TerrainTypes(cmdl);
    } else if (run_function == "GenerateTerrain") {
        return GenerateTerrain(cmdl);
    } else if (run_function == "MacroMap" || run_function == "LuaTest") {
        return MacroMap(cmdl);
    } else if (run_function == "NoiseTest") {
        return NoiseTest();
    } else if (run_function == "SaveTest") {
        return save_test(cmdl);
    } else if (run_function == "PathFinder") {
        return path_finder_test(cmdl);
    } else if (run_function == "PathFinderTest") {
        return path_finder_test();
    } else if (run_function == "Logging") {
        return LogTest();
    } else if (run_function == "ChunkDataTest") {
        return ChunkDataTest();
    } else if (run_function == "imageTest") {
        return image_test(cmdl);
    } else if (run_function == "LoadManifest") {
        return util::load_manifest_test();
    } else if (run_function == "EnginTest") {
        return gui::opengl_tests();
    } else if (run_function == "LuaLogTest") {
        return lua_log_test();
    } else if (run_function == "LuaLoadTimeTest") {
        return lua_loadtime_test();
    } else if (run_function == "LuaTransferTimeTest") {
        return lua_transfertime_test();
    } else {
        std::cout << "No known command" << std::endl;
        return 1;
    }
}

int
main(int argc, char** argv) {
    // #lizard forgives the complexity
    // Because iff else over command line args
    argh::parser cmdl;

    cmdl.add_params({
        "-v", "--verbose", // Verbosity
        "-c", "--console"  // Enable console logging
    });
    cmdl.add_param("biome-name");
    //    cmdl.add_param("materials"); materials should be dictated by biome

    cmdl.add_params({"--imgui", "-g"});
    // int seed for generation
    cmdl.add_param("seed");
    // int size of map
    cmdl.add_param("size");
    cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);

    std::string start_type = cmdl(1).str();

    // TODO(nino): need a better arg parser, but allow -vvvv (for example)
    bool console_log = cmdl[{"-c", "--console"}];
    if (cmdl[{"-v", "--verbose"}])
        logging::init(console_log, quill::LogLevel::TraceL3);
    else
        logging::init(console_log);

    quill::Logger* logger = logging::main_logger;

    LOG_INFO(logger, "FunGame v{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    LOG_INFO(logger, "Running from {}.", files::get_root_path().string());

    // main thread for opengl and lua loading
    GlobalContext& context = GlobalContext::instance();
    context.set_main_thread();

    assert(context.is_main_thread() && "This is not the main thread. Strange :(");

    if (argc == 1) {
        return graphics_main();
    } else if (start_type == "Test") {
        int return_status = tests(cmdl);
        logging::flush();
        return return_status;
    } else if (start_type == "Start") {
        return graphics_main(cmdl);
    } else {
        std::cout << "Old command line arguments don't work. Try adding \"Test\"."
                  << std::endl;
        return 1;
    }
}
