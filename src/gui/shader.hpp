#pragma once

#include <GL/glew.h>

#include <string>

GLuint load_shaders(const std::string vertex_file_path,
                    const std::string fragment_file_path);
