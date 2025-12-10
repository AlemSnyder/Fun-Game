#include "graphics_main.hpp"

#include "chrono"
#include "config.h"
#include "gui/handler.hpp"
#include "gui/render/structures/screen_data.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "gui/ui/opengl_setup.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "world/climate.hpp"
#include "world/world.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// create structure to pass game start data either from command line or from gui
/*
struct Options {
    std::optional<std::string> biome;

    std::optional<std::filesystem::path> save_file;
};*/

struct background_result {
    int result;
    std::unique_ptr<world::World> world;
    std::unique_ptr<world::Climate> climate;
};

int
graphics_main(const argh::parser& cmdl) {
    // add window width and height?

    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;

    screen_size_t display_w, display_h;
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

    // Start Splash screen
    gui::shader::ShaderHandler temp_handler;
    gui::shader::Program& splash_screen_program = temp_handler.load_program(
        "Splash Screen", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Green.frag"
    );

    std::function<void()> splash_screen_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    auto splash_screen_pipeline = std::make_shared<gui::shader::ShaderProgram_Standard>(
        splash_screen_program, splash_screen_setup
    );

    // TODO allow to do all these without differed
    // TODO assert in at the lowest level that opengl things are run on the main thread
    auto screen_data = std::make_shared<gui::gpu_data::ScreenData>();

    splash_screen_pipeline->data.push_back(screen_data.get());

    // background task {

    // Think about this later
    // need to read biomes from manifest
    // then in background need to mesh things
    // then send those things back to main thread
    // the main thread loads meshes onto gpu and renders to screen
    // Read manifest
    //    util::load_manifest();

    GlobalContext& global_context = GlobalContext::instance();

    // don't forget ot load ScreenData onto gpu
    global_context.run_opengl_queue();

    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", STRESS_TEST_SIZE) >> size;
    std::string biome_name;
    cmdl("biome-name", BIOME_BASE_NAME) >> biome_name;

    manifest::ObjectHandler object_handler;
    auto load_manifests_future =
        global_context.submit_task([&object_handler, biome_name, size, seed]() {
            background_result result;

            int manifest_result = object_handler.load_all_manifests<true>();

            result.result = manifest_result;

            if (result.result == 1) {
                return result;
            }

            result.world = std::make_unique<world::World>(
                &object_handler, biome_name, size, size, seed
            );
            result.world->generate_plants();
            result.climate = std::make_unique<world::Climate>();

            return result;
        });

    // generate options either from command line inputs
    // or from gui
    // struct Options

    //}

    // can't cancel a task after it has been started run.

    while (load_manifests_future.wait_for(std::chrono::seconds(0))
           != std::future_status::ready) {
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // gui::FrameBufferHandler::instance().bind_fbo(0);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // glViewport(0, 0, display_w, display_h);

        splash_screen_pipeline->render(display_w, display_h, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    auto future_value = load_manifests_future.get();

    if (future_value.result == 1) {
        LOG_ERROR(logging::main_logger, "Error Loading. Exiting.");
        return 1;
    }

    world::World& world = *future_value.world;
    world::Climate& climate = *future_value.climate;

    // if need gui start gui
    bool imgui_debug = cmdl[{"-g", "--imgui"}];
    if (imgui_debug) {
        return gui::imgui_entry(window, world, climate);
    } else {
        // if don't then strate to opengl
        return gui::opengl_entry(window, world, climate);
    }

    glDeleteVertexArrays(1, &VertexArrayID);

    return 0;
}

int
graphics_main() {
    LOG_WARNING(logging::main_logger, "Only kinda implemented");

    // global context and logging are already initalized.

    // read commands from file
    // Things like screen size
    // full screen
    // all graphics/audio settings
    // not that bad, make a settings header and define structures in there.

    intro_scene::result result = intro_scene::IntroPage();

    while (true) {
        switch (result.index()) {
            case 0: // intro page
                result = intro_window();
                break;
            case 1: // new world with settings
                {
                    auto settings = std::get<intro_scene::NewGame>(result);
                    result = graphics_main(settings);
                }
                break;
            case 2: // from save with file path
                {
                    auto path = std::get<intro_scene::LoadGame>(result);
                    result = graphics_main(path);
                }
                break;
            case 3: // exiting
                return std::get<intro_scene::Exit>(result).status;
            default:
                LOG_WARNING(
                    logging::main_logger, "NOT IMPLEMENTED/Something went wrong"
                );
                break;
        }
    }

    return 1;
}
