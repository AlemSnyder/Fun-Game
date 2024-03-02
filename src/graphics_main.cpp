#include "graphics_main.hpp"

#include "util/loading.hpp"
#include "gui/ui/opengl_setup.hpp"
#include "types.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

/*
struct Options {
    std::optional<std::string> biome;

    std::optional<std::filesystem::path> save_file;
};*/




int graphics_main(const argh::parser& cmdl){
    // add window width and height?

    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;
    // init graphics

    std::optional<GLFWwindow*> opt_window = gui::setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    GLFWwindow* window = opt_window.value();
    gui::setup_opengl_logging();


    

    std::string run_function = cmdl(1).str();

    // Think about this later
    // need to read biomes from manifest
    // then in background need to mesh things
    // then send those things back to main thread
    // the main thread loads meshes onto gpu and renders to screen
    // Read manifest
    util::load_manifest();

    // if need gui start gui

    // struct Options {} default options
    //Options options;
    // modify based on values
    if (run_function == "UI-opengl"){
        
    }

    // if don't then strate to gui

    return 0;

}