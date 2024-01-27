#include "array_buffer.hpp"

#include <type_traits>

namespace gui {

namespace data_structures {

bool
array_buffer_type::operator==(const ArrayBuffer& other) const {
    return operator==(array_buffer_type(other));
}

ArrayBuffer::~ArrayBuffer() {
    glDeleteBuffers(1, &buffer_ID_);
}

void
ArrayBuffer::bind(GLuint attribute, GLuint index) const {
    if (buffer_type_ != buffer_type::ELEMENT_ARRAY_BUFFER)
        glEnableVertexAttribArray(index);

    glBindBuffer(static_cast<GLenum>(buffer_type_), buffer_ID_);

    switch (buffer_type_) {
        case buffer_type::ARRAY_BUFFER:
            if (is_int_) {
                glVertexAttribIPointer(
                    attribute,                       // attribute
                    vec_size_,                       // size
                    static_cast<GLenum>(draw_type_), // type
                    vec_size_ * type_size_,          // stride
                    (void*)0                         // array buffer offset
                );
            } else {
                glVertexAttribPointer(
                    attribute,                 // attribute
                    vec_size_,                 // size
                    GL_FLOAT,                  // type
                    false,                     // normalize
                    vec_size_ * sizeof(float), // stride
                    (void*)0                   // array buffer offset
                );
            }

            glVertexAttribDivisor(index, devisor_);
            break;

        default:
            break;
    }
}

} // namespace data_structures

} // namespace gui