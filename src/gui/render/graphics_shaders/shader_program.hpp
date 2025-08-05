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
    [[maybe_unused]] std::set<std::pair<std::string, gpu_data::GPUDataType>>
        want_uniforms,
    [[maybe_unused]] std::set<std::pair<std::string, gpu_data::GPUDataType>>
        has_uniforms
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

    for (const auto& uniform : uniforms_needed) {
        LOG_WARNING(
            logging::opengl_logger, "Need \"{}\" uniform with type \"{}\".",
            uniform.first, gpu_data::to_string(uniform.second)
        );
    }

    for (const auto& uniform : uniforms_not_needed) {
        LOG_DEBUG(
            logging::opengl_logger, "Does not need \"{}\" uniform with type \"{}\".",
            uniform.first, gpu_data::to_string(uniform.second)
        );
    }

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
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        opengl_program_(shader_program),
        setup_(setup_commands) {
        LOG_DEBUG(
            logging::opengl_logger, "Program ID: {}", opengl_program_.get_program_ID()
        );
        // LOG_DEBUG(logging::opengl_logger, "Uniforms ID: {}", uniforms_.get_names());
        // log_uniforms(shader_program.get_detected_uniforms(), uniforms.get_names());
    }

    void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    );

    inline virtual ~Render_Base() {}
};

// TODO each of these should use the glMulti... version of the given command

/**
 * @brief No elements No instancing
 */
class ShaderProgram_Standard :
    virtual public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUData*> data;

    inline ShaderProgram_Standard(
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        Render_Base(shader_program, setup_commands) {}

    inline virtual ~ShaderProgram_Standard() {}

    void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) override;
};

/**
 * @brief Yes elements No instancing
 */
class ShaderProgram_Elements :
    virtual public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataElements*> data;

    inline ShaderProgram_Elements(
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        Render_Base(shader_program, setup_commands) {}

    inline virtual ~ShaderProgram_Elements() {}

    void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) override;
};

/**
 * @brief No elements Yes instancing
 */
class ShaderProgram_Instanced :
    virtual public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataInstanced*> data;

    inline ShaderProgram_Instanced(
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        Render_Base(shader_program, setup_commands) {}

    inline virtual ~ShaderProgram_Instanced() {}

    void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) override;
};

/**
 * @brief Yes elements Yes instancing
 */
class ShaderProgram_ElementsInstanced :
    virtual public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataElementsInstanced*> data;

    inline ShaderProgram_ElementsInstanced(
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        Render_Base(shader_program, setup_commands) {}

    inline virtual ~ShaderProgram_ElementsInstanced() {}

    void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) override;
};

/**
 * @brief No elements Yes instancing array of multiple
 */
class ShaderProgram_MultiElements :
    virtual public Render_Base,
    virtual public render_to::FrameBuffer {
 public:
    // Ya I know this looks bad, but data_ is basically a parameter
    std::vector<const gpu_data::GPUDataElementsMulti*> data;

    inline ShaderProgram_MultiElements(
        shader::Program& shader_program, const std::function<void()> setup_commands
    ) :
        Render_Base(shader_program, setup_commands) {}

    inline virtual ~ShaderProgram_MultiElements() {}

    void virtual render(
        screen_size_t width, screen_size_t height, GLuint framebuffer_ID
    ) override;
};

} // namespace shader

} // namespace gui
