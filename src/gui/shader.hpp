#pragma once

#include <GL/glew.h>

#include <filesystem>
#include <string>
#include <map>

namespace gui{

std::string get_shader_string(GLuint gl_shader_type);

class ShaderHandeler{
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

    ShaderHandeler();
    ~ShaderHandeler();

 private:
    GLuint load_shader(const std::filesystem::path& file_path, GLuint gl_shader_type);

    GLuint reload_shader(const std::filesystem::path& file_path, GLuint gl_shader_type);

};

}
