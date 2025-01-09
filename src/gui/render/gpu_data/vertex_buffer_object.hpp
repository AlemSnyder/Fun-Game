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
 * @file Vertex_buffer_object.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines VertexBufferObject class
 *
 * @ingroup GUI  GPU_DATA
 *
 */

#pragma once

#include "../gl_enums.hpp"
#include "global_context.hpp"
#include "logging.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <concepts>
#include <type_traits>
#include <vector>

namespace gui {

namespace gpu_data {

namespace {

/**
 * @brief Determines how a data type should be interpreted on the GPU.
 *
 * @details The entire thing works by template deduction and magic.
 */
struct GPUStructureType {
    const uint8_t major_size; // 1,2,3 or 4 vec size
    const uint8_t minor_size;
    const uint8_t type_size;     // size of type float, int etc.
    const bool is_int;           // should be interpreted as integer
    const GPUDataType draw_type; // type of smallest unit of memory

 private:
    /**
     * @brief Construct a new GPUStructureType.
     *
     * @details This one directly copies the given data in this class. Use a templated
     * constructor with no arguments for the correct values.
     *
     * @param uint8_t major_size_ the number of dimensions in a given vector
     * @param uint8_t minor_size_ if matrix the number of rows
     * @param uint8_t type_size the size in bytes of the low level value being stored
     * @param bool is_int weather or not the data should be interpreted as an integer.
     * @param GPUDataType draw_type buffer type.
     */
    inline constexpr GPUStructureType(
        uint8_t major_size_, uint8_t minor_size_, uint8_t type_size, bool is_int,
        GPUDataType draw_type
    ) :
        major_size(major_size_),
        minor_size(minor_size_), type_size(type_size), is_int(is_int),
        draw_type(draw_type) {}

    template <
        uint8_t major_size_T, uint8_t minor_size_T, uint8_t type_size_T, bool is_int_T,
        GPUDataType draw_type_T>
    constexpr inline static GPUStructureType
    create_from_data() {
        static_assert(
            (1 <= major_size_T && major_size_T <= 4), "Vector size not allowed"
        );
        static_assert(
            (1 <= minor_size_T && minor_size_T <= 4), "Vector size not allowed"
        );

        return GPUStructureType(
            major_size_T, minor_size_T, type_size_T, is_int_T, draw_type_T
        );
    }

    template <std::integral T>
    constexpr inline static GPUDataType
    presume_type() {
        static_assert(
            sizeof(T) == sizeof(int8_t) || sizeof(T) == sizeof(int16_t)
                || sizeof(T) == sizeof(int32_t),
            "Invalid Type"
        );
        if constexpr (sizeof(T) == sizeof(int8_t)) {
            if (std::is_unsigned_v<T>)
                return GPUDataType::UNSIGNED_BYTE;
            else
                return GPUDataType::BYTE;
        } else if constexpr (sizeof(T) == sizeof(int16_t)) {
            if (std::is_unsigned_v<T>)
                return GPUDataType::UNSIGNED_SHORT;
            else
                return GPUDataType::SHORT;
        } else if constexpr (sizeof(T) == sizeof(int32_t)) {
            if (std::is_unsigned_v<T>)
                return GPUDataType::UNSIGNED_INT;
            else
                return GPUDataType::INT;
        }
    }

    // std::same_as<float> is because there is a compiler bug in gcc
    // should be fixed in a later version of gcc tm
    template <std::same_as<float> T> // float
    constexpr inline static GPUDataType
    presume_type() {
        return GPUDataType::FLOAT;
    }

    template <std::same_as<double> T> // double
    constexpr inline static GPUDataType
    presume_type() {
        return GPUDataType::DOUBLE;
    }

    template <std::integral T>
    inline constexpr static GPUStructureType
    create_from_object([[maybe_unused]] T i = 0) {
        return create_from_data<1, 1, sizeof(T), true, presume_type<T>()>();
    }

    template <std::floating_point T>
    inline constexpr static GPUStructureType
    create_from_object([[maybe_unused]] T i = 0) {
        return create_from_data<1, 1, sizeof(T), false, presume_type<T>()>();
    }

    template <int i, class T, glm::qualifier Q>
    constexpr static GPUStructureType
    create_from_object([[maybe_unused]] glm::vec<i, T, Q> V = 0) {
        return create_from_data<
            i, 1, sizeof(T), std::is_integral_v<T>, presume_type<T>()>();
    }

    // glm matricies
    template <int i, int j, class T, glm::qualifier Q>
    constexpr static GPUStructureType
    create_from_object([[maybe_unused]] glm::mat<i, j, T, Q> V = 0) {
        return create_from_data<
            i, j, sizeof(T), std::is_integral_v<T>, presume_type<T>()>();
    }

    template <class T>
    constexpr static GPUStructureType
    create_from_object([[maybe_unused]] T t) {
        assert(false && "Invalid type.");
        return create_from_data<1, 1, 1, true, GPUDataType::BYTE>();
    }

 public:
    template <class T>
    constexpr static GPUStructureType
    create() {
        return create_from_object(T());
    }
};

} // namespace

/**
 * @brief Generates a Vertex Buffer Object to store data on GPU.
 */
template <class T, BindingTarget Buffer = BindingTarget::ARRAY_BUFFER>
class VertexBufferObject {
 private:
    GLuint buffer_ID_ = 0; // For binding
    GLuint divisor_;       // For instancing usually 0, 1

    size_t size_ = 0;
    size_t alloc_size_ = 0;

 public:
    /**
     * @brief Default constructor
     */
    inline VertexBufferObject() : divisor_(0), size_(0), alloc_size_(0) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { glGenBuffers(1, &buffer_ID_); });
    }

    VertexBufferObject(VertexBufferObject&&) = default;

    VertexBufferObject(const VertexBufferObject&) = delete;

    /**
     * @brief Construct VertexBufferObject with data
     *
     * @param const std::vector<T>& data data to send to GPU
     */
    inline explicit VertexBufferObject(const std::vector<T>& data) :
        VertexBufferObject(data, 0) {}

    /**
     * @brief Construct VertexBufferObject with data and divisor
     *
     * @param std::vector<T>& data data to send to GPU
     * @param GLuint divisor go look up instancing
     */
    inline explicit VertexBufferObject(const std::vector<T>& data, GLuint divisor) :
        divisor_(divisor) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, data]() {
            LOG_BACKTRACE(
                logging::opengl_logger, "Buffer ID before generation: {}", buffer_ID_
            );
            glGenBuffers(1, &buffer_ID_);
            this->pointer_update_(data.data(), 0, data.size());
        });
    };

    /**
     * @brief Construct VertexBufferObject with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    inline VertexBufferObject(std::initializer_list<T> data) :
        VertexBufferObject(data, 0) {}

    /**
     * @brief Update VertexBufferObject with data and divisor
     *
     * @param std::vector<T>& data data to send to GPU
     * @param GLuint divisor go look up instancing
     */
    inline void
    update(std::vector<T> data, GLuint offset) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, data = std::move(data), offset]() {
            this->pointer_update_(data.data(), offset, data.size());
        });
    };

    /**
     * @brief Update VertexBufferObject with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    inline void
    update(const std::vector<T>& data) {
        update(data.data(), 0);
    }

    /**
     * @brief Get the divisor
     *
     * @return GLuint divisor_
     */
    [[nodiscard]] inline GLuint
    divisor() const noexcept {
        return divisor_;
    }

    /**
     * @brief Get the array's size
     *
     * @return size_t size_
     */
    [[nodiscard]] inline size_t
    size() const noexcept {
        return size_;
    }

    ~VertexBufferObject() { glDeleteBuffers(1, &buffer_ID_); }

    /**
     * @brief Attach to the given index.
     *
     * @param GLuint index the location = # in programs
     */
    void attach_to_vertex_attribute(GLuint index) const;

    /**
     * @brief Bind this vertex buffer object.
     */
    inline void bind() const;

    /**
     * @brief Get array type.
     *
     * @return GPUArrayType array type
     */
    [[nodiscard]] inline constexpr static GPUStructureType
    get_array_type() {
        constexpr GPUStructureType data_type = GPUStructureType::create<T>();
        return data_type;
    }

    /**
     * @brief Get data type
     *
     * @return GPUDataType underlying data type
     */
    [[nodiscard]] inline constexpr static GPUDataType
    get_opengl_numeric_type() {
        constexpr GPUDataType draw_type = get_array_type().draw_type;
        return draw_type;
    }

    /**
     * @brief Get the matrix row dimension
     *
     * @return uint8_t row dimension
     */
    [[nodiscard]] inline constexpr static uint8_t
    get_major_size() {
        constexpr uint8_t major_size = get_array_type().major_size;
        return major_size;
    }

    /**
     * @brief Get the matrix column dimension
     *
     * @return uint8_t column dimension
     */
    [[nodiscard]] inline constexpr static uint8_t
    get_minor_size() {
        constexpr uint8_t minor_size = get_array_type().minor_size;
        return minor_size;
    }

    /**
     * @brief Get the size of the underling type
     *
     * @return uint8_t type size
     */
    [[nodiscard]] inline constexpr static uint8_t
    get_type_size() {
        constexpr uint8_t type_size = get_array_type().type_size;
        return type_size;
    }

 private:
    /**
     * @brief Update VertexBufferObject with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    void pointer_update_(const T* data_begin, size_t offset, size_t add_data_size);
};

// go do vector implementations

template <class T, BindingTarget Buffer>
void
VertexBufferObject<T, Buffer>::pointer_update_(
    const T* data_begin, size_t offset, size_t add_data_size
) {
    constexpr GPUStructureType data_type = GPUStructureType::create<T>();

    static_assert(
        sizeof(T)
            == static_cast<size_t>(data_type.type_size)
                   * static_cast<size_t>(data_type.major_size)
                   * static_cast<size_t>(data_type.minor_size),
        "You messed up how you do math"
    );

    size_t data_size_in_bytes = add_data_size * static_cast<size_t>(data_type.type_size)
                                * static_cast<size_t>(data_type.major_size)
                                * static_cast<size_t>(data_type.minor_size);

    size_t offset_size_in_bytes = offset * static_cast<size_t>(data_type.type_size)
                                  * static_cast<size_t>(data_type.major_size)
                                  * static_cast<size_t>(data_type.minor_size);

    LOG_BACKTRACE(
        logging::opengl_logger, "Writing {} * {} * {} * {} = {} bytes.", add_data_size,
        data_type.type_size, data_type.major_size, data_type.minor_size,
        data_size_in_bytes
    );

    if (alloc_size_ < offset + add_data_size) {
        // reallocate
        alloc_size_ = offset + add_data_size;

        // need to in addition allocate the offset

        bind();
        // this should theoretically copy the existing data into a new buffer.
        glBufferData(
            static_cast<GLenum>(Buffer), offset_size_in_bytes + data_size_in_bytes,
            nullptr, GL_DYNAMIC_DRAW
        );

        // TODO add case to reduce size
        // The problem is that the way this is setup doesn't allow that.
        // there is not way to say where the new data end is. one should not
    } else {
        LOG_BACKTRACE(
            logging::opengl_logger,
            "Updating Buffer ID: {}, vec size {}, data type: {}", buffer_ID_,
            data_type.major_size, to_string(data_type.draw_type)
        );
    }
    // write data
    glBufferSubData(
        static_cast<GLenum>(Buffer), offset_size_in_bytes, data_size_in_bytes,
        data_begin
    );
    size_ = alloc_size_;
}

template <class T, BindingTarget Buffer>
void
VertexBufferObject<T, Buffer>::bind() const {
    constexpr GPUStructureType data_type = GPUStructureType::create<T>();

    LOG_BACKTRACE(
        logging::opengl_logger, "Binding Buffer ID: {}, vec size {}, data type: {}",
        buffer_ID_, data_type.major_size, to_string(data_type.draw_type)
    );

    if (buffer_ID_ == 0) {
        LOG_ERROR(
            logging::opengl_logger,
            "Buffer ID 0 is not allowed. Has this buffer been initialized?"
        );
    }

    glBindBuffer(static_cast<GLenum>(Buffer), buffer_ID_);
}

template <class T, BindingTarget Buffer>
__attribute__((optimize(3))) void
VertexBufferObject<T, Buffer>::attach_to_vertex_attribute(GLuint index) const {
    static_assert(
        Buffer != BindingTarget::ELEMENT_ARRAY_BUFFER,
        "Element array buffers do not have an index."
    );

    constexpr GPUStructureType data_type = GPUStructureType::create<T>();

    bind();

    if constexpr (Buffer == BindingTarget::ARRAY_BUFFER) {
#pragma GCC unroll 4
        for (size_t i = 0; i < data_type.minor_size; i++) {
            glEnableVertexAttribArray(index + i);
            constexpr GLenum type = static_cast<GLenum>(data_type.draw_type);
            constexpr GLuint stride =
                data_type.major_size * data_type.minor_size * data_type.type_size;
            void* offset = (void*)(data_type.major_size * data_type.type_size * i);
            if constexpr (data_type.is_int) {
                glVertexAttribIPointer(
                    index + i,            // index
                    data_type.major_size, // size
                    type,                 // type
                    stride,               // stride
                    offset                // array buffer offset
                );
            } else {
                if constexpr (data_type.draw_type == GPUDataType::FLOAT) {
                    glVertexAttribPointer(
                        index + i,            // index
                        data_type.major_size, // size
                        type,                 // type
                        false,                // normalize
                        stride,               // stride
                        offset                // array buffer offset
                    );
                } else if constexpr (data_type.draw_type == GPUDataType::DOUBLE) {
                    glVertexAttribLPointer(
                        index + i,            // index
                        data_type.major_size, // size
                        type,                 // type
                        stride,               // stride
                        offset                // array buffer offset
                    );
                }
            }

            glVertexAttribDivisor(index + i, divisor_);
        }
    }
}

} // namespace gpu_data

} // namespace gui
