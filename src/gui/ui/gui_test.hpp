#pragma once

#include "../../entity/mesh.hpp"
#include "../../logging.hpp"
#include "../../world.hpp"
#include "../gui_logging.hpp"
#include "../handler.hpp"
#include "opengl_setup.hpp"
// #include "../render/graphics_shaders/shader.hpp"
#include "../render/data_structures/screen_data.hpp"
#include "../render/graphics_shaders/program_handler.hpp"
#include "../render/graphics_shaders/shader_program.hpp"
#include "../render/uniform_types.hpp"
#include "../scene/controls.hpp"
#include "../scene/scene.hpp"

#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/imgui.h>
#include <stdio.h>

#include <functional>
#include <memory>
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <memory>
#include <glm/glm.hpp>

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

    shader::ShaderHandler shader_handler;

    shader::Program& sky_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Green.frag"
    );

    /*
        std::function<void()> sky_render_setup = []() {
            // Draw over everything
            glDisable(GL_CULL_FACE);
            // The sky has no depth
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_FALSE);
        };

        auto pixel_projection = std::make_shared<render::PixelProjection>();

        shader::Uniforms sky_render_program_uniforms(
            std::vector<std::shared_ptr<shader::Uniform>>({pixel_projection})
        );

        auto sky_renderer = std::make_shared<shader::ShaderProgram_Standard>(
            sky_program, sky_render_setup, sky_render_program_uniforms
        );

        do {
            FrameBufferHandler::instance().bind_fbo(0);

            sky_renderer->render(window_width, window_height, 0);

            // Swap buffers
            glfwSwapBuffers(window);
            glfwPollEvents();

        } // Check if the ESC key was pressed or the window was closed
        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS
               && glfwWindowShouldClose(window) == 0);

        // Cleanup VBO and shader
        glDeleteVertexArrays(1, &VertexArrayID);

        glfwDestroyWindow(window);

        // Close OpenGL window and terminate GLFW
        glfwTerminate();

        return 0;*/

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left
        0.5f,  -0.5f, 0.0f, // right
        0.0f,  0.5f,  0.0f  // top
    };

    std::vector<glm::vec3> vertices_2;

    vertices_2.push_back(glm::vec3(-0.5, -0.5, 0));
    vertices_2.push_back(glm::vec3(0.5, -0.5, 0));
    vertices_2.push_back(glm::vec3(0.0, 0.5, 0));


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
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as
    // the vertex attribute's bound vertex buffer object so afterwards we can safely
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int VBO2;
    glGenBuffers(1, &VBO2);
    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices_2.size(), vertices_2.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as
    // the vertex attribute's bound vertex buffer object so afterwards we can safely
    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);



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
        // render
        // ------
        glClear(GL_COLOR_BUFFER_BIT);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // draw our first triangle
        glUseProgram(sky_program.get_program_ID());
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

} // namespace gui
