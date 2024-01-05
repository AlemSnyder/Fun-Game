#include "array_buffer.hpp"

#include <type_traits>

namespace gui {

namespace data_structures {

array_buffer_type::array_buffer_type(
    int vec_size, int type_size, bool is_int, GL_draw_type draw_type
) :
    vec_size_(vec_size),
    type_size_(type_size), is_int_(is_int), draw_type_(draw_type){};

ArrayBuffer::~ArrayBuffer() {
    glDeleteBuffers(1, &buffer_ID_);
}

template <class T>
ArrayBuffer::ArrayBuffer(std::vector<T> buffer_data, GLuint devisor) :
    array_buffer_type(array_buffer_type::init(buffer_data[0])), devisor_(devisor) {
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