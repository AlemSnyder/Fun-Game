#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>

namespace gui {

namespace gpu_data {

// TODO enum draw type
// static draw dynamic draw etc

/**
 * @brief Data type of buffer
 */
enum class GPUDataType : GLenum {

    // clang-format off
    BYTE            = GL_BYTE,
    UNSIGNED_BYTE   = GL_UNSIGNED_BYTE,
    SHORT           = GL_SHORT,
    UNSIGNED_SHORT  = GL_UNSIGNED_SHORT,
    INT             = GL_INT,
    UNSIGNED_INT    = GL_UNSIGNED_INT,

    // HALF_FLOAT =  GL_HALF_FLOAT,
    FLOAT           = GL_FLOAT,
    DOUBLE          = GL_DOUBLE,
    // FIXED =  GL_FIXED,
    // INT_2_10_10_10_REV =  GL_INT_2_10_10_10_REV,
    // UNSIGNED_INT_2_10_10_10_REV =  GL_UNSIGNED_INT_2_10_10_10_REV,
    // UNSIGNED_INT_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV,
    // clang-format on
};

/**
 * @brief Targe. Designates how the buffer is used.
 */
enum class BindingTarget : GLenum {
    ARRAY_BUFFER = GL_ARRAY_BUFFER,
    // ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER
    // COPY_READ_BUFFER = GL_COPY_READ_BUFFER
    // COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER
    // DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER
    // DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER
    ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
    // PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER
    // PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER
    // QUERY_BUFFER = GL_QUERY_BUFFER
    // SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER
    // TEXTURE_BUFFER = GL_TEXTURE_BUFFER
    // TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER
    // UNIFORM_BUFFER = GL_UNIFORM_BUFFER
};

enum class ShaderType : GLenum {
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
    //    TESS_CONTROL_SHADER = GL_TESS_CONTROL_SHADER,
    //    TESS_EVALUATION_SHADER = GL_TESS_EVALUATION_SHADER,
    //    COMPUTE_SHADER = GL_COMPUTE_SHADER,
};

/**
 * @brief String representation of GPUDataType
 *
 * @param const GPUDataType& data type to represent
 */
constexpr inline std::string
to_string(const GPUDataType& data_type) {
    switch (data_type) {
        case GPUDataType::BYTE:
            return "BYTE";
        case GPUDataType::UNSIGNED_BYTE:
            return "UNSIGNED_BYTE";
        case GPUDataType::SHORT:
            return "SHORT";
        case GPUDataType::UNSIGNED_SHORT:
            return "UNSIGNED_SHORT";
        case GPUDataType::INT:
            return "INT";
        case GPUDataType::UNSIGNED_INT:
            return "UNSIGNED_INT";
        case GPUDataType::FLOAT:
            return "FLOAT";
        case GPUDataType::DOUBLE:
            return "DOUBLE";
        default:
            abort();
    }
}

} // namespace gpu_data

} // namespace gui
