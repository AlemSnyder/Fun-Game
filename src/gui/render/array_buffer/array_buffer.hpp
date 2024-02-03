#pragma once

#include "logging.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <concepts>
#include <type_traits>
#include <vector>

namespace gui {

namespace array_buffer {

// enum draw type
// static draw dynamic draw etc

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

enum class buffer_type : GLenum {
    ARRAY_BUFFER = GL_ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
};

class ArrayBuffer;

// cursed
class array_buffer_type {
 protected:
    const uint8_t vec_size_;  // 1,2,3 or 4 vec size
    const uint8_t type_size_; // size of type float, int etc.
    const bool is_int_;       // should be interpreted as integer
    const GPUDataType draw_type_;

 public:
    inline constexpr array_buffer_type(
        uint8_t vec_size, uint8_t type_size, bool is_int, GPUDataType draw_type
    ) :
        vec_size_(vec_size),
        type_size_(type_size), is_int_(is_int), draw_type_(draw_type) {
        assert(!(vec_size_ < 1 || vec_size > 4) && "Vector size not allowed");
    };

    // inline array_buffer_type() : array_buffer_type(0,0,0,GPUDataType::BYTE) {}

    inline constexpr GPUDataType
    get_draw_type() const {
        return draw_type_;
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

    template <typename T> // float/ double
    constexpr inline static GPUDataType
    presume_type() {
        switch (sizeof(T)) {
            case sizeof(glm::float32):

                return GPUDataType::FLOAT;

            case sizeof(glm::float64):
                return GPUDataType::DOUBLE;

            default:
                assert(false && "Invalid type.");
                break;
        }
    }

    template <std::integral T>
    inline constexpr static array_buffer_type
    init([[maybe_unused]] T i) {
        return array_buffer_type(1, sizeof(T), true, presume_type<T>());
    }

    template <std::floating_point T>
    inline constexpr static array_buffer_type
    init([[maybe_unused]] T i) {
        return array_buffer_type(1, sizeof(T), false, presume_type<T>());
    }

    template <int i, class T, glm::qualifier Q>
    constexpr static array_buffer_type
    init([[maybe_unused]] glm::vec<i, T, Q> V) {
        return array_buffer_type(
            i, sizeof(T), std::is_integral_v<T>, presume_type<T>()
        );
    }

    template <class T>
    constexpr static array_buffer_type
    init([[maybe_unused]] T t) {
        assert(false && "Invalid type.");
        return array_buffer_type(0, 0, 0, GPUDataType::BYTE);
    }

    bool operator==(const array_buffer_type& other) const = default;

    bool operator==(const ArrayBuffer& other) const;
};

class ArrayBuffer : public array_buffer_type {
 private:
    GLuint buffer_ID_;

    GLuint divisor_; // for instancing

    const buffer_type buffer_type_;

 public:
    template <class T>
    void update(std::vector<T> data, GLuint divisor);

    template <class T>
    inline void
    update(std::vector<T> data) {
        update<T>(data, divisor_);
    }

    inline void
    set_divisor(GLuint divisor) noexcept {
        divisor_ = divisor;
    }

    template <class T>
    inline ArrayBuffer(std::vector<T> data) :
        ArrayBuffer(data, 0, buffer_type::ARRAY_BUFFER) {}

    template <class T>
    ArrayBuffer(std::vector<T> data, GLuint divisor, buffer_type bt);

    template <class T>
    inline ArrayBuffer() :
        array_buffer_type(array_buffer_type::presume_type<T>()), buffer_ID_(0),
        divisor_(0), buffer_type_(buffer_type::ARRAY_BUFFER) {}

    ~ArrayBuffer() { glDeleteBuffers(1, &buffer_ID_); };

    void bind(GLuint attribute, GLuint index) const;

    inline void
    bind(GLuint attribute) const {
        bind(attribute, attribute);
    };
};

template <class T>
ArrayBuffer::ArrayBuffer(std::vector<T> buffer_data, GLuint divisor, buffer_type bt) :
    array_buffer_type(array_buffer_type::init(T())), divisor_(divisor),
    buffer_type_(bt) {
    glGenBuffers(1, &buffer_ID_);
    glBindBuffer(static_cast<GLenum>(buffer_type_), buffer_ID_);
    glBufferData(
        static_cast<GLenum>(buffer_type_), buffer_data.size() * type_size_ * vec_size_,
        buffer_data.data(),
        GL_DYNAMIC_DRAW // TODO
    );
}

template <class T>
void
ArrayBuffer::update(std::vector<T> buffer_data, GLuint divisor) {
    divisor_ = divisor;

    bool classes_are_equivelent = operator==(array_buffer_type::init(T()));

    assert(classes_are_equivelent && "Class must be the same");

    glDeleteBuffers(1, &buffer_ID_);

    glGenBuffers(1, &buffer_ID_);
    glBindBuffer(static_cast<GLenum>(buffer_type_), buffer_ID_);
    glBufferData(
        static_cast<GLenum>(buffer_type_), buffer_data.size() * type_size_ * vec_size_,
        buffer_data.data(),
        GL_DYNAMIC_DRAW // TODO
    );
}

} // namespace array_buffer

} // namespace gui
