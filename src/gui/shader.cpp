#include "shader.hpp"

#include "../logging.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace gui {

std::string
get_shader_string(GLuint gl_shader_type) {
    switch (gl_shader_type) {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_FRAGMENT_SHADER:
            return "fragment";
        case GL_GEOMETRY_SHADER:
            return "geometry";

        default:
            return "NOT A VALID SHADER TYPE";
    }
}

void
ShaderHandler::clear() {
    for (auto it = shaders.begin(); it != shaders.end(); it++) {
        GLuint shader_id = it->second;
        glDeleteShader(shader_id);
    }
    shaders.clear();
}

// public
GLuint
ShaderHandler::get_shader(
    const std::filesystem::path& file_relative_path, GLuint gl_shader_type
) {
    GLuint shader_id;

    auto it = shaders.find(file_relative_path);
    if (it == shaders.end()) {
        shader_id = load_shader(file_relative_path, gl_shader_type);
        if (shader_id != 0) {
            shaders.insert_or_assign(file_relative_path, shader_id);
        }
    } else {
        shader_id = it->second;
    }

    return shader_id;
}

// public
GLuint
ShaderHandler::reload_shader(
    const std::filesystem::path& file_relative_path, GLuint gl_shader_type
) {
    GLuint shader_id;

    auto it = shaders.find(file_relative_path);
    if (it != shaders.end()) {
        shader_id = it->second;
    } else {
        shader_id = load_shader(file_relative_path, gl_shader_type);
        if (shader_id != 0) {
            shaders.insert_or_assign(file_relative_path, shader_id);
        }
        return shader_id;
    }

    GLuint shader_id_new = load_shader(file_relative_path, gl_shader_type);
    // I could log an error, but load_shader should also do this.
    if (shader_id_new != 0) {
        shaders.insert_or_assign(file_relative_path, shader_id_new);
        glDeleteShader(shader_id);
    }

    return shader_id;
}

// private
GLuint
ShaderHandler::load_shader(
    const std::filesystem::path& file_relative_path, GLuint gl_shader_type
) {
    std::filesystem::path file_absolute_path =
        std::filesystem::absolute(file_relative_path);

    std::string shader_type_string = get_shader_string(gl_shader_type);

    // Create the shaders
    GLuint shader_id = glCreateShader(gl_shader_type);

    // Read the Vertex Shader code from the file
    LOG_BACKTRACE(
        logging::opengl_logger, "Loading {} shader from {}", shader_type_string,
        file_absolute_path.string()
    );

    std::string shader_code;
    std::ifstream shader_stream(file_absolute_path, std::ios::in);
    if (shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << shader_stream.rdbuf();
        shader_code = sstr.str();
        shader_stream.close();
    } else {
        LOG_ERROR(
            logging::opengl_logger, "Cannot open {}. Is this the right directory?",
            file_absolute_path.string()
        );
        return 0;
    }

    GLint Result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    LOG_BACKTRACE(
        logging::opengl_logger, "Compiling {} shader {}", shader_type_string,
        file_absolute_path.string()
    );

    char const* source_pointer = shader_code.c_str();
    glShaderSource(shader_id, 1, &source_pointer, NULL);
    glCompileShader(shader_id);

    // Check Vertex Shader
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::string shader_error_message(info_log_length + 1, '\0');
        glGetShaderInfoLog(
            shader_id, info_log_length, NULL, shader_error_message.data()
        );

        LOG_ERROR(
            logging::opengl_logger, "{} shader error: {}", shader_type_string,
            shader_error_message
        );
    }
    return shader_id;
}

// TODO(alem): what do we do on error
GLuint
ShaderHandler::load_program(
    const std::filesystem::path& vertex_file, const std::filesystem::path& fragment_file
) {
    logging::opengl_logger->init_backtrace(4, quill::LogLevel::Error);

    GLint vertex_shader_id = get_shader(vertex_file, GL_VERTEX_SHADER);
    GLint fragment_shader_id = get_shader(fragment_file, GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int info_log_length;

    // Link the program
    // LOG_BACKTRACE(logging::opengl_logger, "Linking shader program");

    GLuint program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);

    // Check the program
    glGetProgramiv(program_id, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::string program_error_message(info_log_length + 1, '\0');
        glGetProgramInfoLog(
            program_id, info_log_length, NULL, program_error_message.data()
        );

        LOG_ERROR(
            logging::opengl_logger, "Shader program linking error: {}",
            program_error_message
        );
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    LOG_INFO(
        logging::opengl_logger, "Shader compiled successfully with program ID {}",
        program_id
    );

    LOG_BACKTRACE(
        logging::opengl_logger, "New Backtrace, because quill won't let me clear it."
    );

    return program_id;
}

} // namespace gui
