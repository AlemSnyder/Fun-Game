// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable
// pipeline (GLFW is a cross-platform general purpose library for handling windows,
// inputs, OpenGL/Vulkan/Metal graphics context creation, etc.) If you are new to Dear
// ImGui, read documentation from the docs/ folder + read the top of imgui.cpp. Read
// online: https://github.com/ocornut/imgui/tree/master/docs

// Some slight modifications
#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../world.hpp"
#include "../render/data_structures/frame_buffer_multisample.hpp"
#include "../render/graphics_shaders/individual_int_renderer.hpp"
#include "../render/graphics_shaders/instanced_int_renderer.hpp"
#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "../render/graphics_shaders/individual_int_renderer.hpp"
#include "../render/data_structures/shadow_map.hpp"
#include "../render/graphics_shaders/sky.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"
#include "../shader.hpp"
#include "imgui_style.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#  include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <memory>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize
// ease of testing and compatibility with old VS compilers. To link with VS2010-era
// libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do
// using this pragma. Your own project should not be affected, as you are likely to link
// with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#  pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for
// details.
#ifdef __EMSCRIPTEN__
#  include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

namespace UI {

// Main code
int
imguiTest(World& world) {
    glEnable(GL_MULTISAMPLE);

    // initialize logging
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // Initialise GLFW
    glewExperimental = true; // Needed for core profile
    if (!glfwInit()) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
        getchar();
        glfwTerminate();
        return -1;
    }

    const char* glsl_version = "#version 450";

    glfwWindowHint(GLFW_SAMPLES, 4);               // anti-aliasing of 4
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set Major
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5); // and Minor version
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    ); // To make MacOS happy; should not be needed
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    ); // somehow turning on core profiling

    int window_width = 1280;
    int window_height = 800;

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(
        window_width, window_height, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL
    );
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLEW");
        getchar();
        glfwTerminate();
        return -1;
    }

    // initialize logging
    GLint context_flag;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flag);
    if (context_flag & GL_CONTEXT_FLAG_DEBUG_BIT) {
        LOG_INFO(logging::opengl_logger, "GLFW Logging with debug");
        try {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            // set gl message call back function
            glDebugMessageCallback(gui::message_callback, 0);
            glDebugMessageControl(
                GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE
            );
            LOG_INFO(logging::opengl_logger, "GLFW Logging initialized");
        } catch (...) {
            LOG_CRITICAL(logging::opengl_logger, "Failed to initialize GLFW");
            getchar();
            glfwTerminate();
            return -1;
        }
    }

    // send color texture to gpu
    terrain::TerrainColorMapping::assign_color_texture();

    // No idea why this is necessary, but it is
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    gui::set_imgui_style();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Our state
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    // ImVec2 button_size = ImVec2(100, 100);

    gui::Scene main_scene(window_width, window_height, 4096);

    gui::ShaderHandeler shader_handeler = gui::ShaderHandeler();

    world.update_all_chunks_mesh();

    std::vector<std::shared_ptr<gui::data_structures::TerrainMesh>> terrain_mesh =
        world.get_chunks_mesh();

    gui::models::IndividualIntRenderer<gui::data_structures::TerrainMesh> chunk_renderer(
        shader_handeler
    );

    // chunk_renderer.add_mesh()

    for (const auto& chunk_mesh : terrain_mesh) {
        chunk_mesh->set_color_texture(terrain::TerrainColorMapping::get_color_texture());
        chunk_renderer.add_mesh(chunk_mesh);
    }

    glm::vec3 light_direction =
        glm::normalize(glm::vec3(40.0f, 8.2f, 120.69f)) // direction
        * 128.0f;                                       // length

    glm::mat4 depth_projection_matrix =
        glm::ortho<float>(0.0f, 192.0f, 0.0f, 192.0f, 0.0f, 128.0f);

    // Renders the Shadow depth map
    chunk_renderer.set_light_direction(light_direction);
    chunk_renderer.set_depth_projection_matrix(depth_projection_matrix);

    chunk_renderer.set_depth_texture(main_scene.get_depth_texture());

    main_scene.set_shadow_light_direction(light_direction);
    main_scene.set_shadow_depth_projection_matrix(depth_projection_matrix);

    main_scene.frame_buffer_multisample_attach(
        std::make_shared<
            gui::models::IndividualIntRenderer<gui::data_structures::TerrainMesh>>(
            chunk_renderer
        )
    );

    main_scene.shadow_attach(std::make_shared<gui::models::IndividualIntRenderer<
                                  gui::data_structures::TerrainMesh>>(chunk_renderer));

    // main_scene.add render ()
    //  n more lines after this

    //! Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt
    // to do a fopen() of the imgui.ini file. You may manually call
    // LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = NULL;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
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

        // TODO
        // https://stackoverflow.com/questions/23362497/how-can-i-resize-existing-texture-attachments-at-my-framebuffer

        main_scene.update(window);

        glm::vec3 position = controls::get_position_vector();

        gui::FrameBufferHandler::bind_fbo(0);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // the scene frame
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        // the scene frame has no rounding/padding on border
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        {
            ImGui::Begin(
                "OpenGL Texture Image", 0,
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
                    | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings
                    | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar
                    | ImGuiWindowFlags_NoScrollWithMouse
                    | ImGuiWindowFlags_NoBringToFrontOnFocus
            );
            ImGui::Image(
                reinterpret_cast<ImTextureID>(main_scene.get_scene()),
                ImVec2(window_width, window_height)
            );

            if (ImGui::IsWindowFocused()) {
                controls::computeMatricesFromInputs(window);
            }

            ImGui::End();
        }
        // remove changes to style
        ImGui::PopStyleVar(3);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to
        // create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"
            ); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text."
            ); // Display some text (you can use a format strings too)
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat(
                "float", &f, 0.0f, 1.0f
            ); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3(
                "clear color", (float*)&clear_color
            ); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most
                                         // widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

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
                "positoin <%.3f, %.3f, %.3f>", position.x, position.y, position.z
            );
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        {
            ImGui::Begin("OpenGL Texture Text");
            if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
                ImGui::SetWindowFocus();
            }
            ImGui::Text("pointer MS = %i", main_scene.get_scene());
            ImGui::Text("size = %d x %d", window_width, window_height);
            ImGui::End();
        }

        if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
            ImGui::Begin("Shadow Depth Texture");
            ImGui::Image(
                reinterpret_cast<ImTextureID>(main_scene.get_depth_texture()),
                ImVec2(
                    main_scene.get_shadow_width() / 8,
                    main_scene.get_shadow_height() / 8
                )
            );

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(
            clear_color.x * clear_color.w, clear_color.y * clear_color.w,
            clear_color.z * clear_color.w, clear_color.w
        );
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup VBO and shader
    glDeleteVertexArrays(1, &VertexArrayID);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

} // namespace UI
