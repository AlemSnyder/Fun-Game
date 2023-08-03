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

#include <GL/glew.h>

#include <filesystem>
#include <map>
#include <string>

namespace gui {
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
    // maybe these should be coppies?
    std::map<const std::filesystem::path, GLuint> shaders;

 public:
    // file extensions are for loosers. One must pass the type inaddition
    GLuint get_shader(const std::filesystem::path& file_path, GLuint gl_shader_type);

    GLuint load_program(
        const std::filesystem::path& vertex_file_path,
        const std::filesystem::path& fragment_file_path
    );

    void clear();

    ShaderHandler();
    ~ShaderHandler();

 private:
    GLuint load_shader(const std::filesystem::path& file_path, GLuint gl_shader_type);

    GLuint reload_shader(const std::filesystem::path& file_path, GLuint gl_shader_type);
};

} // namespace gui
