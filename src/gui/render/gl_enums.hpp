#pragma once

#include "exceptions.hpp"
#include "logging.hpp"

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
    // FIXED =  GL_FIXED,
    // INT_2_10_10_10_REV =  GL_INT_2_10_10_10_REV,
    // UNSIGNED_INT_2_10_10_10_REV =  GL_UNSIGNED_INT_2_10_10_10_REV,
    // UNSIGNED_INT_10F_11F_11F_REV = GL_UNSIGNED_INT_10F_11F_11F_REV,
    // clang-format on

    FLOAT = GL_FLOAT,                                     //	float
    FLOAT_VEC2 = GL_FLOAT_VEC2,                           // 	vec2
    FLOAT_VEC3 = GL_FLOAT_VEC3,                           // 	vec3
    FLOAT_VEC4 = GL_FLOAT_VEC4,                           // 	vec4
    DOUBLE = GL_DOUBLE,                                   // 	double
    DOUBLE_VEC2 = GL_DOUBLE_VEC2,                         // 	dvec2
    DOUBLE_VEC3 = GL_DOUBLE_VEC3,                         // 	dvec3
    DOUBLE_VEC4 = GL_DOUBLE_VEC4,                         // 	dvec4
    INT = GL_INT,                                         // 	int
    INT_VEC2 = GL_INT_VEC2,                               // 	ivec2
    INT_VEC3 = GL_INT_VEC3,                               // 	ivec3
    INT_VEC4 = GL_INT_VEC4,                               // 	ivec4
    UNSIGNED_INT = GL_UNSIGNED_INT,                       // 	unsigned int
    UNSIGNED_INT_VEC2 = GL_UNSIGNED_INT_VEC2,             // 	uvec2
    UNSIGNED_INT_VEC3 = GL_UNSIGNED_INT_VEC3,             // 	uvec3
    UNSIGNED_INT_VEC4 = GL_UNSIGNED_INT_VEC4,             // 	uvec4
    BOOL = GL_BOOL,                                       // 	bool
    BOOL_VEC2 = GL_BOOL_VEC2,                             // 	bvec2
    BOOL_VEC3 = GL_BOOL_VEC3,                             // 	bvec3
    BOOL_VEC4 = GL_BOOL_VEC4,                             // 	bvec4
    FLOAT_MAT2 = GL_FLOAT_MAT2,                           // 	mat2
    FLOAT_MAT3 = GL_FLOAT_MAT3,                           // 	mat3
    FLOAT_MAT4 = GL_FLOAT_MAT4,                           // 	mat4
    FLOAT_MAT2x3 = GL_FLOAT_MAT2x3,                       // 	mat2x3
    FLOAT_MAT2x4 = GL_FLOAT_MAT2x4,                       // 	mat2x4
    FLOAT_MAT3x2 = GL_FLOAT_MAT3x2,                       // 	mat3x2
    FLOAT_MAT3x4 = GL_FLOAT_MAT3x4,                       // 	mat3x4
    FLOAT_MAT4x2 = GL_FLOAT_MAT4x2,                       // 	mat4x2
    FLOAT_MAT4x3 = GL_FLOAT_MAT4x3,                       // 	mat4x3
    DOUBLE_MAT2 = GL_DOUBLE_MAT2,                         // 	dmat2
    DOUBLE_MAT3 = GL_DOUBLE_MAT3,                         // 	dmat3
    DOUBLE_MAT4 = GL_DOUBLE_MAT4,                         // 	dmat4
    DOUBLE_MAT2x3 = GL_DOUBLE_MAT2x3,                     // 	dmat2x3
    DOUBLE_MAT2x4 = GL_DOUBLE_MAT2x4,                     // 	dmat2x4
    DOUBLE_MAT3x2 = GL_DOUBLE_MAT3x2,                     // 	dmat3x2
    DOUBLE_MAT3x4 = GL_DOUBLE_MAT3x4,                     // 	dmat3x4
    DOUBLE_MAT4x2 = GL_DOUBLE_MAT4x2,                     // 	dmat4x2
    DOUBLE_MAT4x3 = GL_DOUBLE_MAT4x3,                     // 	dmat4x3
    SAMPLER_1D = GL_SAMPLER_1D,                           // 	sampler1D
    SAMPLER_2D = GL_SAMPLER_2D,                           // 	sampler2D
    SAMPLER_3D = GL_SAMPLER_3D,                           // 	sampler3D
    SAMPLER_CUBE = GL_SAMPLER_CUBE,                       // 	samplerCube
    SAMPLER_1D_SHADOW = GL_SAMPLER_1D_SHADOW,             // 	sampler1DShadow
    SAMPLER_2D_SHADOW = GL_SAMPLER_2D_SHADOW,             // 	sampler2DShadow
    SAMPLER_1D_ARRAY = GL_SAMPLER_1D_ARRAY,               // 	sampler1DArray
    SAMPLER_2D_ARRAY = GL_SAMPLER_2D_ARRAY,               // 	sampler2DArray
    SAMPLER_1D_ARRAY_SHADOW = GL_SAMPLER_1D_ARRAY_SHADOW, // 	sampler1DArrayShadow
    SAMPLER_2D_ARRAY_SHADOW = GL_SAMPLER_2D_ARRAY_SHADOW, // 	sampler2DArrayShadow
    SAMPLER_2D_MULTISAMPLE = GL_SAMPLER_2D_MULTISAMPLE,   // 	sampler2DMS
    SAMPLER_2D_MULTISAMPLE_ARRAY = GL_SAMPLER_2D_MULTISAMPLE_ARRAY, // 	sampler2DMSArray
    SAMPLER_CUBE_SHADOW = GL_SAMPLER_CUBE_SHADOW,               // 	samplerCubeShadow
    SAMPLER_BUFFER = GL_SAMPLER_BUFFER,                         // 	samplerBuffer
    SAMPLER_2D_RECT = GL_SAMPLER_2D_RECT,                       // 	sampler2DRect
    SAMPLER_2D_RECT_SHADOW = GL_SAMPLER_2D_RECT_SHADOW,         // 	sampler2DRectShadow
    INT_SAMPLER_1D = GL_INT_SAMPLER_1D,                         // 	isampler1D
    INT_SAMPLER_2D = GL_INT_SAMPLER_2D,                         // 	isampler2D
    INT_SAMPLER_3D = GL_INT_SAMPLER_3D,                         // 	isampler3D
    INT_SAMPLER_CUBE = GL_INT_SAMPLER_CUBE,                     // 	isamplerCube
    INT_SAMPLER_1D_ARRAY = GL_INT_SAMPLER_1D_ARRAY,             // 	isampler1DArray
    INT_SAMPLER_2D_ARRAY = GL_INT_SAMPLER_2D_ARRAY,             // 	isampler2DArray
    INT_SAMPLER_2D_MULTISAMPLE = GL_INT_SAMPLER_2D_MULTISAMPLE, // 	isampler2DMS
    INT_SAMPLER_2D_MULTISAMPLE_ARRAY =
        GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,                  // 	isampler2DMSArray
    INT_SAMPLER_BUFFER = GL_INT_SAMPLER_BUFFER,               // 	isamplerBuffer
    INT_SAMPLER_2D_RECT = GL_INT_SAMPLER_2D_RECT,             // 	isampler2DRect
    UNSIGNED_INT_SAMPLER_1D = GL_UNSIGNED_INT_SAMPLER_1D,     // 	usampler1D
    UNSIGNED_INT_SAMPLER_2D = GL_UNSIGNED_INT_SAMPLER_2D,     // 	usampler2D
    UNSIGNED_INT_SAMPLER_3D = GL_UNSIGNED_INT_SAMPLER_3D,     // 	usampler3D
    UNSIGNED_INT_SAMPLER_CUBE = GL_UNSIGNED_INT_SAMPLER_CUBE, // 	usamplerCube
    UNSIGNED_INT_SAMPLER_1D_ARRAY =
        GL_UNSIGNED_INT_SAMPLER_1D_ARRAY, // 	usampler2DArray
    UNSIGNED_INT_SAMPLER_2D_ARRAY =
        GL_UNSIGNED_INT_SAMPLER_2D_ARRAY, // 	usampler2DArray
    UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE =
        GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE, // 	usampler2DMS
    UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY =
        GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,             // 	usampler2DMSArray
    UNSIGNED_INT_SAMPLER_BUFFER = GL_UNSIGNED_INT_SAMPLER_BUFFER, // 	usamplerBuffer
    UNSIGNED_INT_SAMPLER_2D_RECT = GL_UNSIGNED_INT_SAMPLER_2D_RECT, // 	usampler2DRect
    IMAGE_1D = GL_IMAGE_1D,                                         // 	image1D
    IMAGE_2D = GL_IMAGE_2D,                                         // 	image2D
    IMAGE_3D = GL_IMAGE_3D,                                         // 	image3D
    IMAGE_2D_RECT = GL_IMAGE_2D_RECT,                               // 	image2DRect
    IMAGE_CUBE = GL_IMAGE_CUBE,                                     // 	imageCube
    IMAGE_BUFFER = GL_IMAGE_BUFFER,                                 // 	imageBuffer
    IMAGE_1D_ARRAY = GL_IMAGE_1D_ARRAY,                             // 	image1DArray
    IMAGE_2D_ARRAY = GL_IMAGE_2D_ARRAY,                             // 	image2DArray
    IMAGE_2D_MULTISAMPLE = GL_IMAGE_2D_MULTISAMPLE,                 // 	image2DMS
    IMAGE_2D_MULTISAMPLE_ARRAY = GL_IMAGE_2D_MULTISAMPLE_ARRAY,     // 	image2DMSArray
    INT_IMAGE_1D = GL_INT_IMAGE_1D,                                 // 	iimage1D
    INT_IMAGE_2D = GL_INT_IMAGE_2D,                                 // 	iimage2D
    INT_IMAGE_3D = GL_INT_IMAGE_3D,                                 // 	iimage3D
    INT_IMAGE_2D_RECT = GL_INT_IMAGE_2D_RECT,                       // 	iimage2DRect
    INT_IMAGE_CUBE = GL_INT_IMAGE_CUBE,                             // 	iimageCube
    INT_IMAGE_BUFFER = GL_INT_IMAGE_BUFFER,                         // 	iimageBuffer
    INT_IMAGE_1D_ARRAY = GL_INT_IMAGE_1D_ARRAY,                     // 	iimage1DArray
    INT_IMAGE_2D_ARRAY = GL_INT_IMAGE_2D_ARRAY,                     // 	iimage2DArray
    INT_IMAGE_2D_MULTISAMPLE = GL_INT_IMAGE_2D_MULTISAMPLE,         // 	iimage2DMS
    INT_IMAGE_2D_MULTISAMPLE_ARRAY =
        GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,                        // 	iimage2DMSArray
    UNSIGNED_INT_IMAGE_1D = GL_UNSIGNED_INT_IMAGE_1D,             // 	uimage1D
    UNSIGNED_INT_IMAGE_2D = GL_UNSIGNED_INT_IMAGE_2D,             // 	uimage2D
    UNSIGNED_INT_IMAGE_3D = GL_UNSIGNED_INT_IMAGE_3D,             // 	uimage3D
    UNSIGNED_INT_IMAGE_2D_RECT = GL_UNSIGNED_INT_IMAGE_2D_RECT,   // 	uimage2DRect
    UNSIGNED_INT_IMAGE_CUBE = GL_UNSIGNED_INT_IMAGE_CUBE,         // 	uimageCube
    UNSIGNED_INT_IMAGE_BUFFER = GL_UNSIGNED_INT_IMAGE_BUFFER,     // 	uimageBuffer
    UNSIGNED_INT_IMAGE_1D_ARRAY = GL_UNSIGNED_INT_IMAGE_1D_ARRAY, // 	uimage1DArray
    UNSIGNED_INT_IMAGE_2D_ARRAY = GL_UNSIGNED_INT_IMAGE_2D_ARRAY, // 	uimage2DArray
    UNSIGNED_INT_IMAGE_2D_MULTISAMPLE =
        GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE, // 	uimage2DMS
    UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY =
        GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,               // 	uimage2DMSArray
    UNSIGNED_INT_ATOMIC_COUNTER = GL_UNSIGNED_INT_ATOMIC_COUNTER, //
    NUM_TYPES
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
            return "GL_BYTE";
        case GPUDataType::UNSIGNED_BYTE:
            return "GL_UNSIGNED_BYTE";
        case GPUDataType::SHORT:
            return "GL_SHORT";
        case GPUDataType::UNSIGNED_SHORT:
            return "GL_UNSIGNED_SHORT";
        case GPUDataType::FLOAT:
            return "GL_FLOAT"; //	float
        case GPUDataType::FLOAT_VEC2:
            return "GL_FLOAT_VEC2"; // 	vec2
        case GPUDataType::FLOAT_VEC3:
            return "GL_FLOAT_VEC3"; // 	vec3
        case GPUDataType::FLOAT_VEC4:
            return "GL_FLOAT_VEC4"; // 	vec4
        case GPUDataType::DOUBLE:
            return "GL_DOUBLE"; // 	double
        case GPUDataType::DOUBLE_VEC2:
            return "GL_DOUBLE_VEC2"; // 	dvec2
        case GPUDataType::DOUBLE_VEC3:
            return "GL_DOUBLE_VEC3"; // 	dvec3
        case GPUDataType::DOUBLE_VEC4:
            return "GL_DOUBLE_VEC4"; // 	dvec4
        case GPUDataType::INT:
            return "GL_INT"; // 	int
        case GPUDataType::INT_VEC2:
            return "GL_INT_VEC2"; // 	ivec2
        case GPUDataType::INT_VEC3:
            return "GL_INT_VEC3"; // 	ivec3
        case GPUDataType::INT_VEC4:
            return "GL_INT_VEC4"; // 	ivec4
        case GPUDataType::UNSIGNED_INT:
            return "GL_UNSIGNED_INT"; // 	unsigned int
        case GPUDataType::UNSIGNED_INT_VEC2:
            return "GL_UNSIGNED_INT_VEC2"; // 	uvec2
        case GPUDataType::UNSIGNED_INT_VEC3:
            return "GL_UNSIGNED_INT_VEC3"; // 	uvec3
        case GPUDataType::UNSIGNED_INT_VEC4:
            return "GL_UNSIGNED_INT_VEC4"; // 	uvec4
        case GPUDataType::BOOL:
            return "GL_BOOL"; // 	bool
        case GPUDataType::BOOL_VEC2:
            return "GL_BOOL_VEC2"; // 	bvec2
        case GPUDataType::BOOL_VEC3:
            return "GL_BOOL_VEC3"; // 	bvec3
        case GPUDataType::BOOL_VEC4:
            return "GL_BOOL_VEC4"; // 	bvec4
        case GPUDataType::FLOAT_MAT2:
            return "GL_FLOAT_MAT2"; // 	mat2
        case GPUDataType::FLOAT_MAT3:
            return "GL_FLOAT_MAT3"; // 	mat3
        case GPUDataType::FLOAT_MAT4:
            return "GL_FLOAT_MAT4"; // 	mat4
        case GPUDataType::FLOAT_MAT2x3:
            return "GL_FLOAT_MAT2x3"; // 	mat2x3
        case GPUDataType::FLOAT_MAT2x4:
            return "GL_FLOAT_MAT2x4"; // 	mat2x4
        case GPUDataType::FLOAT_MAT3x2:
            return "GL_FLOAT_MAT3x2"; // 	mat3x2
        case GPUDataType::FLOAT_MAT3x4:
            return "GL_FLOAT_MAT3x4"; // 	mat3x4
        case GPUDataType::FLOAT_MAT4x2:
            return "GL_FLOAT_MAT4x2"; // 	mat4x2
        case GPUDataType::FLOAT_MAT4x3:
            return "GL_FLOAT_MAT4x3"; // 	mat4x3
        case GPUDataType::DOUBLE_MAT2:
            return "GL_DOUBLE_MAT2"; // 	dmat2
        case GPUDataType::DOUBLE_MAT3:
            return "GL_DOUBLE_MAT3"; // 	dmat3
        case GPUDataType::DOUBLE_MAT4:
            return "GL_DOUBLE_MAT4"; // 	dmat4
        case GPUDataType::DOUBLE_MAT2x3:
            return "GL_DOUBLE_MAT2x3"; // 	dmat2x3
        case GPUDataType::DOUBLE_MAT2x4:
            return "GL_DOUBLE_MAT2x4"; // 	dmat2x4
        case GPUDataType::DOUBLE_MAT3x2:
            return "GL_DOUBLE_MAT3x2"; // 	dmat3x2
        case GPUDataType::DOUBLE_MAT3x4:
            return "GL_DOUBLE_MAT3x4"; // 	dmat3x4
        case GPUDataType::DOUBLE_MAT4x2:
            return "GL_DOUBLE_MAT4x2"; // 	dmat4x2
        case GPUDataType::DOUBLE_MAT4x3:
            return "GL_DOUBLE_MAT4x3"; // 	dmat4x3
        case GPUDataType::SAMPLER_1D:
            return "GL_SAMPLER_1D"; // 	sampler1D
        case GPUDataType::SAMPLER_2D:
            return "GL_SAMPLER_2D"; // 	sampler2D
        case GPUDataType::SAMPLER_3D:
            return "GL_SAMPLER_3D"; // 	sampler3D
        case GPUDataType::SAMPLER_CUBE:
            return "GL_SAMPLER_CUBE"; // 	samplerCube
        case GPUDataType::SAMPLER_1D_SHADOW:
            return "GL_SAMPLER_1D_SHADOW"; // 	sampler1DShadow
        case GPUDataType::SAMPLER_2D_SHADOW:
            return "GL_SAMPLER_2D_SHADOW"; // 	sampler2DShadow
        case GPUDataType::SAMPLER_1D_ARRAY:
            return "GL_SAMPLER_1D_ARRAY"; // 	sampler1DArray
        case GPUDataType::SAMPLER_2D_ARRAY:
            return "GL_SAMPLER_2D_ARRAY"; // 	sampler2DArray
        case GPUDataType::SAMPLER_1D_ARRAY_SHADOW:
            return "GL_SAMPLER_1D_ARRAY_SHADOW"; // 	sampler1DArrayShadow
        case GPUDataType::SAMPLER_2D_ARRAY_SHADOW:
            return "GL_SAMPLER_2D_ARRAY_SHADOW"; // 	sampler2DArrayShadow
        case GPUDataType::SAMPLER_2D_MULTISAMPLE:
            return "GL_SAMPLER_2D_MULTISAMPLE"; // 	sampler2DMS
        case GPUDataType::SAMPLER_2D_MULTISAMPLE_ARRAY:
            return "GL_SAMPLER_2D_MULTISAMPLE_ARRAY"; // 	sampler2DMSArray
        case GPUDataType::SAMPLER_CUBE_SHADOW:
            return "GL_SAMPLER_CUBE_SHADOW"; // 	samplerCubeShadow
        case GPUDataType::SAMPLER_BUFFER:
            return "GL_SAMPLER_BUFFER"; // 	samplerBuffer
        case GPUDataType::SAMPLER_2D_RECT:
            return "GL_SAMPLER_2D_RECT"; // 	sampler2DRect
        case GPUDataType::SAMPLER_2D_RECT_SHADOW:
            return "GL_SAMPLER_2D_RECT_SHADOW"; // 	sampler2DRectShadow
        case GPUDataType::INT_SAMPLER_1D:
            return "GL_INT_SAMPLER_1D"; // 	isampler1D
        case GPUDataType::INT_SAMPLER_2D:
            return "GL_INT_SAMPLER_2D"; // 	isampler2D
        case GPUDataType::INT_SAMPLER_3D:
            return "GL_INT_SAMPLER_3D"; // 	isampler3D
        case GPUDataType::INT_SAMPLER_CUBE:
            return "GL_INT_SAMPLER_CUBE"; // 	isamplerCube
        case GPUDataType::INT_SAMPLER_1D_ARRAY:
            return "GL_INT_SAMPLER_1D_ARRAY"; // 	isampler1DArray
        case GPUDataType::INT_SAMPLER_2D_ARRAY:
            return "GL_INT_SAMPLER_2D_ARRAY"; // 	isampler2DArray
        case GPUDataType::INT_SAMPLER_2D_MULTISAMPLE:
            return "GL_INT_SAMPLER_2D_MULTISAMPLE"; // 	isampler2DMS
        case GPUDataType::INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            return "GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY"; // 	isampler2DMSArray
        case GPUDataType::INT_SAMPLER_BUFFER:
            return "GL_INT_SAMPLER_BUFFER"; // 	isamplerBuffer
        case GPUDataType::INT_SAMPLER_2D_RECT:
            return "GL_INT_SAMPLER_2D_RECT"; // 	isampler2DRect
        case GPUDataType::UNSIGNED_INT_SAMPLER_1D:
            return "GL_UNSIGNED_INT_SAMPLER_1D"; // 	usampler1D
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D:
            return "GL_UNSIGNED_INT_SAMPLER_2D"; // 	usampler2D
        case GPUDataType::UNSIGNED_INT_SAMPLER_3D:
            return "GL_UNSIGNED_INT_SAMPLER_3D"; // 	usampler3D
        case GPUDataType::UNSIGNED_INT_SAMPLER_CUBE:
            return "GL_UNSIGNED_INT_SAMPLER_CUBE"; // 	usamplerCube
        case GPUDataType::UNSIGNED_INT_SAMPLER_1D_ARRAY:
            return "GL_UNSIGNED_INT_SAMPLER_1D_ARRAY"; // 	usampler2DArray
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_ARRAY:
            return "GL_UNSIGNED_INT_SAMPLER_2D_ARRAY"; // 	usampler2DArray
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
            return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE"; // 	usampler2DMS
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
            return "GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY"; // 	usampler2DMSArray
        case GPUDataType::UNSIGNED_INT_SAMPLER_BUFFER:
            return "GL_UNSIGNED_INT_SAMPLER_BUFFER"; // 	usamplerBuffer
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_RECT:
            return "GL_UNSIGNED_INT_SAMPLER_2D_RECT"; // 	usampler2DRect
        case GPUDataType::IMAGE_1D:
            return "GL_IMAGE_1D"; // 	image1D
        case GPUDataType::IMAGE_2D:
            return "GL_IMAGE_2D"; // 	image2D
        case GPUDataType::IMAGE_3D:
            return "GL_IMAGE_3D"; // 	image3D
        case GPUDataType::IMAGE_2D_RECT:
            return "GL_IMAGE_2D_RECT"; // 	image2DRect
        case GPUDataType::IMAGE_CUBE:
            return "GL_IMAGE_CUBE"; // 	imageCube
        case GPUDataType::IMAGE_BUFFER:
            return "GL_IMAGE_BUFFER"; // 	imageBuffer
        case GPUDataType::IMAGE_1D_ARRAY:
            return "GL_IMAGE_1D_ARRAY"; // 	image1DArray
        case GPUDataType::IMAGE_2D_ARRAY:
            return "GL_IMAGE_2D_ARRAY"; // 	image2DArray
        case GPUDataType::IMAGE_2D_MULTISAMPLE:
            return "GL_IMAGE_2D_MULTISAMPLE"; // 	image2DMS
        case GPUDataType::IMAGE_2D_MULTISAMPLE_ARRAY:
            return "GL_IMAGE_2D_MULTISAMPLE_ARRAY"; // 	image2DMSArray
        case GPUDataType::INT_IMAGE_1D:
            return "GL_INT_IMAGE_1D"; // 	iimage1D
        case GPUDataType::INT_IMAGE_2D:
            return "GL_INT_IMAGE_2D"; // 	iimage2D
        case GPUDataType::INT_IMAGE_3D:
            return "GL_INT_IMAGE_3D"; // 	iimage3D
        case GPUDataType::INT_IMAGE_2D_RECT:
            return "GL_INT_IMAGE_2D_RECT"; // 	iimage2DRect
        case GPUDataType::INT_IMAGE_CUBE:
            return "GL_INT_IMAGE_CUBE"; // 	iimageCube
        case GPUDataType::INT_IMAGE_BUFFER:
            return "GL_INT_IMAGE_BUFFER"; // 	iimageBuffer
        case GPUDataType::INT_IMAGE_1D_ARRAY:
            return "GL_INT_IMAGE_1D_ARRAY"; // 	iimage1DArray
        case GPUDataType::INT_IMAGE_2D_ARRAY:
            return "GL_INT_IMAGE_2D_ARRAY"; // 	iimage2DArray
        case GPUDataType::INT_IMAGE_2D_MULTISAMPLE:
            return "GL_INT_IMAGE_2D_MULTISAMPLE"; // 	iimage2DMS
        case GPUDataType::INT_IMAGE_2D_MULTISAMPLE_ARRAY:
            return "GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY"; // 	iimage2DMSArray
        case GPUDataType::UNSIGNED_INT_IMAGE_1D:
            return "GL_UNSIGNED_INT_IMAGE_1D"; // 	uimage1D
        case GPUDataType::UNSIGNED_INT_IMAGE_2D:
            return "GL_UNSIGNED_INT_IMAGE_2D"; // 	uimage2D
        case GPUDataType::UNSIGNED_INT_IMAGE_3D:
            return "GL_UNSIGNED_INT_IMAGE_3D"; // 	uimage3D
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_RECT:
            return "GL_UNSIGNED_INT_IMAGE_2D_RECT"; // 	uimage2DRect
        case GPUDataType::UNSIGNED_INT_IMAGE_CUBE:
            return "GL_UNSIGNED_INT_IMAGE_CUBE"; // 	uimageCube
        case GPUDataType::UNSIGNED_INT_IMAGE_BUFFER:
            return "GL_UNSIGNED_INT_IMAGE_BUFFER"; // 	uimageBuffer
        case GPUDataType::UNSIGNED_INT_IMAGE_1D_ARRAY:
            return "GL_UNSIGNED_INT_IMAGE_1D_ARRAY"; // 	uimage1DArray
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_ARRAY:
            return "GL_UNSIGNED_INT_IMAGE_2D_ARRAY"; // 	uimage2DArray
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
            return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE"; // 	uimage2DMS
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
            return "GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY"; // 	uimage2DMSArray
        case GPUDataType::UNSIGNED_INT_ATOMIC_COUNTER:
            return "GL_UNSIGNED_INT_ATOMIC_COUNTER"; //
        default:
            LOG_CRITICAL(
                logging::opengl_logger, "GPUDataType with value {} is not implemented.",
                static_cast<GLenum>(data_type)
            );
            throw exc::not_implemented_error("Given GPUDataType is not implemented.");
    }
}

/**
 * @brief String representation of GPUDataType
 *
 * @param const GPUDataType& data type to represent
 */
constexpr inline size_t
get_size(const GPUDataType& data_type) {
    switch (data_type) {
        case GPUDataType::BYTE:
        case GPUDataType::UNSIGNED_BYTE:
        case GPUDataType::BOOL:
            return 1;
        case GPUDataType::SHORT:
        case GPUDataType::UNSIGNED_SHORT:
        case GPUDataType::BOOL_VEC2:
            return 2;
        case GPUDataType::BOOL_VEC3:
            return 3;
        case GPUDataType::INT:
        case GPUDataType::UNSIGNED_INT:
        case GPUDataType::FLOAT:
        case GPUDataType::BOOL_VEC4:
        // Everything below this with size 4 is basically a pointer
        case GPUDataType::SAMPLER_1D:
        case GPUDataType::SAMPLER_2D:
        case GPUDataType::SAMPLER_3D:
        case GPUDataType::SAMPLER_CUBE:
        case GPUDataType::SAMPLER_1D_SHADOW:
        case GPUDataType::SAMPLER_2D_SHADOW:
        case GPUDataType::SAMPLER_1D_ARRAY:
        case GPUDataType::SAMPLER_2D_ARRAY:
        case GPUDataType::SAMPLER_1D_ARRAY_SHADOW:
        case GPUDataType::SAMPLER_2D_ARRAY_SHADOW:
        case GPUDataType::SAMPLER_2D_MULTISAMPLE:
        case GPUDataType::SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GPUDataType::SAMPLER_CUBE_SHADOW:
        case GPUDataType::SAMPLER_BUFFER:
        case GPUDataType::SAMPLER_2D_RECT:
        case GPUDataType::SAMPLER_2D_RECT_SHADOW:
        case GPUDataType::INT_SAMPLER_1D:
        case GPUDataType::INT_SAMPLER_2D:
        case GPUDataType::INT_SAMPLER_3D:
        case GPUDataType::INT_SAMPLER_CUBE:
        case GPUDataType::INT_SAMPLER_1D_ARRAY:
        case GPUDataType::INT_SAMPLER_2D_ARRAY:
        case GPUDataType::INT_SAMPLER_2D_MULTISAMPLE:
        case GPUDataType::INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GPUDataType::INT_SAMPLER_BUFFER:
        case GPUDataType::INT_SAMPLER_2D_RECT:
        case GPUDataType::UNSIGNED_INT_SAMPLER_1D:
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D:
        case GPUDataType::UNSIGNED_INT_SAMPLER_3D:
        case GPUDataType::UNSIGNED_INT_SAMPLER_CUBE:
        case GPUDataType::UNSIGNED_INT_SAMPLER_1D_ARRAY:
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_ARRAY:
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE:
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY:
        case GPUDataType::UNSIGNED_INT_SAMPLER_BUFFER:
        case GPUDataType::UNSIGNED_INT_SAMPLER_2D_RECT:
        case GPUDataType::IMAGE_1D:
        case GPUDataType::IMAGE_2D:
        case GPUDataType::IMAGE_3D:
        case GPUDataType::IMAGE_2D_RECT:
        case GPUDataType::IMAGE_CUBE:
        case GPUDataType::IMAGE_BUFFER:
        case GPUDataType::IMAGE_1D_ARRAY:
        case GPUDataType::IMAGE_2D_ARRAY:
        case GPUDataType::IMAGE_2D_MULTISAMPLE:
        case GPUDataType::IMAGE_2D_MULTISAMPLE_ARRAY:
        case GPUDataType::INT_IMAGE_1D:
        case GPUDataType::INT_IMAGE_2D:
        case GPUDataType::INT_IMAGE_3D:
        case GPUDataType::INT_IMAGE_2D_RECT:
        case GPUDataType::INT_IMAGE_CUBE:
        case GPUDataType::INT_IMAGE_BUFFER:
        case GPUDataType::INT_IMAGE_1D_ARRAY:
        case GPUDataType::INT_IMAGE_2D_ARRAY:
        case GPUDataType::INT_IMAGE_2D_MULTISAMPLE:
        case GPUDataType::INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        case GPUDataType::UNSIGNED_INT_IMAGE_1D:
        case GPUDataType::UNSIGNED_INT_IMAGE_2D:
        case GPUDataType::UNSIGNED_INT_IMAGE_3D:
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_RECT:
        case GPUDataType::UNSIGNED_INT_IMAGE_CUBE:
        case GPUDataType::UNSIGNED_INT_IMAGE_BUFFER:
        case GPUDataType::UNSIGNED_INT_IMAGE_1D_ARRAY:
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_ARRAY:
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_MULTISAMPLE:
        case GPUDataType::UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY:
        case GPUDataType::UNSIGNED_INT_ATOMIC_COUNTER:
            return 4;
        case GPUDataType::DOUBLE:
        case GPUDataType::INT_VEC2:
        case GPUDataType::UNSIGNED_INT_VEC2:
        case GPUDataType::FLOAT_VEC2:
            return 8;
        case GPUDataType::INT_VEC3:
        case GPUDataType::UNSIGNED_INT_VEC3:
        case GPUDataType::FLOAT_VEC3:
            return 12;
        case GPUDataType::INT_VEC4:
        case GPUDataType::UNSIGNED_INT_VEC4:
        case GPUDataType::FLOAT_VEC4:
        case GPUDataType::DOUBLE_VEC2:
        case GPUDataType::FLOAT_MAT2:
            return 16;
        case GPUDataType::DOUBLE_VEC3:
        case GPUDataType::FLOAT_MAT2x3:
        case GPUDataType::FLOAT_MAT3x2:
            return 24;
        case GPUDataType::DOUBLE_VEC4:
        case GPUDataType::FLOAT_MAT2x4:
        case GPUDataType::FLOAT_MAT4x2:
        case GPUDataType::DOUBLE_MAT2:
            return 32;
        case GPUDataType::FLOAT_MAT3:
            return 36;
        case GPUDataType::FLOAT_MAT3x4:
        case GPUDataType::FLOAT_MAT4x3:
        case GPUDataType::DOUBLE_MAT2x3:
        case GPUDataType::DOUBLE_MAT3x2:
            return 48;
        case GPUDataType::FLOAT_MAT4:
        case GPUDataType::DOUBLE_MAT2x4:
        case GPUDataType::DOUBLE_MAT4x2:
            return 64;
        case GPUDataType::DOUBLE_MAT3:
            return 72;
        case GPUDataType::DOUBLE_MAT3x4:
        case GPUDataType::DOUBLE_MAT4x3:
            return 96;
        case GPUDataType::DOUBLE_MAT4:
            return 128;
        default:
            LOG_CRITICAL(
                logging::opengl_logger, "GPUDataType with value {} is not implemented.",
                static_cast<GLenum>(data_type)
            );
            throw exc::not_implemented_error("Given GPUDataType is not implemented.");
    }
}

} // namespace gpu_data

} // namespace gui
