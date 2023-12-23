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
 * @file shader.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Define ShaderHandler
 *
 * @ingroup GUI
 *
 */

#pragma once

#include "../../../types.hpp"
#include "opengl_program_status.hpp"

#include <GL/glew.h>

#include <compare>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace gui {

class OpenGLProgramExecuter;

namespace shader {

class File {
 private:
    const std::filesystem::path file_;

    FileStatus status_;

 public:
    inline File(const std::filesystem::path file) :
        file_(std::filesystem::absolute(file)), status_(FileStatus::EMPTY) {}

    [[nodiscard]] std::optional<std::string> get_file_content();

    [[nodiscard]] inline FileStatus
    get_status() const {
        return status_;
    }

    [[nodiscard]] inline std::filesystem::path
    get_file_path() const {
        return file_;
    }

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const File& other) const noexcept {
        return file_ <=> other.file_;
    }

    [[nodiscard]] inline bool
    operator==(const File& other) const noexcept {
        return file_ == other.file_;
    }
};

class Shader {
 private:
    const std::vector<File> files_;
    GLuint shader_ID_;
    const GLuint shader_type_;

    ShaderStatus status_;

 public:
    inline Shader(const std::vector<File> files, GLuint shader_type) :
        files_(files), shader_ID_(0), shader_type_(shader_type),
        status_(ShaderStatus::EMPTY) {
        reload();
    }

    void reload();

    [[nodiscard]] inline ShaderStatus
    get_status() const {
        return status_;
    }

    [[nodiscard]] inline GLuint
    get_shader_ID() const noexcept {
        return shader_ID_;
    }

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const Shader& other) const noexcept {
        return files_ <=> other.files_;
    }

    [[nodiscard]] inline bool
    operator==(const Shader& other) const noexcept {
        return files_ == other.files_;
    }
};

class Program {
 private:
    GLuint program_ID_;

    Shader& vertex_shader_;
    Shader& fragment_shader_;

    ProgramStatus status_;

    std::set<OpenGLProgramExecuter*> program_executors_;

    /* These aren't implemented because I don't know how to use them.
    std::optional<Shader> geometry_shader_;
    std::optional<Shader> tesselation_control_shader_;
    std::optional<Shader> tesselation_evaluation_shader_;
    */

 public:
    inline Program(Shader& vertex_shader, Shader& fragment_shader) :
        program_ID_(0), vertex_shader_(vertex_shader),
        fragment_shader_(fragment_shader), status_(ProgramStatus::EMPTY) {
        reload();
    }

    ~Program();

    void reload();

    inline void
    add_executor(OpenGLProgramExecuter* program_executor) {
        program_executors_.insert(program_executor);
    }

    inline void
    remove_executor(OpenGLProgramExecuter* program_executor) {
        program_executors_.erase(program_executor);
    }

    [[nodiscard]] inline ProgramStatus
    get_status() const noexcept {
        return status_;
    }

    [[nodiscard]] inline GLuint
    get_program_ID() const noexcept {
        return program_ID_;
    }

    [[nodiscard]] inline bool
    operator==(const Program& other) const {
        return vertex_shader_ == other.vertex_shader_
               && fragment_shader_ == other.fragment_shader_;
    }

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const Program& other) const noexcept {
        std::strong_ordering vert_order = vertex_shader_ <=> other.vertex_shader_;
        if (vert_order == std::strong_ordering::equivalent) {
            std::strong_ordering frag_order =
                fragment_shader_ <=> other.fragment_shader_;
            return frag_order;
        }
        return vert_order;
    };
};

/**
 * @brief Converts GLuint shader type to human readable string for logging.
 *
 * @details Returns a human readable string for each opengl shader type.
 *
 * @returns std::string string name of shader type
 */
std::string get_shader_string(GLuint gl_shader_type);

/**
 * @brief Loads and saves shader programs
 *
 * @details
 */
class ShaderHandler {
 private:
    // maybe these should be copies?
    //    std::map<const std::filesystem::path, GLuint> shaders;

    std::set<shader::File> files_;

    // I should be tries for the cpp crimes
    std::map<const std::vector<shader::File>, shader::Shader> shaders_;

    std::map<const shader::Program, shader::Program> programs_;

 public:
    // file extensions are for losers One must pass the type in addition
    /**
     * @brief Get shader program id corresponding to given file path.
     *
     * @details Returns a shader program either from cache or by loading one
     * on success, or 0 on failure.
     */
    shader::Shader&
    get_shader(const std::vector<shader::File> source_files, GLuint gl_shader_type);

    const shader::Program& load_program(
        const std::vector<std::filesystem::path> vertex_file_path,
        const std::vector<std::filesystem::path> fragment_file_path
    );

    [[nodiscard]] inline const shader::Program&
    load_program(
        const std::filesystem::path vertex_file_path,
        const std::filesystem::path fragment_file_path
    ) {
        return load_program(std::vector({vertex_file_path}), {fragment_file_path});
    };

    void clear();

    inline ShaderHandler(){};

    // TODO delete programs
    inline ~ShaderHandler() { shaders_.clear(); }
};

} // namespace shader

} // namespace gui
