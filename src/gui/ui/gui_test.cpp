#include "gui_test.hpp"

#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/structures/screen_data.hpp"
#include "../render/structures/star_data.hpp"
#include "../render/structures/uniform_types.hpp"
#include "../scene/controls.hpp"
#include "../scene/helio.hpp"
#include "../scene/scene.hpp"
#include "logging.hpp"
#include "opengl_setup.hpp"
#include "world/climate.hpp"
#include "world/entity/mesh.hpp"
#include "world/world.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <stdio.h>

#include <functional>
#include <memory>

namespace gui {

int
revised_gui_test() {
    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;

    std::optional<GLFWwindow*> opt_window = setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    GLFWwindow* window = opt_window.value();
    setup_opengl_logging();

    controls::computeMatricesFromInputs(window);

    shader::ShaderHandler shader_handler;

    shader::Program& green_program = shader_handler.load_program(
        "Green", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Green.frag"
    );

    shader::Program& blue_program = shader_handler.load_program(
        "Blue", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Blue.frag"
    );

    shader::Program& sky_program = shader_handler.load_program(
        "Sky", files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "background" / "Sky.frag"
    );

    std::function<void()> sky_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    auto lighting_environment = std::make_shared<scene::Helio>(.3, 5, 60, .3);

    auto pixel_projection = std::make_shared<render::PixelProjection>();

    pixel_projection->update(window_width, window_height);

    auto matrix_view_inverse_projection =
        std::make_shared<render::MatrixViewInverseProjection>();

    auto light_direction_uniform =
        std::make_shared<render::LightDirection>(lighting_environment);

    auto spectral_light_color_uniform =
        std::make_shared<render::SpectralLight>(lighting_environment);

    sky_program.set_uniform(pixel_projection, "pixel_projection");
    sky_program.set_uniform(matrix_view_inverse_projection, "MVIP");
    sky_program.set_uniform(light_direction_uniform, "light_direction");
    sky_program.set_uniform(spectral_light_color_uniform, "direct_light_color");

    auto sky_renderer2 =
        std::make_shared<shader::ShaderProgram_Standard>(sky_program, sky_render_setup);

    auto sky_renderer = std::make_shared<shader::ShaderProgram_Standard>(
        blue_program, sky_render_setup
    );

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f,  -0.5f, 0.0f, // right
        0.0f,  0.5f,  0.0f  // top
    };

    std::vector<glm::vec3> vertices_2;

    vertices_2.push_back(glm::vec3(-0.5, -0.5, 0));
    vertices_2.push_back(glm::vec3(-0.5, 0.5, 0));
    vertices_2.push_back(glm::vec3(0.5, 0.0, 0));

    std::vector<glm::vec3> vertices_3;

    vertices_3.push_back(glm::vec3(-1, -1, 0));
    vertices_3.push_back(glm::vec3(-1, 1, 0));
    vertices_3.push_back(glm::vec3(1, -1, 0));
    vertices_3.push_back(glm::vec3(1, 1, 0));

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
    // glBindVertexArray(VertexArrayID);

    // glGenVertexArrays(1, &VAO);
    //  bind the Vertex Array Object first, then bind and set vertex buffer(s), and then
    //  configure vertex attributes(s).

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as
    // the vertex attribute's bound vertex buffer object so afterwards we can safely
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * vertices_2.size() * 3, vertices_2.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as
    // the vertex attribute's bound vertex buffer object so afterwards we can safely
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    gui::gpu_data::VertexBufferObject VBO3(vertices_2);

    gui::gpu_data::ScreenData screen_data;
    sky_renderer->data.push_back(&screen_data);
    sky_renderer2->data.push_back(&screen_data);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify
    // this VAO, but this rarely happens. Modifying other VAOs requires a call to
    // glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's
    // not directly necessary.
    VertexBufferHandler::instance().unbind_vertex_buffer();

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // render loop
    // -----------
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window) == 0) {
        lighting_environment->update();

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT);

        VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);

        sky_renderer->render(window_width, window_height, 0);

        sky_renderer2->render(window_width, window_height, 0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(
            0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0
        ); // here
        glEnableVertexAttribArray(0);

        // draw our first triangle
        glUseProgram(green_program.get_program_ID());
        VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);

        VBO3.attach_to_vertex_attribute(0);

        // draw our first triangle
        glUseProgram(green_program.get_program_ID());
        VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind it every time

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
        // etc.)
        // -------------------------------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VertexArrayID);
    glDeleteBuffers(1, &VBO);
    // glDeleteProgram(shaderProgram);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

int
stars_test() {
    screen_size_t window_width = 1280;
    screen_size_t window_height = 800;

    std::optional<GLFWwindow*> opt_window = setup_opengl(window_width, window_height);
    if (!opt_window) {
        LOG_CRITICAL(logging::opengl_logger, "No Window, Exiting.");
        return 1;
    }
    GLFWwindow* window = opt_window.value();
    setup_opengl_logging();

    controls::computeMatricesFromInputs(window);

    shader::ShaderHandler shader_handler;

    shader::Program& blue_program = shader_handler.load_program(
        "Blue", files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Blue.frag"
    );

    shader::Program& sky_program = shader_handler.load_program(
        "Sky", files::get_resources_path() / "shaders" / "background" / "Sky.vert",
        files::get_resources_path() / "shaders" / "background" / "Sky.frag"
    );

    shader::Program& stars_program = shader_handler.load_program(
        "Stars", files::get_resources_path() / "shaders" / "background" / "Stars.vert",
        files::get_resources_path() / "shaders" / "background" / "Stars.frag"
    );

    std::function<void()> sky_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    auto lighting_environment = std::make_shared<scene::Helio>(.3, 5, 60, .3);

    auto pixel_projection = std::make_shared<render::PixelProjection>();

    pixel_projection->update(window_width, window_height);

    auto matrix_view_inverse_projection =
        std::make_shared<render::MatrixViewInverseProjection>();

    auto light_direction_uniform =
        std::make_shared<render::LightDirection>(lighting_environment);

    auto spectral_light_color_uniform =
        std::make_shared<render::SpectralLight>(lighting_environment);

    auto matrix_view_projection_uniform =
        std::make_shared<render::MatrixViewProjection>();

    auto star_rotation_uniform =
        std::make_shared<render::StarRotationUniform>(lighting_environment);

    auto sky_renderer2 =
        std::make_shared<shader::ShaderProgram_Standard>(sky_program, sky_render_setup);

    auto blue_background = std::make_shared<shader::ShaderProgram_Standard>(
        blue_program, sky_render_setup
    );

    stars_program.set_uniform(matrix_view_projection_uniform, "MVIP");
    stars_program.set_uniform(pixel_projection, "pixel_projection");
    stars_program.set_uniform(star_rotation_uniform, "star_rotation");
    stars_program.set_uniform(light_direction_uniform, "light_direction");

    sky_program.set_uniform(matrix_view_projection_uniform, "MVIP");
    sky_program.set_uniform(light_direction_uniform, "light_direction");

    std::function<void()> star_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDepthMask(GL_FALSE);
    };

    auto star_renderer = std::make_shared<shader::ShaderProgram_Instanced>(
        stars_program, star_render_setup
    );

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);

    auto screen_data = std::make_shared<gpu_data::ScreenData>();

    world::Climate climate;

    blue_background->data.push_back(screen_data.get());

    star_renderer->data.push_back(climate.get_stars_data());

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify
    // this VAO, but this rarely happens. Modifying other VAOs requires a call to
    // glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's
    // not directly necessary.
    VertexBufferHandler::instance().unbind_vertex_buffer();

    // uncomment this call to draw in wireframe polygons.
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // render loop
    // -----------
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
           && glfwWindowShouldClose(window) == 0) {
        lighting_environment->update();

        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT);

        VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);

        blue_background->render(window_width, window_height, 0);

        star_renderer->render(window_width, window_height, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VertexArrayID);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

} // namespace gui
