#pragma once

#include "global_context.hpp"
#include "logging.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <concepts>
#include <type_traits>
#include <vector>

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

    FLOAT           = GL_FLOAT,
    DOUBLE          = GL_DOUBLE,
    // clang-format on
};

constexpr std::string
reper(const GPUDataType& data_type) {
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

/**
 * @brief Targe. Designates how the buffer is used.
 */
enum class BindingTarget : GLenum {
    ARRAY_BUFFER = GL_ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
};

/**
 * @brief Determines how a data type should be interpreted on the GPU.
 *
 * @details The entire thing works by template deduction and magic.
 */
struct GPUArrayType {
    const uint8_t vec_size;      // 1,2,3 or 4 vec size
    const uint8_t type_size;     // size of type float, int etc.
    const bool is_int;           // should be interpreted as integer
    const GPUDataType draw_type; // type of smallest unit of memory

    inline constexpr GPUArrayType(
        uint8_t vec_size, uint8_t type_size, bool is_int, GPUDataType draw_type
    ) :
        vec_size(vec_size),
        type_size(type_size), is_int(is_int), draw_type(draw_type) {
        assert((1 <= vec_size && vec_size <= 4) && "Vector size not allowed");
    }

    template <std::integral T>
    inline constexpr GPUArrayType() :
        GPUArrayType(1, sizeof(T), true, presume_type<T>()) {}

    template <std::floating_point T>
    inline constexpr GPUArrayType() :
        GPUArrayType(1, sizeof(T), false, presume_type<T>()) {}

    template <int i, class T, glm::qualifier Q>
    constexpr GPUArrayType() :
        GPUArrayType(i, sizeof(T), std::is_integral_v<T>, presume_type<T>()) {}

    template <class T>
    constexpr GPUArrayType() : GPUArrayType(0, 0, 0, GPUDataType::BYTE) {
        assert(false && "Invalid type.");
    }

    template <std::integral T>
    constexpr inline static GPUDataType
    presume_type() {
        switch (sizeof(T)) {
            case sizeof(int8_t):
                if (std::is_unsigned_v<T>)
                    return GPUDataType::UNSIGNED_BYTE;
                else
                    return GPUDataType::BYTE;

            case sizeof(int16_t):
                if (std::is_unsigned_v<T>)
                    return GPUDataType::UNSIGNED_SHORT;
                else
                    return GPUDataType::SHORT;

            case sizeof(int32_t):
                if (std::is_unsigned_v<T>)
                    return GPUDataType::UNSIGNED_INT;
                else
                    return GPUDataType::INT;

            default:
                assert(false && "Invalid type.");
                break;
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
        return GPUArrayType(1, sizeof(T), true, presume_type<T>());
    }

    template <std::floating_point T>
    inline constexpr static GPUArrayType
    create_from_object([[maybe_unused]] T i = 0) {
        return GPUArrayType(1, sizeof(T), false, presume_type<T>());
    }

    template <int i, class T, glm::qualifier Q>
    constexpr static GPUArrayType
    create_from_object([[maybe_unused]] glm::vec<i, T, Q> V = 0) {
        return GPUArrayType(i, sizeof(T), std::is_integral_v<T>, presume_type<T>());
    }

    // glm matricies
    template <int i, int j, class T, glm::qualifier Q>
    constexpr static GPUArrayType
    create_from_object([[maybe_unused]] glm::mat<i, j, T, Q> V = 0) {
        return GPUArrayType(i * j, sizeof(T), std::is_integral_v<T>, presume_type<T>());
    }

    template <class T>
    constexpr static GPUArrayType
    create_from_object([[maybe_unused]] T t) {
        assert(false && "Invalid type.");
        return GPUArrayType(0, 0, 0, GPUDataType::BYTE);
    }

    template <class T>
    constexpr static GPUArrayType
    create() {
        return create_from_object(T());
    }

    bool operator==(const GPUArrayType& other) const = default;
};

/**
 * @brief Generates a vector like object to store data on GPU.
 */
template <class T, BindingTarget buffer = BindingTarget::ARRAY_BUFFER>
class ArrayBuffer {
 private:
    GLuint buffer_ID_; // For binding
    GLuint divisor_;   // For instancing usually 0, 1

    size_t size = 0;
    size_t aloc_size = 0;

 public:
    /**
     * @brief Default constructor
     */
    inline ArrayBuffer() : divisor_(0), size(0), aloc_size(0) {
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
            this->update_(data.data(), 0, data.size());
        });
    };

    // copy constructor
    //    ArrayBuffer(const ArrayBuffer& other) = delete;
    //    ArrayBuffer(ArrayBuffer&& other) = delete;
    // copy operator
    //    ArrayBuffer& operator=(const ArrayBuffer& other) = delete;
    //    ArrayBuffer& operator=(ArrayBuffer&& other) = delete;

    /**
     * @brief Construct ArrayBuffer with data and divisor
     *
     * @param std::vector<T>& data data to send to GPU
     * @param GLuint divisor go look up instancing
     */
    inline ArrayBuffer(std::initializer_list<T> data, GLuint divisor) :
        divisor_(divisor) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, data]() {
            glGenBuffers(1, &buffer_ID_);
            this->update_(data.begin(), 0, data.size());
        });
    }

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
            this->update_(data.data(), offset, data.size());
        });
    };

    /**
     * @brief Update ArrayBuffer with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    inline void
    update(std::vector<T> data) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, data]() {
            this->update_(data.data(), 0, data.size());
        });
    }

    //    inline void update(size_t offset, std::vector<T> data);

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
     * @brief Bind to the given attribute
     *
     * @param GLuint attribute the location = # in programs
     * @param GLuint index I have no idea what this does.
     */
    void bind(GLuint attribute, GLuint index) const;

    /**
     * @brief Bind to the given attribute
     *
     * @param GLuint attribute the location = # in programs
     */
    inline void
    bind(GLuint attribute) const {
        bind(attribute, attribute);
    };

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

    [[nodiscard]] inline constexpr static int
    get_vec_size() {
        constexpr int vec_size = get_array_type().vec_size;
        return vec_size;
    }

    [[nodiscard]] inline constexpr static int
    get_type_size() {
        constexpr int type_size = get_array_type().type_size;
        return type_size;
    }

 private:
    /**
     * @brief Update ArrayBuffer with data
     *
     * @param std::vector<T>& data data to send to GPU
     */
    void update_(const T* data_begin, size_t offset, size_t add_data_size);
};

// go do vector implementations

template <class T, BindingTarget buffer>
void
ArrayBuffer<T, buffer>::update_(
    const T* data_begin, size_t offset, size_t add_data_size
) {
    constexpr GPUArrayType data_type = GPUArrayType::create<T>();

    LOG_BACKTRACE(
        logging::opengl_logger, "Updating buffer ID: {}, vec size {}, data type: {}",
        buffer_ID_, data_type.vec_size, reper(data_type.draw_type)
    );

    if (aloc_size < offset + add_data_size) {
        // reallocate
        aloc_size = offset + add_data_size;

        glBindBuffer(static_cast<GLenum>(buffer), buffer_ID_);
        glBufferData(
            static_cast<GLenum>(buffer),
            aloc_size * data_type.type_size * data_type.vec_size, nullptr,
            GL_DYNAMIC_DRAW
        );

        glBufferSubData(static_cast<GLenum>(buffer), offset, add_data_size, data_begin);

        // TODO add case to reduce size
    } else {
        glBindBuffer(static_cast<GLenum>(buffer), buffer_ID_);
        glBufferSubData(static_cast<GLenum>(buffer), offset, add_data_size, data_begin);
    }
}

template <class T, BindingTarget buffer>
void
ArrayBuffer<T, buffer>::bind(GLuint attribute, GLuint index) const {
    constexpr GPUArrayType data_type = GPUArrayType::create<T>();

    LOG_BACKTRACE(
        logging::opengl_logger, "Updating buffer ID: {}, vec size {}, data type: {}",
        buffer_ID_, data_type.vec_size, reper(data_type.draw_type)
    );

    if constexpr (buffer != BindingTarget::ELEMENT_ARRAY_BUFFER)
        glEnableVertexAttribArray(index);

    glBindBuffer(static_cast<GLenum>(buffer), buffer_ID_);

    if constexpr (buffer == BindingTarget::ARRAY_BUFFER) {
        if constexpr (data_type.is_int) {
            glVertexAttribIPointer(
                attribute,                                // attribute
                data_type.vec_size,                       // size
                static_cast<GLenum>(data_type.draw_type), // type
                data_type.vec_size * data_type.type_size, // stride
                (void*)0                                  // array buffer offset
            );
        } else {
            glVertexAttribPointer(
                attribute,                                // attribute
                data_type.vec_size,                       // size
                GL_FLOAT,                                 // type
                false,                                    // normalize
                data_type.vec_size * data_type.type_size, // stride
                (void*)0                                  // array buffer offset
            );
        }

        glVertexAttribDivisor(index, divisor_);
    }
}

} // namespace gpu_data

} // namespace gui