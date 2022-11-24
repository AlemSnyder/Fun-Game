#pragma once

#include <GL/glew.h>

#include <filesystem>
#include <string>

GLuint
load_shaders(
    const std::filesystem::path& vertex_file_path,
    const std::filesystem::path& fragment_file_path
);
