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
#include "imgui_style.hpp"
#include "imgui_windows.hpp"
#include "logging.hpp"
#include "opengl_setup.hpp"
#include "scene_setup.hpp"
#include "world/climate.hpp"
#include "world/entity/mesh.hpp"
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

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_another_window = false;
    bool show_light_controls = false;
    bool show_shadow_map = false;

    shader::ShaderHandler shader_handler;

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
    Scene main_scene(mode->width, mode->height, shadow_map_size);
    setup(main_scene, shader_handler, world, climate);

    glm::vec3 position;

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

        if (!io.WantCaptureKeyboard && !io.WantCaptureMouse) {
#if !DEBUG()
            // Disable the mouse so it doesn't appear while playing
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif

            // Process inputs
            controls::computeMatricesFromInputs(window);
        } else {
            // Show the mouse for use with IMGUI
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        glfwGetWindowSize(window, &window_width, &window_height);

        main_scene.update_light_direction();

        main_scene.update(window_width, window_height);

        position = controls::get_position_vector();

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

            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                ImGui::SetWindowFocus();
            }

            ImGui::Text("This is some useful text."
            ); // Display some text (you can use a format strings too)
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::Checkbox("Show Light Controls", &show_light_controls);
            ImGui::Checkbox("Show Shadow Map", &show_shadow_map);

            ImGui::Text(
                "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate,
                io.Framerate
            );
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window) {
            ImGui::Begin(
                "Another Window", &show_another_window
            ); // Pass a pointer to our bool variable (the window will have a closing
               // button that will clear the bool when clicked)
            ImGui::Text(
                "position <%.3f, %.3f, %.3f>", position.x, position.y, position.z
            );
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        if (show_light_controls) {
            display_windows::display_data(
                main_scene.get_lighting_environment(), show_light_controls
            );
        }

        display_windows::display_data(shader_handler.get_programs());

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            show_shadow_map = true;
        }

        if (show_shadow_map) {
            ImGui::Begin("Shadow Depth Texture", &show_shadow_map);
            if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
                ImGui::SetWindowFocus();
            }
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
