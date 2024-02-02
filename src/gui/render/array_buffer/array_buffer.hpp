#pragma once

#include "logging.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <concepts>
#include <type_traits>
#include <vector>

namespace gui {
namespace data_structures {

// enum draw type
// static draw dynamic draw etc

enum class GL_draw_type : GLenum {

    // clang-format off
    BYTE            = GL_BYTE,
    UNSIGNED_BYTE   = GL_UNSIGNED_BYTE,
    SHORT           = GL_SHORT,
    UNSIGNED_SHORT  = GL_UNSIGNED_SHORT,
    INT             = GL_INT,
    UNSIGNED_INT    = GL_UNSIGNED_INT,

    FLOAT           = GL_FLOAT,
    DOUBLE          = GL_DOUBLE,
    // clang-format one
};

enum class buffer_type : GLenum {
    ARRAY_BUFFER  = GL_ARRAY_BUFFER,
    ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,
};

class ArrayBuffer;

// cursed
class array_buffer_type {
 protected:
    const uint8_t vec_size_ : 3; // 1,2,3 or 4 vec size
    const uint8_t type_size_;    // size of type float, int etc.
    const bool is_int_ : 1;      // should be interpreted as integer
    const GL_draw_type draw_type_;


 public:
    inline constexpr array_buffer_type(
        uint8_t vec_size, uint8_t type_size, bool is_int, GL_draw_type draw_type
    ) :
        vec_size_(vec_size),
        type_size_(type_size),
        is_int_(is_int),
        draw_type_(draw_type) {
            assert ( !( vec_size_ < 1 || vec_size > 4 ) && "Vector size not allowed");
        };

    //inline array_buffer_type() : array_buffer_type(0,0,0,GL_draw_type::BYTE) {}

    inline constexpr GL_draw_type get_draw_type() const {
        return draw_type_;
    }

    template <std::integral T>
    constexpr inline static GL_draw_type presume_type() {
        switch (sizeof(T)) {
        case sizeof(int8_t):
            if (std::is_unsigned_v<T>)
                return GL_draw_type::UNSIGNED_BYTE;
            else
                return GL_draw_type::BYTE;

        case sizeof(int16_t):
            if (std::is_unsigned_v<T>)
                return GL_draw_type::UNSIGNED_SHORT;
            else
                return GL_draw_type::SHORT;

        case sizeof(int32_t):
            if (std::is_unsigned_v<T>)
                return GL_draw_type::UNSIGNED_INT;
            else
                return GL_draw_type::INT;

        default:
            assert(false && "Invalid type.");
            break;
        }
    }

    template <typename T> // float/ double
    constexpr inline static GL_draw_type presume_type() {
        switch (sizeof(T)) {
        case sizeof(glm::float32):
        
                return GL_draw_type::FLOAT;

        case sizeof(glm::float64):
                return GL_draw_type::DOUBLE;

        default:
            assert(false && "Invalid type.");
            break;
        }
    }

    template <std::integral T>
    inline constexpr static array_buffer_type init([[maybe_unused]] T i) {
        return array_buffer_type(1, sizeof(T), true, presume_type<T>());
    }

    template <std::floating_point T>
    inline constexpr static array_buffer_type init([[maybe_unused]] T i) {
        return array_buffer_type(1, sizeof(T), false, presume_type<T>());
    }

    template <int i, class T, glm::qualifier Q>
    constexpr static array_buffer_type init([[maybe_unused]] glm::vec<i, T, Q> V){
        return array_buffer_type(i,
            sizeof(T),
            std::is_integral_v<T>,
            presume_type<T>());
        }

    template <class T>
    constexpr static array_buffer_type init([[maybe_unused]] T t){
        assert(false && "Invalid type.");
        return array_buffer_type(0,0,0,GL_draw_type::BYTE);
    }

    bool operator==(const array_buffer_type& other) const = default;

    bool operator==(const ArrayBuffer& other) const;

};


// TODO turn this into a vector
// try to get 50+% of methods
class ArrayBuffer : public array_buffer_type {
 private:
    GLuint buffer_ID_;

    GLuint devisor_; // for instancing

    size_t size;
    size_t aloc_size;

    const buffer_type buffer_type_;


 public:

    template <class T>
    void update(std::vector<T> data, GLuint devisor);

    template <class T>
    inline void update(std::vector<T> data ) {update<T>(data, devisor_ ); }


    template <class T>
    inline void update(size_t offset, std::vector<T> data);// {update<T>(data, devisor_ ); }

    inline void set_devisor(GLuint devisor) noexcept {devisor_ = devisor;}

    template <class T>
    inline ArrayBuffer(std::vector<T> data ) : ArrayBuffer(data, 0, buffer_type::ARRAY_BUFFER) {}

    template <class T>
    ArrayBuffer(std::vector<T> data, GLuint devisor, buffer_type bt);

    template <class T>
    inline ArrayBuffer() : array_buffer_type(array_buffer_type::presume_type<T>()), buffer_ID_(0), devisor_(0), buffer_type_(buffer_type::ARRAY_BUFFER) {}

    ~ArrayBuffer();

    void bind(GLuint attribute, GLuint index) const;

    inline void bind(GLuint attribute) const {bind(attribute, attribute);};

};

template <class T>
ArrayBuffer::ArrayBuffer(std::vector<T> buffer_data, GLuint devisor, buffer_type bt) :
        array_buffer_type(array_buffer_type::init(T())),
        devisor_(devisor), buffer_type_(bt)
    {
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
ArrayBuffer::update(std::vector<T> buffer_data, GLuint devisor) {

    devisor_ = devisor;
    update(0, buffer_data);

}

// go do vector implementations

template <class T>
void
ArrayBuffer::update(size_t offset, std::vector<T> buffer_data) {

    bool classes_are_equivelent = operator==(array_buffer_type::init(T()));
    assert(classes_are_equivelent && "Class must be the same");

    if (aloc_size < offset + buffer_data.size()){
        // reallocate

            glDeleteBuffers(1, &buffer_ID_);

    glGenBuffers(1, &buffer_ID_);
    glBindBuffer(static_cast<GLenum>(buffer_type_), buffer_ID_);
    glBufferData(
        static_cast<GLenum>(buffer_type_), buffer_data.size() * type_size_ * vec_size_,
        buffer_data.data(),
        GL_DYNAMIC_DRAW // TODO
    );
    }
    else {

        glBindBuffer(static_cast<GLenum>(buffer_type_), buffer_ID_);
        glBufferSubData(static_cast<GLenum>(buffer_type_), offset, buffer_data.size(), buffer_data.data() );

    /*glBindBuffer(static_cast<GLenum>(buffer_type_), buffer_ID_);
    glBufferData(
        static_cast<GLenum>(buffer_type_), buffer_data.size() * type_size_ * vec_size_,
        buffer_data.data(),
        GL_DYNAMIC_DRAW
    );*/
    }
}

} // namespace data_structures

} // namespace gui
