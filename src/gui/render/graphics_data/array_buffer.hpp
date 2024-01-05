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
    template <std::integral T>
    array_buffer_type(T i);

    template <int i, class T, glm::qualifier Q>
    array_buffer_type(glm::vec<i, T, Q> vec);

    template <class T>
    array_buffer_type(T other);

    array_buffer_type(int vec_size, int type_size, bool is_int, GL_draw_type draw_type);

    constexpr GL_draw_type presume_type(std::integral auto i);
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
