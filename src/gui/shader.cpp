#include "shader.hpp"

#include <GL/glew.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

GLuint
load_shaders(const char* vertex_file, const char* fragment_file) {
    // get the paths
    std::filesystem::path vertex_file_path = std::filesystem::absolute(vertex_file);
    std::filesystem::path fragment_file_path = std::filesystem::absolute(fragment_file);

    // Create the shaders
    GLuint vertex_shader_ID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string vertex_shader_code;
    std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);
    if (vertex_shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << vertex_shader_stream.rdbuf();
        vertex_shader_code = sstr.str();
        vertex_shader_stream.close();
    } else {
        std::cerr << "Impossible to open " << vertex_file_path
                  << ". Are you in the right directory?" << std::endl;
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string fragment_shader_code;
    std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
    if (fragment_shader_stream.is_open()) {
        std::stringstream sstr;
        sstr << fragment_shader_stream.rdbuf();
        fragment_shader_code = sstr.str();
        fragment_shader_stream.close();
    }

    GLint Result = GL_FALSE;
    int info_log_length;

    // Compile Vertex Shader
    std::cout << "Compiling shader : " << vertex_file_path << std::endl;
    char const* vertex_source_pointer = vertex_shader_code.c_str();
    glShaderSource(vertex_shader_ID, 1, &vertex_source_pointer, NULL);
    glCompileShader(vertex_shader_ID);

    // Check Vertex Shader
    glGetShaderiv(vertex_shader_ID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(vertex_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> vertex_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(
            vertex_shader_ID, info_log_length, NULL, &vertex_shader_error_message[0]
        );
        printf("%s\n", &vertex_shader_error_message[0]);
    }

    // Compile Fragment Shader
    std::cout << "Compiling shader : " << fragment_file_path << std::endl;
    char const* fragment_source_pointer = fragment_shader_code.c_str();
    glShaderSource(fragment_shader_ID, 1, &fragment_source_pointer, NULL);
    glCompileShader(fragment_shader_ID);

    // Check Fragment Shader
    glGetShaderiv(fragment_shader_ID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(fragment_shader_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> fragment_shader_error_message(info_log_length + 1);
        glGetShaderInfoLog(
            fragment_shader_ID, info_log_length, NULL, &fragment_shader_error_message[0]
        );
        printf("%s\n", &fragment_shader_error_message[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint program_ID = glCreateProgram();
    glAttachShader(program_ID, vertex_shader_ID);
    glAttachShader(program_ID, fragment_shader_ID);
    glLinkProgram(program_ID);

    // Check the program
    glGetProgramiv(program_ID, GL_LINK_STATUS, &Result);
    glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &info_log_length);
    if (info_log_length > 0) {
        std::vector<char> program_error_message(info_log_length + 1);
        glGetProgramInfoLog(
            program_ID, info_log_length, NULL, &program_error_message[0]
        );
        printf("%s\n", &program_error_message[0]);
    }

    glDetachShader(program_ID, vertex_shader_ID);
    glDetachShader(program_ID, fragment_shader_ID);

    glDeleteShader(vertex_shader_ID);
    glDeleteShader(fragment_shader_ID);

    return program_ID;
}
