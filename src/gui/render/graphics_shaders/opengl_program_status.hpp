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
 * @file opengl_program_status.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines program statuses
 *
 * @ingroup GUI  RENDER  GRAPHICS_SHADERS
 *
 */

#pragma once

#include <cstdint>

namespace gui {

namespace shader {

enum class FileStatus : uint8_t {
    OK = 0,
    EMPTY,
    FILE_NOT_FOUND,
    FILE_IO_ERROR,
};

enum class ShaderStatus : uint8_t {
    OK = 0,
    EMPTY,
    INVALID_FILE,
    COMPILATION_ERROR,
};

enum class ProgramStatus : uint8_t {
    OK = 0,
    EMPTY,
    LINKING_FAILED,
    INVALID_SHADER,
};

} // namespace shader

} // namespace gui
