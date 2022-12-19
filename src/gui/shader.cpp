#include "shader.hpp"

#include "../logging.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// TODO(alem): what do we do on error
GLuint
load_shaders(
    const std::filesystem::path& vertex_file, const std::filesystem::path& fragment_file
)
{
    // get the paths
    std::filesystem::path vertex_file_path = std::filesystem::absolute(vertex_file);
    std::filesystem::path fragment_file_path = std::filesystem::absolute(fragment_file);

    // Create the shaders
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    LOG_INFO(
        logging::opengl_logger, "Loading vertex shader from {}",
        vertex_file_path.lexically_normal().string()
    );

    std::string vertex_shader_code;
    std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);
    if (vertex_shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << vertex_shader_stream.rdbuf();
        vertex_shader_code = sstr.str();
        vertex_shader_stream.close();
    } else {
        LOG_ERROR(
            logging::opengl_logger, "Cannot open {}. Are you in the right directory?",
            vertex_file_path.lexically_normal().string()
        );
        return 0;
    }

    // Read the Fragment Shader code from the file
    LOG_INFO(
        logging::opengl_logger, "Loading fragment shader from {}",
        vertex_file_path.lexically_normal().string()
    );

    std::string fragment_shader_code;
    std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
    if (fragment_shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << fragment_shader_stream.rdbuf();
        fragment_shader_code = sstr.str();
        fragment_shader_stream.close();
    } else {
        LOG_ERROR(
            logging::opengl_logger, "Cannot open {}. Are you in the right directory?",
            fragment_file_path.lexically_normal().string()
        );
        return 0;
    }

    GLint Result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    LOG_DEBUG(
        logging::opengl_logger, "Compiling vertex shader {}",
        vertex_file_path.lexically_normal().string()
    );

    char const* vertex_source_pointer = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_id, 1, &vertex_source_pointer, NULL);
    glCompileShader(vertex_shader_id);

    // Check Vertex Shader
    glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::string vertex_shader_error_message(info_log_length + 1, '\0');
        glGetShaderInfoLog(
            vertex_shader_id, info_log_length, NULL, vertex_shader_error_message.data()
        );

        LOG_WARNING(
            logging::opengl_logger, "Vertex shader error: {}",
            vertex_shader_error_message
        );
    }

    // Compile Fragment Shader
    LOG_DEBUG(
        logging::opengl_logger, "Compiling fragment shader {}",
        fragment_file_path.lexically_normal().string()
    );

    char const* fragment_source_pointer = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_id, 1, &fragment_source_pointer, NULL);
    glCompileShader(fragment_shader_id);

    // Check Fragment Shader
    glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::string fragment_shader_error_message(info_log_length + 1, '\0');
        glGetShaderInfoLog(
            fragment_shader_id, info_log_length, NULL,
            fragment_shader_error_message.data()
        );

        LOG_WARNING(
            logging::opengl_logger, "Vertex shader error: {}",
            fragment_shader_error_message
        );
    }

    // Link the program
    LOG_DEBUG(logging::opengl_logger, "Linking shader program");

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

        LOG_WARNING(
            logging::opengl_logger, "Shader program linking error: {}",
            program_error_message
        );
    }

    glDetachShader(program_id, vertex_shader_id);
    glDetachShader(program_id, fragment_shader_id);

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    LOG_INFO(
        logging::opengl_logger, "Shader compiled successfully with program ID {}",
        program_id
    );

    return program_id;
}
