#pragma once

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


// cursed
class array_buffer_type {
 protected:
    const uint8_t vec_size_ : 2; // 1,2,3 or 4 vec size
    const uint8_t type_size_;    // size of type float, int etc.
    const bool is_int_ : 1;      // should be interpreted as integer
    const GL_draw_type draw_type_;


 public:

    array_buffer_type(int vec_size, int type_size, bool is_int, GL_draw_type draw_type);

    template <std::integral T>
    constexpr GL_draw_type presume_type() {
        switch (sizeof(T)) {
        case sizeof(int8_t):
            if (std::is_unsigned_v<T>)
                return GL_draw_type::UNSIGNED_BYTE;
            else
                return GL_draw_type::BYTE;

        case (sizeof(int16_t)):
            if (std::is_unsigned_v<T>)
                return GL_draw_type::UNSIGNED_SHORT;
            else
                return GL_draw_type::SHORT;

        case (sizeof(int32_t)):
            if (std::is_unsigned_v<T>)
                return GL_draw_type::UNSIGNED_INT;
            else
                return GL_draw_type::INT;

        default:
            assert(false && "Invalid type.");
            break;
        }
    }

    template <std::integral T>
    inline constexpr static array_buffer_type init() {
        return array_buffer_type(1, sizeof(T), true, presume_type<T>());
    }

    template <int i, class T, glm::qualifier Q>
    constexpr static array_buffer_type init(){
        return array_buffer_type(i,
            sizeof(T),
            std::is_integral_v<T>(),
            presume_type<T>());
        }

    template <class T>
    constexpr static array_buffer_type init(){
        assert(false && "Invalid type.");
        return {0,0,0,GL_draw_type::BYTE};
    }

};

class ArrayBuffer : public array_buffer_type {
 private:
    GLuint buffer_ID_;

    const GLuint devisor_; // for instancing

 public:
    template <class T>
    ArrayBuffer(std::vector<T> data ) : ArrayBuffer(data, 0) {}

    template <class T>
    inline ArrayBuffer(std::vector<T> data, GLuint devisor);

    ~ArrayBuffer();

    void bind(GLuint attribute, GLuint index) const;

    inline void bind(GLuint attribute) const {bind(attribute, attribute);};

};

} // namespace data_structures
} // namespace gui
