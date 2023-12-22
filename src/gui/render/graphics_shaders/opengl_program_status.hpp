#pragma once

#include <cstdint>

namespace gui {

namespace shader {

enum class FileStatus : uint8_t {
    EMPTY = 0,
    OK,
    FILE_NOT_FOUND,
    FILE_IO_ERROR,
};

enum class ShaderStatus : uint8_t {
    EMPTY = 0,
    OK,
    INVALID_FILE,
    COMPILATION_ERROR,
};

enum class ProgramStatus : uint8_t {
    EMPTY = 0,
    OK,
    LINKING_FAILED,
    INVALID_SHADER,
};

} // namespace shader

} // namespace gui
