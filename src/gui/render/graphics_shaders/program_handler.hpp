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
 * @file program_handler.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Define Shader Program related classes
 *
 * @ingroup GUI  SHADER
 *
 */

#pragma once

#include "../gl_enums.hpp"
#include "logging.hpp"
#include "opengl_program_status.hpp"
#include "types.hpp"
#include "uniform.hpp"
#include "util/hash_combine.hpp"

#include <GL/glew.h>

#include <compare>
#include <filesystem>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#define NO_UNIFORM_INT -1

namespace gui {

namespace shader {

class File;
class ShaderData;
class ProgramData;

} // namespace shader
} // namespace gui

template <>
struct std::hash<gui::shader::File>;

template <>
struct std::hash<gui::shader::ShaderData>;

template <>
struct std::hash<gui::shader::ProgramData>;

namespace gui {

namespace shader {

/**
 * @brief File that can be opened.
 *
 * @details Has the file path, and stores the status of the file.
 */
class File {
    friend std::hash<gui::shader::File>;

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
    friend std::hash<gui::shader::ShaderData>;

 protected:
    std::vector<File> files_;
    gpu_data::ShaderType shader_type_;

 public:
    inline ShaderData(const std::vector<File> files, gpu_data::ShaderType shader_type) :
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

 public:
    inline Shader(const std::vector<File> files, gpu_data::ShaderType shader_type) :
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
};

/**
 * @brief Contains the data to identify and create a program.
 */
class ProgramData {
    friend std::hash<gui::shader::ProgramData>;

 protected:
    Shader& vertex_shader_;
    Shader& fragment_shader_;

    /* These aren't implemented because I don't know how to use them.
    std::optional<Shader> geometry_shader_;
    std::optional<Shader> tesselation_control_shader_;
    std::optional<Shader> tesselation_evaluation_shader_;
    */

    // todo make this a type
    std::unordered_map<std::string, Uniform> uniforms_;

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

/**
 * @brief An entire OpenGL program pipeline.
 *
 * @details Programs are used to render things in OpenGL.
 */
class Program : public ProgramData {
 private:
    GLuint program_ID_;

    ProgramStatus status_;

    std::string name_;

 public:
    /**
     * @brief Crate a new Program object from a vertex and a fragment shader
     *
     * @param vertex_shader Shader the vertex shader
     * @param fragment_shader Shader the fragment shader
     */
    inline Program(std::string name, Shader& vertex_shader, Shader& fragment_shader) :
        ProgramData(vertex_shader, fragment_shader), program_ID_(0),
        status_(ProgramStatus::EMPTY), name_(name) {
        reload();
    }

    /**
     * @brief Crate a Program copy constructor.
     *
     * @details I'm not sure how save this is. Should probably be avoided.
     */
    inline Program(std::string name, ProgramData program_data) :
        ProgramData(program_data), program_ID_(0), status_(ProgramStatus::EMPTY),
        name_(name) {
        reload();
    }

    /**
     * @brief Reload the program.
     *
     * @details Re-compiles the program. Will also recompile vertex and
     * fragment shaders if they previously failed. Remember to read the log
     * out put as it will tell you what went wrong.
     */
    void reload();

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

    /**
     * @brief Get the program name
     *
     * @return std::string program name
     */
    [[nodiscard]] inline std::string
    get_name() const noexcept {
        return name_;
    }

    /**
     * @brief Bind the current Opengl program to this one.
     */
    inline void
    use_program() const {
        glUseProgram(program_ID_);
    }

    /**
     * @brief Send uniform data for this program.
     */
    void
    bind_uniforms() {
        for (const auto& uniform : uniforms_) {
            uniform.second.bind();
        }
    }

    /**
     * @brief Generate uniforms for this program
     */
    void attach_uniforms();

    // Set uniform executor
    void set_uniform(std::shared_ptr<UniformExecutor> uex, std::string uniform_name);
};

/**
 * @brief Converts ShaderType shader type to human readable string for logging.
 *
 * @details Returns a human readable string for each opengl shader type.
 *
 * @returns std::string string name of shader type
 */
[[nodiscard]] inline static std::string
get_shader_string(gpu_data::ShaderType gl_shader_type) {
    switch (gl_shader_type) {
        case gpu_data::ShaderType::VERTEX_SHADER:
            return "vertex";
        case gpu_data::ShaderType::FRAGMENT_SHADER:
            return "fragment";
        case gpu_data::ShaderType::GEOMETRY_SHADER:
            return "geometry";
        default:
            abort();
    }
}

} // namespace shader

} // namespace gui

template <>
struct std::hash<gui::shader::File> {
    size_t
    operator()(const gui::shader::File& file) const noexcept {
        std::hash<std::filesystem::path> hasher;
        return hasher(file.file_);
    }
};

template <>
struct std::hash<std::vector<gui::shader::File>> {
    size_t
    operator()(const std::vector<gui::shader::File>& files) const noexcept {
        size_t start = 0;
        for (const auto& file : files) {
            utils::hash_combine(start, file);
        }
        return start;
    }
};

template <>
struct std::hash<gui::shader::ShaderData> {
    size_t
    operator()(const gui::shader::ShaderData& shader) const noexcept {
        std::hash<std::vector<gui::shader::File>> hasher;
        return hasher(shader.files_);
    }
};

template <>
struct std::hash<gui::shader::Shader> {
    size_t
    operator()(const gui::shader::Shader& shader) const noexcept {
        std::hash<gui::shader::ShaderData> hasher;
        return hasher(shader);
    }
};

template <>
struct std::hash<gui::shader::ProgramData> {
    size_t
    operator()(const gui::shader::ProgramData& program) const noexcept {
        size_t start = 0;
        utils::hash_combine(start, gui::shader::ShaderData(program.vertex_shader_));
        utils::hash_combine(start, gui::shader::ShaderData(program.fragment_shader_));

        return start;
    }
};

namespace gui {

namespace shader {

/**
 * @brief Loads and saves shader programs
 *
 * @details
 */
class ShaderHandler {
 protected:
    std::unordered_set<File> files_;

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
    Shader& get_shader(
        const std::vector<File> source_files, gpu_data::ShaderType gl_shader_type
    );

    /**
     * @brief Get all programs associated with this program handler
     *
     * @details Maybe don't use this. Only used by IMGUI interface.
     */
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
        std::string name, const std::vector<std::filesystem::path> vertex_file_path,
        const std::vector<std::filesystem::path> fragment_file_path
    );

    /**
     * @brief Load program from files. In this case only one file.
     */
    [[nodiscard]] inline Program&
    load_program(
        std::string name, const std::filesystem::path vertex_file_path,
        const std::filesystem::path fragment_file_path
    ) {
        return load_program(
            name, std::vector({vertex_file_path}), {fragment_file_path}
        );
    };

    /**
     * @brief Deletes all shaders.
     */
    void clear();

    /**
     * @brief construct a new ShaderHandler
     */
    inline ShaderHandler(){};

    inline ~ShaderHandler() {}
};

} // namespace shader

} // namespace gui
