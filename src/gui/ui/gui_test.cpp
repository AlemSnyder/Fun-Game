#include "gui_test.hpp"

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

    shader::Program& green_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Green.frag"
    );

    shader::Program& blue_program = shader_handler.load_program(
        files::get_resources_path() / "shaders" / "Passthrough.vert",
        files::get_resources_path() / "shaders" / "Blue.frag"
    );

    std::function<void()> sky_render_setup = []() {
        // Draw over everything
        glDisable(GL_CULL_FACE);
        // The sky has no depth
        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);
    };

    auto pixel_projection = std::make_shared<render::PixelProjection>();

    shader::Uniforms sky_render_program_uniforms(
        std::vector<std::shared_ptr<shader::Uniform>>({/*pixel_projection*/})
    );

    auto sky_renderer = std::make_shared<shader::ShaderProgram_Standard>(
        blue_program, sky_render_setup, sky_render_program_uniforms
    );

    /*
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

    gui::data_structures::ArrayBuffer VBO3(vertices_2);

    gui::data_structures::ArrayBuffer VBO4(vertices_3);


    auto screen_data = std::make_shared<gui::data_structures::ScreenData>();
    sky_renderer->data.push_back(screen_data);

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

        sky_renderer->render(window_width, window_height, 0);

        /*

        glViewport(0, 0, window_width, window_height);

        sky_renderer->use_program();
//        glUseProgram(blue_program.get_program_ID());

        screen_data->bind();

        // Draw the triangles !
        glDrawArrays(
            GL_TRIANGLE_STRIP,       // mode
            0,                       // start
            screen_data->get_num_vertices() // number of vertices

        );

        screen_data->release();

        */

/*

        VBO4.bind(0, 0);

        // draw our first triangle
        glUseProgram(blue_program.get_program_ID());
        VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
        // seeing as we only have a single VAO there's no need to bind it every time,
        // but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        // glBindVertexArray(0); // no need to unbind it every time

*/


        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

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

        /*

                glBindBuffer(GL_ARRAY_BUFFER, VBO2);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
           (void*)0); glEnableVertexAttribArray(0);

                // draw our first triangle
                glUseProgram(green_program.get_program_ID());
                VertexBufferHandler::instance().bind_vertex_buffer(VertexArrayID);
                // seeing as we only have a single VAO there's no need to bind it every
           time,
                // but we'll do so to keep things a bit more organized
                glDrawArrays(GL_TRIANGLES, 0, 3);
                // glBindVertexArray(0); // no need to unbind it every time

                // glfw: swap buffers and poll IO events (keys pressed/released, mouse
           moved
                // etc.)
                //
           -------------------------------------------------------------------------------

        */

        VBO3.bind(0, 0);

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

} // namespace gui
