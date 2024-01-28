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
 * @ingroup GUI  RENDER  GRAPHICS_SHADERS
 *
 */

#pragma once

#include "../../handler.hpp"
#include "../array_buffer/gpu_data.hpp"
#include "gui_render_types.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "uniform.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace gui {

namespace shader {

inline void
log_uniforms(
    std::set<std::pair<std::string, std::string>> want_uniforms,
    std::set<std::pair<std::string, std::string>> has_uniforms
) {
#if DEBUG()

    std::set<std::pair<std::string, std::string>> uniforms_needed;
    std::set<std::pair<std::string, std::string>> uniforms_not_needed;

    std::set_difference(
        want_uniforms.begin(), want_uniforms.end(), has_uniforms.begin(),
        has_uniforms.end(), std::inserter(uniforms_needed, uniforms_needed.begin())
    );

    std::set_difference(
        has_uniforms.begin(), has_uniforms.end(), want_uniforms.begin(),
        want_uniforms.end(),
        std::inserter(uniforms_not_needed, uniforms_not_needed.begin())
    );

    if (uniforms_needed.size() > 0)
        LOG_WARNING(logging::opengl_logger, "Needed uniforms {}", uniforms_needed);
    if (uniforms_not_needed.size() > 0)
        LOG_DEBUG(
            logging::opengl_logger, "Not needed uniforms {}", uniforms_not_needed
        );

#endif
}

class ShaderProgram_Standard : virtual public render_to::FrameBuffer {
 private:
    Program& opengl_program_;

    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUData>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_Standard(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        opengl_program_(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        log_uniforms(shader_program.get_detected_uniforms(), uniforms.get_names());
    }

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

        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        opengl_program_.use_program();

        setup_();

        // uniforms_.bind();

        for (auto uniform : uniforms_) {
            GLint uniform_id = opengl_program_.get_uniform(uniform->get_name());
            if (uniform_id != -1)
                uniform->bind(uniform_id);
        }

        for (std::shared_ptr<data_structures::GPUData> mesh : data) {
            if (!mesh->do_render()) {
                continue;
            }

            mesh->bind();

            // test if T inherits from Instancing or not

            // Draw the triangles !
            // glDrawElements(
            //    GL_TRIANGLES,             // mode
            //    mesh->get_num_vertices(), // count
            //    mesh->get_element_type(), // type
            //    (void*)0                  // element array buffer offset
            //);

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

class ShaderProgram_Elements : virtual public render_to::FrameBuffer {
 private:
    Program& opengl_program_;

    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUDataElements>> data;

    inline ShaderProgram_Elements(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        opengl_program_(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        log_uniforms(shader_program.get_detected_uniforms(), uniforms.get_names());
    }

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

        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        opengl_program_.use_program();

        setup_();

        for (auto uniform : uniforms_) {
            GLint uniform_id = opengl_program_.get_uniform(uniform->get_name());
            if (uniform_id != -1)
                uniform->bind(uniform_id);
        }

        for (std::shared_ptr<data_structures::GPUDataElements> mesh : data) {
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

            // Draw the triangles !
            // glDrawArrays(
            //    GL_TRIANGLE_STRIP,     // mode
            //    0,                     // start
            //    mesh->get_num_vertices // number of vertices

            //            );

            mesh->release();
        }
    }
};

class ShaderProgram_Instanced : virtual public render_to::FrameBuffer {
 private:
    Program& opengl_program_;

    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUDataInstanced>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_Instanced(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        opengl_program_(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        log_uniforms(shader_program.get_detected_uniforms(), uniforms.get_names());
    }

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

        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        opengl_program_.use_program();

        setup_();

        for (auto uniform : uniforms_) {
            GLint uniform_id = opengl_program_.get_uniform(uniform->get_name());
            if (uniform_id != -1)
                uniform->bind(uniform_id);
        }

        for (std::shared_ptr<data_structures::GPUDataInstanced> mesh : data) {
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

class ShaderProgram_ElementsInstanced : virtual public render_to::FrameBuffer {
 private:
    Program& opengl_program_;

    const std::function<void()> setup_;

    Uniforms uniforms_;

 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<std::shared_ptr<data_structures::GPUDataElementsInstanced>> data;

    //    template <data_structures::GPUdata_or_something T>
    inline ShaderProgram_ElementsInstanced(
        shader::Program& shader_program, const std::function<void()> setup_commands,
        Uniforms uniforms
    ) :
        opengl_program_(shader_program),
        setup_(setup_commands), uniforms_(uniforms) {
        LOG_DEBUG(
            logging::opengl_logger, "Program ID: {}", opengl_program_.get_program_ID()
        );
        LOG_DEBUG(logging::opengl_logger, "Uniforms ID: {}", uniforms_.get_names());
    }

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

        // Render to the screen
        gui::FrameBufferHandler::instance().bind_fbo(framebuffer_ID);

        // Render on the whole framebuffer, complete
        // from the lower left corner to the upper right
        glViewport(0, 0, width, height);

        opengl_program_.use_program();

        setup_();

        for (auto uniform : uniforms_) {
            GLint uniform_id = opengl_program_.get_uniform(uniform->get_name());
            if (uniform_id != -1)
                uniform->bind(uniform_id);
        }

        for (std::shared_ptr<data_structures::GPUDataElementsInstanced> mesh : data) {
            if (!mesh->do_render()) {
                continue;
            }

            mesh->bind();

            auto element_type = mesh->get_element_type();

            // Draw the triangles !
            glDrawElementsInstanced(
                GL_TRIANGLES,                      // mode
                mesh->get_num_vertices(),          // count
                static_cast<GLenum>(element_type), // type
                (void*)0,                          // element array buffer offset
                mesh->get_num_models()
            );

            mesh->release();
        }
    }
};

} // namespace shader

} // namespace gui