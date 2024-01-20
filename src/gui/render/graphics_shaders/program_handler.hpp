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
 * @ingroup GUI  SHADER
 *
 */

#pragma once

#include "../../../types.hpp"
#include "opengl_program_status.hpp"
#include "uniform.hpp"

#include <GL/glew.h>

#include <compare>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace gui {

class OpenGLProgramExecuter;

namespace shader {

/**
 * @brief File that can be opened.
 *
 * @details Has the file path, and stores the status of the file.
 */
class File {
 private:
    // path to file
    const std::filesystem::path file_;
    // file status
    FileStatus status_;

 public:
    /**
     * @brief Construct a new File object.
     *
     * @param file file path
     */
    inline File(const std::filesystem::path file) :
        file_(std::filesystem::absolute(file)), status_(FileStatus::EMPTY) {}

    /**
     * @brief Returns file contents as a string.
     *
     * @return std::optional<std::string> file contents
     */
    [[nodiscard]] std::optional<std::string> get_file_content();

    /**
     * @brief Get file status.
     *
     * @return FileStatus
     */
    [[nodiscard]] inline FileStatus
    get_status() const {
        return status_;
    }

    /**
     * @brief Get the file path.
     *
     * @return std::filesystem::path file path
     */
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

/**
 * @brief Data that is used to create a shader.
 *
 * @details In particular this holds the file path and the shader type.
 */
class ShaderData {
 protected:
    const std::vector<File> files_;
    GLuint shader_type_;

 public:
    inline ShaderData(const std::vector<File> files, GLuint shader_type) :
        files_(files), shader_type_(shader_type) {}

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const ShaderData& other) const noexcept {
        return files_ <=> other.files_;
    }

    [[nodiscard]] inline bool
    operator==(const ShaderData& other) const noexcept {
        return files_ == other.files_;
    }
};

/**
 * @brief Represents a shader. Part of a render program.
 *
 * @details A shader is one part the the render pipeline.
 */
class Shader : public ShaderData {
 private:
    GLuint shader_ID_;

    ShaderStatus status_;

    std::set<std::pair<std::string, std::string>> found_uniforms_;

 public:
    inline Shader(const std::vector<File> files, GLuint shader_type) :
        ShaderData(files, shader_type), shader_ID_(0), status_(ShaderStatus::EMPTY) {
        reload();
    }

    inline Shader(const ShaderData shader_data) :
        ShaderData(shader_data), shader_ID_(0), status_(ShaderStatus::EMPTY) {
        reload();
    }

    /**
     * @brief Reload this shader.
     *
     * @details Tries to reread the files and compile the shader. Will stop if one of
     * the files is missing or if there is a shader error.
     */
    void reload();

    [[nodiscard]] inline ShaderStatus
    get_status() const {
        return status_;
    }

    [[nodiscard]] inline GLuint
    get_shader_ID() const noexcept {
        return shader_ID_;
    }

    inline auto uniform_begin() {
        return found_uniforms_.begin();
    }
    inline auto uniform_end() {
        return found_uniforms_.end();
    }

};

/**
 * @brief Contains the data to identify and create a program.
 */
class ProgramData {
 protected:
    Shader& vertex_shader_;
    Shader& fragment_shader_;

    /* These aren't implemented because I don't know how to use them.
    std::optional<Shader> geometry_shader_;
    std::optional<Shader> tesselation_control_shader_;
    std::optional<Shader> tesselation_evaluation_shader_;
    */

    std::set<std::pair<std::string, std::string>> found_uniforms_;

    // todo make this a type
    std::map<std::string, GLint> uniforms_;

 public:
    inline ProgramData(Shader& vertex_shader, Shader& fragment_shader) :
        vertex_shader_(vertex_shader), fragment_shader_(fragment_shader) {}

    [[nodiscard]] inline bool
    operator==(const ProgramData& other) const {
        return vertex_shader_ == other.vertex_shader_
               && fragment_shader_ == other.fragment_shader_;
    }

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const ProgramData& other) const noexcept {
        std::strong_ordering vert_order = vertex_shader_ <=> other.vertex_shader_;
        if (vert_order == std::strong_ordering::equivalent) {
            std::strong_ordering frag_order =
                fragment_shader_ <=> other.fragment_shader_;
            return frag_order;
        }
        return vert_order;
    };
};

class Program : public ProgramData {
 private:
    GLuint program_ID_;

    ProgramStatus status_;

    //std::set<OpenGLProgramExecuter*> program_executors_;

    /**
     * @brief An entire OpenGL program pipeline.
     *
     * @details Programs are used to render things in OpenGL.
     */

    // get program status
    [[nodiscard]] GLint inline new_uniform(std::string uniform_name) {
        return glGetUniformLocation(get_program_ID(), uniform_name.c_str());
    }

 public:
    inline Program(Shader& vertex_shader, Shader& fragment_shader) :
        ProgramData(vertex_shader, fragment_shader), program_ID_(0),
        status_(ProgramStatus::EMPTY) {
        reload();
    }

    inline Program(ProgramData program_data) :
        ProgramData(program_data), program_ID_(0), status_(ProgramStatus::EMPTY) {
        reload();
    }

    void reload();

    [[nodiscard]] GLint inline get_uniform(std::string uniform_name) const {
        auto value = uniforms_.find(uniform_name);
        if (value != uniforms_.end()){
            return value->second;
        }
        return 0;
    }

    /**
     * @brief Add OpenGLProgramExecuter so that when the program is updated
     * uniforms will also be updated.
     */
    //inline void
    //add_executor(OpenGLProgramExecuter* program_executor) {
    //    program_executors_.insert(program_executor);
    //}

    /**
     * @brief Remove OpenGLProgramExecuter usually when the executer is being
     * deleted.
     */
    //inline void
    //remove_executor(OpenGLProgramExecuter* program_executor) {
    //    program_executors_.erase(program_executor);
    //}

    /**
     * @brief Return the status of the program.
     *
     * @details Gives information about the status of the compilation of the
     * program.
     */
    [[nodiscard]] inline ProgramStatus
    get_status() const noexcept {
        return status_;
    }

    [[nodiscard]] inline std::set<std::pair<std::string, std::string>>
    get_detected_uniforms() const noexcept {
        return found_uniforms_;
    }

    /**
     * @brief Get the status of the program as a string.
     *
     * @details Returns a pair of strings. The first a short description, and
     * the second an in-depth description of the error.
     */
    [[nodiscard]] const std::pair<std::string, std::string>& get_status_string() const;

    /**
     * @brief Get the ID of the compiled program.
     *
     * @details If the program ID is 0 then the program has not ben compiled.
     */
    [[nodiscard]] inline GLuint
    get_program_ID() const noexcept {
        return program_ID_;
    }

    inline void
    use_program() const {
        glUseProgram(program_ID_);
    }

    void attach_uniforms();
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
 protected:
    // maybe these should be copies?
    //    std::map<const std::filesystem::path, GLuint> shaders;

    std::set<File> files_;

    // I should be tries for the cpp crimes
    std::map<const ShaderData, Shader> shaders_;

    std::map<const ProgramData, Program> programs_;

 public:
    // file extensions are for losers One must pass the type in addition
    /**
     * @brief Get shader program id corresponding to given file path.
     *
     * @details Returns a shader program either from cache or by loading one
     * on success, or 0 on failure.
     */
    Shader& get_shader(const std::vector<File> source_files, GLuint gl_shader_type);

    [[nodiscard]] inline std::map<const ProgramData, Program>&
    get_programs() {
        return programs_;
    }

    /**
     * @brief Load program from given file paths.
     *
     * @details Technically this should be changed so that one can add other
     * types of shaders.
     */
    Program& load_program(
        const std::vector<std::filesystem::path> vertex_file_path,
        const std::vector<std::filesystem::path> fragment_file_path
    );

    /**
     * @brief Load program from files. In this case only one file.
     */
    [[nodiscard]] inline Program&
    load_program(
        const std::filesystem::path vertex_file_path,
        const std::filesystem::path fragment_file_path
    ) {
        return load_program(std::vector({vertex_file_path}), {fragment_file_path});
    };

    /**
     * @brief Deletes all shaders.
     */
    void clear();

    inline ShaderHandler(){};

    inline ~ShaderHandler() { shaders_.clear(); }
};

} // namespace shader

} // namespace gui
