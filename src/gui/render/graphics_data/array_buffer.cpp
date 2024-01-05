#include "array_buffer.hpp"

#include <type_traits>

namespace gui {

namespace data_structures {

array_buffer_type::array_buffer_type(std::integral auto i) :
    array_buffer_type(1, sizeof(i), true, presume_type(i)) {}

template <int i, class T, glm::qualifier Q>
array_buffer_type::array_buffer_type(glm::vec<i, T, Q> vec) :
    array_buffer_type(i, sizeof(T), std::is_integral_v<T>(), presume_type(i)) {}

array_buffer_type::array_buffer_type(
    int vec_size, int type_size, bool is_int, GL_draw_type draw_type
) :
    vec_size_(vec_size),
    type_size_(type_size), is_int_(is_int), draw_type_(draw_type){};

template <class T>
array_buffer_type::array_buffer_type(T other) {
    assert(false && "Invalid type.");
}

constexpr GL_draw_type
array_buffer_type::presume_type(std::integral auto i) {
    switch (sizeof(i)) {
        case sizeof(int8_t):
            if (std::is_unsigned_v(i))
                return GL_draw_type::UNSIGNED_BYTE;
            else
                return GL_draw_type::BYTE;

        case (sizeof(int16_t)):
            if (std::is_unsigned_v(i))
                return GL_draw_type::UNSIGNED_SHORT;
            else
                return GL_draw_type::SHORT;

        case (sizeof(int32_t)):
            if (std::is_unsigned_v(i))
                return GL_draw_type::UNSIGNED_INT;
            else
                return GL_draw_type::INT;

        default:
            assert(false && "Invalid type.");
            break;
    }
}

ArrayBuffer::~ArrayBuffer() {
    glDeleteBuffers(1, &buffer_ID_);
}

template <class T>
ArrayBuffer::ArrayBuffer(std::vector<T> buffer_data, GLuint devisor) :
    array_buffer_type(buffer_data[0]), devisor_(devisor) {
    glGenBuffers(1, &buffer_ID_);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ID_);
    glBufferData(
        GL_ARRAY_BUFFER, buffer_data.size() * type_size_, buffer_data.data(),
        GL_STATIC_DRAW
    );
}

void
ArrayBuffer::bind(GLuint attribute, GLuint index) const {
    glEnableVertexAttribArray(index);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_ID_);

    if (is_int_) {
        glVertexAttribIPointer(
            attribute,                       // attribute
            type_size_,                      // size
            static_cast<GLenum>(draw_type_), // type
            0,                               // stride
            (void*)0                         // array buffer offset
        );
    } else {
        glVertexAttribPointer(
            attribute,  // attribute
            type_size_, // size
            GL_FLAT,    // type
            false,      // normalize
            0,          // stride
            (void*)0    // array buffer offset
        );
    }

    glVertexAttribDivisor(index, devisor_);
}

} // namespace data_structures

} // namespace gui