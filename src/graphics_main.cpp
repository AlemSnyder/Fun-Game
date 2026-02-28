#include "graphics_main.hpp"

#include "chrono"
#include "config.h"
#include "gui/handler.hpp"
#include "gui/render/structures/screen_data.hpp"
#include "gui/scene/input.hpp"
#include "gui/the_buttons/user_interface.hpp"
#include "gui/ui/imgui_gui.hpp"
#include "gui/ui/opengl_gui.hpp"
#include "gui/ui/opengl_setup.hpp"
#include "gui/ui/user_interface_setup.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "world/climate.hpp"
#include "world/world.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>

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

// read settings from command line
int
graphics_main(const argh::parser& cmdl) {
    size_t seed;
    cmdl("seed", SEED) >> seed;
    size_t size;
    cmdl("size", STRESS_TEST_SIZE) >> size;
    std::string biome_name;
    cmdl("biome-name", BIOME_BASE_NAME) >> biome_name;
    bool imgui_debug = cmdl[{"-g", "--imgui"}];

    intro_scene::result new_game_settings = intro_scene::NewGame{
        .biome = biome_name, .seed = seed, .size = size, .DearIMGUI = imgui_debug
    };

    return graphics_main(new_game_settings);
}

int
graphics_main(intro_scene::result result) {
    LOG_WARNING(logging::main_logger, "Only kinda implemented");

    // global context and logging are already initalized.

    // read commands from file
    // Things like screen size
    // full screen
    // all graphics/audio settings
    // not that bad, make a settings header and define structures in there.

    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;
    // init graphics

    std::optional<GLFWwindow*> opt_window =
        gui::setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    // TODO stream line this part and make sure to clean things up correctly
    GLFWwindow* window = opt_window.value();
    gui::setup_opengl_logging();

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    gui::VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
    //    intro_scene::result result = intro_scene::IntroPage();

    while (true) {
        switch (result.index()) {
            case 0: // exiting
                goto exit;
                break;
            case 1: // intro page
                result = intro_window(window);
                break;
            case 2: // new world with settings
                {
                    // auto settings = std::get<intro_scene::NewGame>(result);
                    result = start_game(result, window);
                }
                break;
            case 3: // from save with file path
                {
                    // auto path = std::get<intro_scene::LoadGame>(result);
                    result = start_game(result, window);
                }
                break;
            default:
                LOG_WARNING(
                    logging::main_logger, "NOT IMPLEMENTED/Something went wrong"
                );
                break;
        }
    }
exit:
    gui::scene::InputHandler::forward_inputs_to(
        nullptr
    ); // this removes an object from static storage
    GlobalContext& context = GlobalContext::instance();
    context.run_opengl_queue(); // this should clean up anything left in the queue

    glDeleteVertexArrays(1, &VertexArrayID);
    return std::get<intro_scene::Exit>(result).status;
}

intro_scene::result
start_game(intro_scene::result result, GLFWwindow* window) {
    // add window width and height?

    screen_size_t display_w, display_h;

    // Start Splash screen
    gui::shader::ShaderHandler temp_handler;
    gui::shader::Program& splash_screen_program = temp_handler.load_program(
        "Splash Screen", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Green.frag"
    );

    std::function<void()> splash_screen_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
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

    // background task
    GlobalContext& global_context = GlobalContext::instance();
    // don't forget ot load ScreenData onto gpu
    global_context.run_opengl_queue();
    manifest::ObjectHandler object_handler;

    std::future<background_result> load_manifests_future;
    bool imgui_debug;

    if (result.index() == 2) { // new game
        std::string biome_name = std::get<intro_scene::NewGame>(result).biome;
        size_t size = std::get<intro_scene::NewGame>(result).size;
        size_t seed = std::get<intro_scene::NewGame>(result).seed;
        imgui_debug = std::get<intro_scene::NewGame>(result).DearIMGUI;

        load_manifests_future =
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

    } else if (result.index() == 2) { // load game
        LOG_ERROR(logging::main_logger, "Loading World Not Implemented (yet*)");
        return intro_scene::IntroPage();
        // not implemented
    }

    // can't cancel a task after it has been started run.

    while (load_manifests_future.wait_for(std::chrono::seconds(0))
           != std::future_status::ready) {
        glfwGetFramebufferSize(window, &display_w, &display_h);

        splash_screen_pipeline->render(display_w, display_h, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    auto future_value = load_manifests_future.get();

    if (future_value.result == 1) {
        LOG_ERROR(logging::main_logger, "Error Loading. Exiting.");
        return intro_scene::Exit(1);
    }

    world::World& world = *future_value.world;
    world::Climate& climate = *future_value.climate;

    // if need gui start gui
    if (imgui_debug) {
        int gui_result = gui::imgui_entry(window, world, climate);
        return intro_scene::Exit(gui_result);
    } else {
        // if don't then strate to opengl
        int gui_result = gui::opengl_entry(window, world, climate);
        return intro_scene::Exit(gui_result);
    }

    return intro_scene::Exit(0);
}

// TODO move to separate file
intro_scene::result
intro_window(GLFWwindow* window) {
    screen_size_t display_w, display_h;

    // Start Splash screen
    gui::shader::ShaderHandler temp_handler;
    gui::shader::Program& splash_screen_program = temp_handler.load_program(
        "Splash Screen", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Blue.frag"
    );

    std::function<void()> splash_screen_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    auto splash_screen_pipeline = std::make_shared<gui::shader::ShaderProgram_Standard>(
        splash_screen_program, splash_screen_setup
    );

    auto screen_data = std::make_shared<gui::gpu_data::ScreenData>();

    splash_screen_pipeline->data.push_back(screen_data.get());

    GlobalContext& global_context = GlobalContext::instance();
    // don't forget ot load ScreenData onto gpu

    // gui::the_buttons::UserInterface main_interface(temp_handler, 4);
    // gui::setup(main_interface);

    auto main_interface =
        std::make_shared<gui::the_buttons::UserInterface>(temp_handler, 4);
    gui::setup(*main_interface);

    gui::scene::InputHandler::imgui_active = false;
    gui::scene::InputHandler::set_window(window);
    gui::scene::InputHandler::forward_inputs_to(
        std::static_pointer_cast<gui::scene::Inputs>(main_interface)
    );
    global_context.run_opengl_queue();

    while (!glfwWindowShouldClose(window)) {
        glfwGetFramebufferSize(window, &display_w, &display_h);

        splash_screen_pipeline->render(display_w, display_h, 0);

        main_interface->update(display_w, display_h);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return intro_scene::Exit(0);
}
