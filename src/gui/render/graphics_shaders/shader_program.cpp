// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

#include "shader_program.hpp"

#include "gui/handler.hpp"
#include "gui/render/gpu_data/data_types.hpp"
#include "logging.hpp"
#include "program_handler.hpp"
#include "render_types.hpp"
#include "types.hpp"
#include "uniform.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace gui {

namespace shader {

void
Render_Base::render(screen_size_t width, screen_size_t height, GLuint framebuffer_ID) {
    // Render to the screen
    gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

    // Render on the whole framebuffer, complete
    // from the lower left corner to the upper right
    glViewport(0, 0, width, height);

    opengl_program_.use_program();

    setup_();

    opengl_program_.bind_uniforms();
}

void
ShaderProgram_Standard::render(
    screen_size_t width, screen_size_t height, GLuint framebuffer_ID
) {
#if DEBUG()
    static bool has_logged = false;
    if (data.size() == 0 && !has_logged) {
        LOG_WARNING(
            logging::opengl_logger, "Nothing to be rendered. Program id {}",
            opengl_program_.get_program_ID()
        );
        has_logged = true;
    }
#endif

    Render_Base::render(width, height, framebuffer_ID);

    for (const auto mesh : data) {
        if (!mesh->do_render()) {
            continue;
        }

        mesh->bind();

        // Draw the triangles !
        glDrawArrays(
            GL_TRIANGLE_STRIP,       // mode
            0,                       // start
            mesh->get_num_vertices() // number of vertices
        );

        mesh->release();
    }
}

void
ShaderProgram_Elements::render(
    screen_size_t width, screen_size_t height, GLuint framebuffer_ID
) {
#if DEBUG()
    static bool has_logged = false;
    if (data.size() == 0 && !has_logged) {
        LOG_WARNING(
            logging::opengl_logger, "Nothing to be rendered. Program id {}",
            opengl_program_.get_program_ID()
        );
        has_logged = true;
    }
#endif

    Render_Base::render(width, height, framebuffer_ID);

    for (const auto mesh : data) {
        if (!mesh->do_render()) {
            continue;
        }

        mesh->bind();

        // test if T inherits from Instancing or not

        auto num_vertices = mesh->get_num_vertices();
        auto element_type = mesh->get_element_type();

        // Draw the triangles !
        glDrawElements(
            GL_TRIANGLES,                      // mode
            num_vertices,                      // count
            static_cast<GLenum>(element_type), // type
            (void*)0                           // element array buffer offset
        );

        mesh->release();
    }
}

void
ShaderProgram_Instanced::render(
    screen_size_t width, screen_size_t height, GLuint framebuffer_ID
) {
#if DEBUG()
    static bool has_logged = false;
    if (data.size() == 0 && !has_logged) {
        LOG_WARNING(
            logging::opengl_logger, "Nothing to be rendered. Program id {}",
            opengl_program_.get_program_ID()
        );
        has_logged = true;
    }
#endif

    Render_Base::render(width, height, framebuffer_ID);

    for (const auto mesh : data) {
        if (!mesh->do_render()) {
            continue;
        }

        mesh->bind();

        // Draw the triangles !
        glDrawArraysInstanced(
            GL_TRIANGLE_STRIP,        // mode
            0,                        // start
            mesh->get_num_vertices(), // number of vertices
            mesh->get_num_models()    // number of models

        );

        mesh->release();
    }
}

void
ShaderProgram_ElementsInstanced::render(
    screen_size_t width, screen_size_t height, GLuint framebuffer_ID
) {
#if DEBUG()
    static bool has_logged = false;
    if (data.size() == 0 && !has_logged) {
        LOG_WARNING(
            logging::opengl_logger, "Nothing to be rendered. Program id {}",
            opengl_program_.get_program_ID()
        );
        has_logged = true;
    }
#endif

    Render_Base::render(width, height, framebuffer_ID);

    for (const auto mesh : data) {
        if (!mesh->do_render()) {
            continue;
        }

        //            assert(static_cast<GLsizei>(mesh->get_num_vertices()) >= 0 &&
        //            "Num vertices must non-negative");
        //            assert(static_cast<GLsizei>(mesh->get_num_models()) >= 0 &&
        //            "Num models must non-negative");

        mesh->bind();

        auto element_type = mesh->get_element_type();

        LOG_BACKTRACE(
            logging::opengl_logger,
            "glDrawElementsInstanced(GL_TRIANGLES, {}, {}, 0, {})",
            mesh->get_num_vertices(), to_string(element_type), mesh->get_num_models()
        );

        // Draw the triangles !
        glDrawElementsInstanced(
            GL_TRIANGLES,                      // mode
            mesh->get_num_vertices(),          // count
            static_cast<GLenum>(element_type), // type
            (void*)0,                          // element array buffer offset
            mesh->get_num_models()             // instance count
        );

        mesh->release();
    }
}

void
ShaderProgram_MultiElements::render(
    screen_size_t width, screen_size_t height, GLuint framebuffer_ID
) {
#if DEBUG()
    static bool has_logged = false;
    if (data.size() == 0 && !has_logged) {
        LOG_WARNING(
            logging::opengl_logger, "Nothing to be rendered. Program id {}",
            opengl_program_.get_program_ID()
        );
        has_logged = true;
    }
#endif

    Render_Base::render(width, height, framebuffer_ID);

    for (const auto mesh : data) {
        if (!mesh->do_render()) {
            continue;
        }

        mesh->bind();

        auto element_type = mesh->get_element_type();

#if DEBUG()
        if (!has_logged) {
            LOG_INFO(
                logging::opengl_logger,
                "glMultiDrawElementsBaseVertex(GL_TRIANGLES, {}, {}, {}, {}, {})",
                mesh->get_num_vertices(), to_string(element_type),
                mesh->get_elements_position(), mesh->get_num_objects(),
                mesh->get_base_vertex()
            );
            has_logged = true;
        }
#endif

        static_assert(
            sizeof(void*) == sizeof(size_t), "Sizes should match for reinterpret_cast."
        );

        LOG_BACKTRACE(
            logging::opengl_logger,
            "glMultiDrawElementsBaseVertex(GL_TRIANGLES, [...], {}, [...], {}, [...])",
            to_string(element_type), mesh->get_num_objects()
        );

        // Draw the triangles !
        glMultiDrawElementsBaseVertex(
            GL_TRIANGLES,                      // mode
            mesh->get_num_vertices().data(),   // count
            static_cast<GLenum>(element_type), // type
            reinterpret_cast<const void* const*>(mesh->get_elements_position().data()
            ),                       // indices
            mesh->get_num_objects(), // drawcount
            mesh->get_base_vertex().data()
        );

        mesh->release();
    }
}

} // namespace shader

} // namespace gui
