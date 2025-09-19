// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable
// pipeline (GLFW is a cross-platform general purpose library for handling windows,
// inputs, OpenGL/Vulkan/Metal graphics context creation, etc.) If you are new to Dear
// ImGui, read documentation from the docs/ folder + read the top of imgui.cpp. Read
// online: https://github.com/ocornut/imgui/tree/master/docs

// Some slight modifications
#include "imgui_gui.hpp"

#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"
#include "gui/scene/input.hpp"
#include "imgui_style.hpp"
#include "imgui_windows.hpp"
#include "logging.hpp"
#include "manifest/object_handler.hpp"
#include "opengl_setup.hpp"
#include "scene_setup.hpp"
#include "util/mesh.hpp"
#include "world/climate.hpp"
#include "world/world.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <memory>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize
// ease of testing and compatibility with old VS compilers. To link with VS2010-era
// libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do
// using this pragma. Your own project should not be affected, as you are likely to link
// with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#  pragma comment(lib, "legacy_stdio_definitions")
#endif

namespace gui {

// Main code
// returns exit status
int
imgui_entry(GLFWwindow* window, world::World& world, world::Climate& climate) {
    screen_size_t window_width;
    screen_size_t window_height;
    screen_size_t shadow_map_size = 4096;

    glfwGetWindowSize(window, &window_width, &window_height);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    set_imgui_style();

    const char* glsl_version = "#version 450";

    // Our state
    bool show_position_window = false;
    bool show_light_controls = false;
    bool show_shadow_map = false;
    bool show_programs_window = false;
    bool show_entity_window = false;
    bool show_scene_depth_interact_window = false;

    glm::vec3 position;

    auto key_map =
        files::read_json_from_file<std::unordered_map<gui::scene::Action, gui::Key>>(
            files::get_data_path() / "keymapping.json"
        );
    gui::scene::KeyMapping key_mapping = key_map.has_value()
                                             ? gui::scene::KeyMapping(key_map.value())
                                             : gui::scene::KeyMapping();
    std::shared_ptr<scene::Controls> controller =
        std::make_shared<scene::Controls>(key_mapping);
    scene::InputHandler::set_window(window);
    scene::InputHandler::forward_inputs_to(static_pointer_cast<scene::Inputs>(controller
    ));

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    std::unordered_set<std::shared_ptr<world::object::entity::EntityInstance>>
        path_entities;

    shader::ShaderHandler shader_handler;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    Scene main_scene(mode->width, mode->height, shadow_map_size, controller);
    setup(main_scene, shader_handler, world, climate);

    //! Main loop

    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
        // dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
        // main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
        // your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from
        // your application based on those two flags.
        glfwPollEvents();
        scene::InputHandler::handle_pooled_inputs(window);

        if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
#if !DEBUG()
            // Disable the mouse so it doesn't appear while playing
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
        } else {
            // Show the mouse for use with IMGUI
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        glfwGetWindowSize(window, &window_width, &window_height);

        main_scene.update_light_direction();

        world.update_entities();

        main_scene.update(window_width, window_height);

        // "render" scene to the screen
        main_scene.copy_to_window(window_width, window_height);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to
        // create a named window.
        {
            ImGui::Begin("Hello, world!"
            ); // Create a window called "Hello, world!" and append into it.

            if (scene::InputHandler::escape()) {
                ImGui::SetWindowFocus();
                // the lifting of the escape key will not be capture because imgui will
                // be focused
                scene::InputHandler::clear_escape();
            }

            ImGui::Text("This is some useful text."
            ); // Display some text (you can use a format strings too)
            ImGui::Checkbox("Position Window", &show_position_window);
            ImGui::Checkbox("Show Light Controls", &show_light_controls);
            ImGui::Checkbox("Show Shadow Map", &show_shadow_map);
            ImGui::Checkbox("Show Programs", &show_programs_window);
            ImGui::Checkbox("Show Entities", &show_entity_window);
            ImGui::Checkbox("Scene", &show_scene_depth_interact_window);

            ImGui::Text(
                "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                io.Framerate
            );
            static int breadth_first_search_start[3];
            ImGui::DragInt3(
                "Start Position", breadth_first_search_start, (1.0F), 0, world.height
            );
            static bool path_exists = false;
            static int path_length = 0;

            if (ImGui::Button("Breadth First Search")) {
                for (auto& entity : path_entities) {
                    world.remove_entity(entity);
                }
                path_entities.clear();
                auto path = world.pathfind_to_object(
                    TerrainOffset3(
                        breadth_first_search_start[0], breadth_first_search_start[1],
                        breadth_first_search_start[2]
                    ),
                    "base/Flower_Test"
                );
                if (path) {
                    path_exists = true;
                    path_length = path.value().size();

                    // auto test_object = object_handler.get_object("base/Test_Entity");

                    for (auto position : path.value()) {
                        auto new_entity =
                            world.spawn_entity("base/Test_Entity", position);

                        path_entities.insert(new_entity);
                    }

                } else {
                    path_exists = false;
                    path_length = 0;
                }
            }
            ImGui::Text(
                "Path found: %s. With length %d.", path_exists ? "true" : "false",
                path_length
            );

            if (ImGui::Button("Clear Breadth First Search")) {
                for (auto& entity : path_entities) {
                    world.remove_entity(entity);
                }
                path_entities.clear();
            }
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_position_window) {
            position = main_scene.get_viewer_position();
            ImGui::Begin(
                "Position Window", &show_position_window
            ); // Pass a pointer to our bool variable (the window will have a closing
               // button that will clear the bool when clicked)
            ImGui::Text(
                "position <%.3f, %.3f, %.3f>", position.x, position.y, position.z
            );
            ImGui::End();
        }

        if (show_light_controls) {
            display_windows::display_data(
                main_scene.get_lighting_environment(), show_light_controls
            );
        }

        if (show_programs_window) {
            display_windows::display_data(
                shader_handler.get_programs(), show_programs_window
            );
        }

        if (show_entity_window) {
            display_windows::display_data(
                *world.get_object_handler(), show_entity_window
            );
        }

        if (show_shadow_map) {
            ImGui::Begin("Shadow Depth Texture", &show_shadow_map);
            ImGui::Image(
                reinterpret_cast<ImTextureID>(main_scene.get_depth_texture()),
                ImVec2(
                    main_scene.get_shadow_width() / 8,
                    main_scene.get_shadow_height() / 8
                ),
                ImVec2(0, 1), ImVec2(1, 0)
            );

            ImGui::End();
        }

        if (show_scene_depth_interact_window) {
            display_windows::display_data(main_scene, show_scene_depth_interact_window, window_width, window_height);
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup VBO and shader
    // glDeleteVertexArrays(1, &VertexArrayID);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

} // namespace gui
