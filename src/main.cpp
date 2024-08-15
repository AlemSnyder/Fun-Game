#include "config.h"
#include "entity/mesh.hpp"
#include "gui/render/graphics_shaders/program_handler.hpp"
#include "gui/scene/controls.hpp"
#include "gui/ui/gui_test.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "logging.hpp"
#include "terrain/generation/biome.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"
#include "util/png_image.hpp"
#include "util/voxel_io.hpp"
#include "world.hpp"

#include <argh.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <glaze/glaze.hpp>
#include <imgui/imgui.h>
#include <png.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define INITIAL_WINDOW_WIDTH  1024
#define INITIAL_WINDOW_HEIGHT 768

constexpr static size_t STRESS_TEST_SIZE = 16;
constexpr static size_t SEED = 5;

constexpr static std::string biome_base_name = "Base";

void
save_terrain(terrain::generation::biome_json_data biome_data) {
    quill::Logger* logger = logging::main_logger;

    LOG_INFO(logger, "Saving {} tile types", biome_data.biome_data.tile_data.size());

    for (MapTile_t i = 0; i < biome_data.biome_data.tile_data.size(); i++) {
        terrain::generation::Biome biome(biome_data, 5);

        MacroDim map_size = 3;
        Dim terrain_height = 128;
        auto macro_map = terrain::generation::Biome::single_tile_type_map(i);
        terrain::Terrain ter(
            map_size, map_size, World::macro_tile_size, terrain_height, 5, biome,
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
    glz::context ctx{};

    std::string biome_name;
    cmdl("biome-name", "-") >> biome_name;
    biome_data = terrain::generation::Biome::get_json_data(biome_name);

    save_terrain(biome_data);

    return 0;
}

int
GenerateTerrain(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", 6) >> size;
    World world(biome_base_name, size, size);

    std::filesystem::path path_out = files::get_argument_path(cmdl(2).str());

    world.qb_save(path_out);

    return 0;
}

int
MacroMap() {
    terrain::generation::Biome biome(biome_base_name, SEED);

    // test terrain generation
    auto map = biome.get_map(64);

    std::vector<TileMacro_t> int_map;
    for (const auto& map_tile : map) {
        int_map.push_back(map_tile.get_tile_type());
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

        return result;

    } else {
        std::filesystem::path lua_file_path = files::get_argument_path(cmdl(2).str());

        std::filesystem::path png_path = files::get_argument_path(cmdl(3).str());

        size_t size;
        cmdl("size", 6) >> size;

        terrain::generation::TerrainMacroMap map =
            terrain::generation::Biome::map_generation_test(lua_file_path, size);

        image::write_result_t result = image::write_image(map, png_path);

        image::log_result(result, png_path);

        return result;
    }

    return 0;
}

int
ChunkDataTest() {
    World world(biome_base_name, 6, 6);

    const terrain::Chunk chunk = world.get_terrain_main().get_chunks()[1];

    terrain::ChunkData chunk_data(chunk);

    for (VoxelDim x = -1; x < terrain::Chunk::SIZE; x++) {
        for (VoxelDim y = -1; y < terrain::Chunk::SIZE; y++) {
            for (VoxelDim z = -1; z < terrain::Chunk::SIZE; z++) {
                MatColorId chunk_mat_color = chunk.get_voxel_color_id(x, y, z);
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

int
save_test(const argh::parser& cmdl) {
    std::filesystem::path path_in = files::get_argument_path(cmdl(2).str());

    std::filesystem::path path_out = files::get_argument_path(cmdl(3).str());

    size_t seed;
    cmdl("seed", SEED) >> seed;
    World world(biome_base_name, path_in, seed);

    world.qb_save_debug(path_out);

    return 0;
}

int
path_finder_test(const argh::parser& cmdl) {
    std::filesystem::path path_in = files::get_argument_path(cmdl(2).str());

    std::filesystem::path path_out = files::get_argument_path(cmdl(3).str());
    quill::Logger* logger = logging::main_logger;

    size_t seed;
    cmdl("seed", SEED) >> seed;
    World world(biome_base_name, path_in, seed);

    auto start_end = world.get_terrain_main().get_start_end_test();

    LOG_INFO(
        logger, "Start: {}, {}, {}", start_end.first->get_x(), start_end.first->get_y(),
        start_end.first->get_z()
    );

    LOG_INFO(
        logger, "End: {}, {}, {}", start_end.second->get_x(), start_end.second->get_y(),
        start_end.second->get_z()
    );

    auto tile_path =
        world.get_terrain_main().get_path_Astar(start_end.first, start_end.second);

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
    const terrain::Material* path_mat = world.get_material(DEBUG_MATERIAL);
    for (const terrain::Tile* tile : tile_path.value()) {
        world.get_terrain_main()
            .get_tile(world.get_terrain_main().pos(tile))
            ->set_material(path_mat, path_color_id);
    }

    world.qb_save(path_out);

    return 0;
}

int
imgui_entry_main(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", 2) >> size;
    // Create world object from material data, biome data, and the number of
    // chunks in the x,y direction. Here the size is 2,2.
    World world(biome_base_name, size, size, seed);

    return gui::imgui_entry(world);
}

int
StressTest(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", STRESS_TEST_SIZE) >> size;
    World world(biome_base_name, size, size, seed);
    // Create world object from material data, biome data, and the number of
    // chunks in the x,y direction. Here the size is a user parameter that
    // defaults to STRESS_TEST_SIZE.

    return gui::opengl_entry(world);
}

int
opengl_entry(const argh::parser& cmdl) {
    std::filesystem::path path_in = files::get_argument_path(cmdl(2).str());

    size_t seed;
    cmdl("seed", SEED) >> seed;

    World world(biome_base_name, path_in, seed);

    return gui::opengl_entry(world);
}

inline int
LogTest() {
    LOG_BACKTRACE(logging::terrain_logger, "Backtrace log {}", 1);
    LOG_BACKTRACE(logging::terrain_logger, "Backtrace log {}", 2);

    LOG_INFO(logging::terrain_logger, "Welcome to Quill!");
    LOG_ERROR(logging::terrain_logger, "An error message. error code {}", 123);
    LOG_WARNING(logging::terrain_logger, "A warning message.");
    LOG_CRITICAL(logging::terrain_logger, "A critical error.");
    LOG_DEBUG(logging::terrain_logger, "Debugging foo {}", 1234);
    LOG_TRACE_L1(logging::terrain_logger, "{:>30}", "right aligned");
    LOG_TRACE_L2(
        logging::terrain_logger, "Positional arguments are {1} {0} ", "too", "supported"
    );
    LOG_TRACE_L3(logging::terrain_logger, "Support for floats {:03.2f}", 1.23456);

    LOG_INFO(
        logging::lua_logger, "Using Lua logger. The lua logger should not log the cpp "
                             "file, but instead the lua file."
    );

    LOG_INFO(
        logging::lua_logger, "[{}.lua:{}] - This is what a lua log should look like.",
        "example_file", 37
    );

    return 0;
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
    cmdl.add_param("materials");
    cmdl.add_param("seed");
    cmdl.add_param("size");
    cmdl.parse(argc, argv, argh::parser::SINGLE_DASH_IS_MULTIFLAG);

    std::string run_function = cmdl(1).str();

    // init logger
    logging::set_thread_name("MainThread");

    // TODO(nino): need a better arg parser, but allow -vvvv (for example)
    bool console_log = cmdl[{"-c", "--console"}];
    if (cmdl[{"-v", "--verbose"}])
        logging::init(console_log, quill::LogLevel::TraceL3);
    else
        logging::init(console_log);

    quill::Logger* logger = logging::main_logger;

    LOG_INFO(logger, "FunGame v{}.{}.{}", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    LOG_INFO(logger, "Running from {}.", files::get_root_path().string());

    if (argc == 1) {
        return 1;
    } else if (run_function == "TerrainTypes") {
        return TerrainTypes(cmdl);
    } else if (run_function == "GenerateTerrain") {
        return GenerateTerrain(cmdl);
    } else if (run_function == "MacroMap" || run_function == "LuaTest") {
        return MacroMap();
    } else if (run_function == "NoiseTest") {
        return NoiseTest();
    } else if (run_function == "StressTest") {
        return StressTest(cmdl);
    } else if (run_function == "SaveTest") {
        return save_test(cmdl);
    } else if (run_function == "PathFinder") {
        return path_finder_test(cmdl);
    } else if (run_function == "UI-opengl") {
        return opengl_entry(cmdl);
    } else if (run_function == "Logging") {
        return LogTest();
    } else if (run_function == "UI-imgui") {
        return imgui_entry_main(cmdl);
    } else if (run_function == "ChunkDataTest") {
        return ChunkDataTest();
    } else if (run_function == "imageTest") {
        return image_test(cmdl);
    } else if (run_function == "RefactoredProgramTest") {
        return gui::revised_gui_test();
    } else if (run_function == "StarsTest") {
        return gui::stars_test();
    } else {
        std::cout << "No known command" << std::endl;
        return 0;
    }
}
