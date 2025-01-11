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

/**
 * @file shader_program.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines shader program classes
 *
 * @ingroup GUI  SHADER
 *
 * To any future reader: I hate how this is written. There is duplication
 * because of language constrains. If you can find a way to fix this that would
 * be quite beneficial.
 *
 * This file defines four classes. They are nearly identical, but they have
 * different render methods. They cannot be templated because they are
 * render_to::FrameBuffer interfaces. GCC doesn't allow virtual templates
 * because the compiler needs to know "soon" how many classes are of a certain
 * virtual type.
 */

#pragma once

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

/**
 * @brief Logs needed and unneeded uniforms.
 *
 * @param std::set<std::pair<std::string,gpu_data::GPUDataType>> want_uniforms
 * @param std::set<std::pair<std::string,gpu_data::GPUDataType>> has_uniforms
 */
inline void
log_uniforms(
    std::set<std::pair<std::string, gpu_data::GPUDataType>> want_uniforms,
    std::set<std::pair<std::string, gpu_data::GPUDataType>> has_uniforms
) {
#if DEBUG()

    // Literally just a set compare.

    std::set<std::pair<std::string, gpu_data::GPUDataType>> uniforms_needed;
    std::set<std::pair<std::string, gpu_data::GPUDataType>> uniforms_not_needed;

    std::set_difference(
        want_uniforms.begin(), want_uniforms.end(), has_uniforms.begin(),
        has_uniforms.end(), std::inserter(uniforms_needed, uniforms_needed.begin())
    );

    std::set_difference(
        has_uniforms.begin(), has_uniforms.end(), want_uniforms.begin(),
        want_uniforms.end(),
        std::inserter(uniforms_not_needed, uniforms_not_needed.begin())
    );

/*
    if (uniforms_needed.size() > 0)
        LOG_WARNING(logging::opengl_logger, "Needed uniforms {}", uniforms_needed);
    if (uniforms_not_needed.size() > 0)
        LOG_DEBUG(
            logging::opengl_logger, "Not needed uniforms {}", uniforms_not_needed
        );
*/

#endif
}

/**
 * @brief Base for render programs
 *
 * @warning don't use this or cast to this. It is used to reduce the number of lines in
 * the file by 40.
 */
class Render_Base {
 protected:
    Program& opengl_program_;

    const std::function<void()> setup_;

 public:
    inline Render_Base(
        shader::Program& shader_program, const std::function<void()> setup_commands) :
        opengl_program_(shader_program),
        setup_(setup_commands) {
        LOG_DEBUG(
            logging::opengl_logger, "Program ID: {}", opengl_program_.get_program_ID()
        );
        //LOG_DEBUG(logging::opengl_logger, "Uniforms ID: {}", uniforms_.get_names());
        //log_uniforms(shader_program.get_detected_uniforms(), uniforms.get_names());
    }

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        opengl_program_.use_program();

        setup_();

        opengl_program_.bind_uniforms();
    }
};

// TODO each of these should use the glMulti... version of the given command

/**
 * @brief No elements No instancing
 */
class ShaderProgram_Standard :
    public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUData*> data;

    inline ShaderProgram_Standard(
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        Render_Base(shader_program, setup_commands) {}

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
#if DEBUG()
        static bool has_logged = false;
        if (data.size() == 0 && !has_logged) {
            LOG_WARNING(logging::opengl_logger, "Nothing to be rendered.");
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
};

/**
 * @brief Yes elements No instancing
 */
class ShaderProgram_Elements :
    public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataElements*> data;

    inline ShaderProgram_Elements(
        shader::Program& shader_program, const std::function<void()> setup_commands    ) :
        Render_Base(shader_program, setup_commands) {}

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
#if DEBUG()
        static bool has_logged = false;
        if (data.size() == 0 && !has_logged) {
            LOG_WARNING(logging::opengl_logger, "Nothing to be rendered.");
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
};

/**
 * @brief No elements Yes instancing
 */
class ShaderProgram_Instanced :
    public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataInstanced*> data;

    inline ShaderProgram_Instanced(
        shader::Program& shader_program, const std::function<void()> setup_commands    ) :
        Render_Base(shader_program, setup_commands) {}

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
#if DEBUG()
        static bool has_logged = false;
        if (data.size() == 0 && !has_logged) {
            LOG_WARNING(logging::opengl_logger, "Nothing to be rendered.");
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
};

/**
 * @brief Yes elements Yes instancing
 */
class ShaderProgram_ElementsInstanced :
    public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataElementsInstanced*> data;

    inline ShaderProgram_ElementsInstanced(
        shader::Program& shader_program, const std::function<void()> setup_commands    ) :
        Render_Base(shader_program, setup_commands) {}

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
#if DEBUG()
        static bool has_logged = false;
        if (data.size() == 0 && !has_logged) {
            LOG_WARNING(logging::opengl_logger, "Nothing to be rendered.");
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
                mesh->get_num_vertices(), to_string(element_type),
                mesh->get_num_models()
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
};

/**
 * @brief No elements Yes instancing array of multiple
 */
class ShaderProgram_MultiElements :
    public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataElementsMulti*> data;

    inline ShaderProgram_MultiElements(
        shader::Program& shader_program, const std::function<void()> setup_commands    ) :
        Render_Base(shader_program, setup_commands) {}

    inline void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) {
#if DEBUG()
        static bool has_logged = false;
        if (data.size() == 0 && !has_logged) {
            LOG_WARNING(logging::opengl_logger, "Nothing to be rendered.");
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
                sizeof(void*) == sizeof(size_t),
                "Sizes should match for reinterpret_cast."
            );

            LOG_BACKTRACE(
                logging::opengl_logger,
                "glMultiDrawElementsBaseVertex(GL_TRIANGLES, [...], {}, [...], {}, [...])",
                to_string(element_type),
                mesh->get_num_objects()
            );

            // Draw the triangles !
            glMultiDrawElementsBaseVertex(
                GL_TRIANGLES,                      // mode
                mesh->get_num_vertices().data(),   // count
                static_cast<GLenum>(element_type), // type
                reinterpret_cast<const void* const*>(mesh->get_elements_position().data(
                )),                      // indices
                mesh->get_num_objects(), // drawcount
                mesh->get_base_vertex().data()
            );

            mesh->release();
        }
    }
};

} // namespace shader

} // namespace gui
