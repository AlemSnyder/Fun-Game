#include "config.h"
#include "entity/mesh.hpp"
#include "gui/scene/controls.hpp"
#include "gui/shader.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "logging.hpp"
#include "terrain/generation/biome.hpp"
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
// #include <png++/png.hpp>
#include <png.h>
#include <stdint.h>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#define INITIAL_WINDOW_WIDTH  1024
#define INITIAL_WINDOW_HEIGHT 768

constexpr static size_t SEED = 5;

void
save_terrain(
    Json::Value materials_json, Json::Value biome_data, std::string biome_name
) {
    quill::Logger* logger = quill::get_logger();

    LOG_INFO(logger, "Saving {} tile types", biome_data["Tile_Data"].size());

    terrain::generation::biome_json_data biome_file_data{
        biome_name, materials_json, biome_data
    };
    for (MapTile_t i = 0; i < biome_data["Tile_Data"].size(); i++) {
        terrain::generation::Biome biome(biome_file_data, 5);

        MacroDim map_size = 3;
        Dim terrain_height = 128;
        std::vector<terrain::generation::MapTile> macro_map = get_test_map(i);
        terrain::Terrain ter(
            map_size, map_size, World::macro_tile_size, terrain_height, 5, biome,
            macro_map
        );

        std::filesystem::path save_path = files::get_root_path() / "SavedTerrain";
        save_path /= biome_name;
        save_path /= "biome_";
        save_path += std::to_string(i);
        save_path += ".qb";
        ter.qb_save(save_path.string());
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
TerrainTypes(const argh::parser& cmdl) {
    Json::Value biome_data;
    std::string biome_name;

    cmdl("biome-name", "-") >> biome_name;
    std::filesystem::path biome_data_file = biome_name;
    biome_data_file += ".json";
    auto biome_file = files::open_data_file(biome_data_file);
    if (biome_file.has_value())
        biome_file.value() >> biome_data;
    else {
        LOG_CRITICAL(
            logging::file_io_logger, "Could not open biome data {}", biome_data_file
        );
        return 1;
    }
    std::string material_file;

    Json::Value materials_json;
    cmdl("materials", "-") >> material_file;
    std::filesystem::path material_data_file = material_file;
    material_data_file += ".json";
    auto materials_file = files::open_data_file(material_data_file);
    if (materials_file.has_value())
        materials_file.value() >> materials_json;
    else {
        LOG_CRITICAL(
            logging::file_io_logger, "Could not open material data {}", material_file
        );
        return 1;
    }

    if (cmdl[{"-a", "--all"}])
        save_all_terrain(materials_json, biome_data);
    else
        save_terrain(biome_data[biome_name], materials_json, biome_name);

    return 0;
}

int
GenerateTerrain(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", 6) >> size;
    World world("base", size, size);

    std::string path_out = cmdl(2).str();

    world.qb_save(path_out);

    return 0;
}

int
MacroMap() {
    quill::Logger* logger = quill::get_logger();

    terrain::generation::Biome biome("base", 2);

    // test terrain generation
    auto map = biome.get_map(64);

    std::vector<TileMacro_t> int_map;
    for (const auto& map_tile : map) {
        int_map.push_back(map_tile.get_tile_type());
    }

    LOG_INFO(logger, "Map: {}", int_map);

    return 0;
}

int
image_test() {
    // png::image<png::rgb_pixel> image(16,16);

    std::filesystem::path terrain_png =
        files::get_root_path() / "terrain_test" / "test.png";

    std::FILE* file = fopen(terrain_png.string().data(), "wb");
    // std::FILE fp (file);
    //  TODO add tests on validity of file & path

    // god a hate c. How is one supposed to know this is a pointer?
    // TODO these nullptr should be function pointers
    png_structp png_ptr =
        png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (png_ptr == nullptr) {
        return 1;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        return 1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(file);
        return 1;
    }

    png_init_io(png_ptr, file);

    size_t WIDTH = 1000;
    size_t HEIGHT = 1000;

    png_set_IHDR(
        png_ptr, info_ptr, WIDTH, HEIGHT, 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    png_text meta_data;
    meta_data.compression = PNG_TEXT_COMPRESSION_NONE;
    meta_data.lang_key = "en";
    meta_data.key = "An Image";
    meta_data.text = "Some text";
    png_set_text(png_ptr, info_ptr, &meta_data, 1);
    png_write_info(png_ptr, info_ptr);

    // png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, nullptr);

    // std::vector<png_byte*> row_pointers(HEIGHT);

    png_bytep row = (png_bytep)malloc(WIDTH * sizeof(png_byte));
    for (size_t i = 0; i < HEIGHT; i++) {
        // TODO free this memory
        for (size_t j = 0; j < WIDTH; j++) {
            row[j] = i*j;
        }
        // row_pointers[i] = row;
        png_write_row(png_ptr, row);
    }
    free(row);

    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // for (size_t i = 0; i < HEIGHT; i++) {
    //     free(row_pointers[i]);
    // }

    return 0;
}

int
ChunkDataTest() {
    World world("base", 6, 6);

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
    quill::Logger* logger = quill::get_logger();

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
    std::string path_in = cmdl(2).str();
    std::string path_out = cmdl(3).str();
    size_t seed;
    cmdl("seed", SEED) >> seed;
    World world("base", path_in, seed);

    world.qb_save_debug(path_out);

    return 0;
}

int
path_finder_test(const argh::parser& cmdl) {
    std::string path_in = cmdl(2).str();
    std::string path_out = cmdl(3).str();
    quill::Logger* logger = quill::get_logger();
    size_t seed;
    cmdl("seed", SEED) >> seed;
    World world("base", path_in, seed);

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
    World world("base", size, size, seed);

    return gui::imgui_entry(world);
}

int
StressTest(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", 2) >> size;
    World world("base", size, size, seed);
    // Create world object from material data, biome data, and the number of
    // chunks in the x,y direction. Here the size is 2,2.

    return gui::opengl_entry(world);
}

int
opengl_entry(const argh::parser& cmdl) {
    std::string path_in = cmdl(2).str();

    size_t seed;
    cmdl("seed", SEED) >> seed;

    World world("base", path_in, seed);

    return gui::opengl_entry(world);
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
        logging::init(console_log, quill::LogLevel::TraceL3, false);
    else
        logging::init(console_log);

    quill::Logger* logger = logging::get_logger();

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
        return image_test();
    } else {
        std::cout << "No known command" << std::endl;
        return 0;
    }
}
