#include "graphics_main.hpp"

#include "gui/handler.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "gui/ui/opengl_setup.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "util/loading.hpp"
#include "world/world.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

/*
struct Options {
    std::optional<std::string> biome;

    std::optional<std::filesystem::path> save_file;
};*/

constexpr static size_t STRESS_TEST_SIZE = 16;
constexpr static size_t SEED = 5;

int
graphics_main(const argh::parser& cmdl) {
    // add window width and height?

    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;
    // init graphics

    std::optional<GLFWwindow*> opt_window =
        gui::setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    GLFWwindow* window = opt_window.value();
    gui::setup_opengl_logging();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    gui::VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);

    std::string run_function = cmdl(1).str();

    // Think about this later
    // need to read biomes from manifest
    // then in background need to mesh things
    // then send those things back to main thread
    // the main thread loads meshes onto gpu and renders to screen
    // Read manifest
    util::load_manifest();

    // struct Options {} default options
    // Options options;
    // modify based on values

    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", STRESS_TEST_SIZE) >> size;
    std::string biome_name;
    cmdl("biome-name", "base") >> biome_name;

    world::World world(biome_name, size, size, seed);

    // if need gui start gui
    bool imgui_debug = cmdl[{"-g", "--imgui"}];
    if (imgui_debug) {
        return gui::imgui_entry(world, window);
    } else {
        return gui::opengl_entry(world, window);
    }

    // if don't then strate to gui

    // glDeleteVertexArrays(1, &VertexArrayID);

    return 0;
}

int
graphics_main() {
    LOG_CRITICAL(logging::main_logger, "NOT IMPLEMENTED");

    return 1;
}