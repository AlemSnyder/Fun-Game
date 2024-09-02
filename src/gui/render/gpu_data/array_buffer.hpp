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
struct GPUArrayType {
    const uint8_t major_size; // 1,2,3 or 4 vec size
    const uint8_t minor_size;
    const uint8_t type_size;     // size of type float, int etc.
    const bool is_int;           // should be interpreted as integer
    const GPUDataType draw_type; // type of smallest unit of memory

 private:
    inline constexpr GPUArrayType(
        uint8_t major_size_, uint8_t minor_size_, uint8_t type_size, bool is_int,
        GPUDataType draw_type
    ) :
        major_size(major_size_), minor_size(minor_size_), type_size(type_size),
        is_int(is_int), draw_type(draw_type) {}

 public:
    template <
        uint8_t major_size_T, uint8_t minor_size_T, uint8_t type_size_T, bool is_int_T,
        GPUDataType draw_type_T>
    constexpr inline static GPUArrayType
    create_from_data() {
        static_assert(
            (1 <= major_size_T && major_size_T <= 4), "Vector size not allowed"
        );
        static_assert(
            (1 <= minor_size_T && minor_size_T <= 4), "Vector size not allowed"
        );

        return GPUArrayType(
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
    inline constexpr static GPUArrayType
    create_from_object([[maybe_unused]] T i = 0) {
        return create_from_data<1, 1, sizeof(T), true, presume_type<T>()>();
    }

    template <std::floating_point T>
    inline constexpr static GPUArrayType
    create_from_object([[maybe_unused]] T i = 0) {
        return create_from_data<1, 1, sizeof(T), false, presume_type<T>()>();
    }

    template <int i, class T, glm::qualifier Q>
    constexpr static GPUArrayType
    create_from_object([[maybe_unused]] glm::vec<i, T, Q> V = 0) {
        return create_from_data<
            i, 1, sizeof(T), std::is_integral_v<T>, presume_type<T>()>();
    }

    // glm matricies
    template <int i, int j, class T, glm::qualifier Q>
    constexpr static GPUArrayType
    create_from_object([[maybe_unused]] glm::mat<i, j, T, Q> V = 0) {
        return create_from_data<
            i, j, sizeof(T), std::is_integral_v<T>, presume_type<T>()>();
    }

    template <class T>
    constexpr static GPUArrayType
    create_from_object([[maybe_unused]] T t) {
        assert(false && "Invalid type.");
        return create_from_data<0, 0, 0, true, GPUDataType::BYTE>();
    }

    template <class T>
    constexpr static GPUArrayType
    create() {
        return create_from_object(T());
    }
};

} // namespace

/**
 * @brief Generates a vector like object to store data on GPU.
 */
template <class T, BindingTarget buffer = BindingTarget::ARRAY_BUFFER>
class ArrayBuffer {
 private:
    GLuint buffer_ID_; // For binding
    GLuint divisor_;   // For instancing usually 0, 1

    size_t size_ = 0;
    size_t aloc_size_ = 0;

 public:
    /**
     * @brief Default constructor
     */
    inline ArrayBuffer() : divisor_(0), size_(0), aloc_size_(0) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { glGenBuffers(1, &buffer_ID_); });
    }

    /**
     * @brief Construct ArrayBuffer with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    inline explicit ArrayBuffer(const std::vector<T>& data) : ArrayBuffer(data, 0) {}

    /**
     * @brief Construct ArrayBuffer with data and divisor
     *
     * @param std::vector<T>& data data to send to GPU
     * @param GLuint divisor go look up instancing
     */
    inline explicit ArrayBuffer(const std::vector<T>& data, GLuint divisor) :
        divisor_(divisor) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, data]() {
            LOG_BACKTRACE(
                logging::opengl_logger, "buffer ID before generation: {}", buffer_ID_
            );
            glGenBuffers(1, &buffer_ID_);
            this->pointer_update_(data.data(), 0, data.size());
        });
    };

    /**
     * @brief Construct ArrayBuffer with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    inline ArrayBuffer(std::initializer_list<T> data) : ArrayBuffer(data, 0) {}

    /**
     * @brief Update ArrayBuffer with data and divisor
     *
     * @param std::vector<T>& data data to send to GPU
     * @param GLuint divisor go look up instancing
     */
    inline void
    update(std::vector<T> data, GLuint offset) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, data, offset]() {
            this->pointer_update_(data.data(), offset, data.size());
        });
    };

    /**
     * @brief Update ArrayBuffer with data
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
     * @return GLuint& divisor_
     */
    [[nodiscard]] inline GLuint&
    divisor() noexcept {
        return divisor_;
    }

    ~ArrayBuffer() { glDeleteBuffers(1, &buffer_ID_); }

    /**
     * @brief Bind to the given index
     *
     * @param GLuint index the location = # in programs
     */
    void bind(GLuint index) const;

    inline void bind() const;

    [[nodiscard]] inline constexpr static GPUArrayType
    get_array_type() {
        constexpr GPUArrayType data_type = GPUArrayType::create<T>();
        return data_type;
    }

    [[nodiscard]] inline constexpr static GPUDataType
    get_opengl_numeric_type() {
        constexpr GPUDataType draw_type = get_array_type().draw_type;
        return draw_type;
    }

    [[nodiscard]] inline constexpr static uint8_t
    get_major_size() {
        constexpr uint8_t major_size = get_array_type().major_size;
        return major_size;
    }

    [[nodiscard]] inline constexpr static uint8_t
    get_type_size() {
        constexpr uint8_t type_size = get_array_type().type_size;
        return type_size;
    }

 private:
    /**
     * @brief Update ArrayBuffer with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    void pointer_update_(const T* data_begin, size_t offset, size_t add_data_size);
};

// go do vector implementations

template <class T, BindingTarget buffer>
void
ArrayBuffer<T, buffer>::pointer_update_(
    const T* data_begin, size_t offset, size_t add_data_size
) {
    constexpr GPUArrayType data_type = GPUArrayType::create<T>();

    LOG_BACKTRACE(
        logging::opengl_logger, "Updating buffer ID: {}, vec size {}, data type: {}",
        buffer_ID_, data_type.major_size, to_string(data_type.draw_type)
    );

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

    if (aloc_size_ < offset + add_data_size) {
        // reallocate
        aloc_size_ = offset + add_data_size;

        // need to in addition allocate the offset

        glBindBuffer(static_cast<GLenum>(buffer), buffer_ID_);
        // this should theoretically copy the existing data into a new buffer.
        glBufferData(
            static_cast<GLenum>(buffer), offset_size_in_bytes + data_size_in_bytes,
            nullptr, GL_DYNAMIC_DRAW
        );

        // TODO add case to reduce size
        // The problem is that the way this is setup doesn't allow that.
        // there is not way to say where the new data end is. one should not
    }
    // write data
    glBufferSubData(
        static_cast<GLenum>(buffer), offset_size_in_bytes, data_size_in_bytes,
        data_begin
    );
}

template <class T, BindingTarget buffer>
void
ArrayBuffer<T, buffer>::bind() const {
    static_assert(
        buffer == BindingTarget::ELEMENT_ARRAY_BUFFER,
        "Must supply index for non element array buffers."
    );
    constexpr GPUArrayType data_type = GPUArrayType::create<T>();

    LOG_BACKTRACE(
        logging::opengl_logger, "Binding buffer ID: {}, vec size {}, data type: {}",
        buffer_ID_, data_type.major_size, to_string(data_type.draw_type)
    );

    glBindBuffer(static_cast<GLenum>(buffer), buffer_ID_);
}

template <class T, BindingTarget buffer>
__attribute__((optimize(3))) void
ArrayBuffer<T, buffer>::bind(GLuint index) const {
    static_assert(
        buffer != BindingTarget::ELEMENT_ARRAY_BUFFER,
        "Element array buffers do not have an index."
    );

    constexpr GPUArrayType data_type = GPUArrayType::create<T>();

    LOG_BACKTRACE(
        logging::opengl_logger, "Binding buffer ID: {}, vec size {}, data type: {}",
        buffer_ID_, data_type.major_size, to_string(data_type.draw_type)
    );

    glBindBuffer(static_cast<GLenum>(buffer), buffer_ID_);

    if constexpr (buffer == BindingTarget::ARRAY_BUFFER) {
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
