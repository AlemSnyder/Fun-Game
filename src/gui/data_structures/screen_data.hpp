#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace gui {

class ScreenData {
 private:
    GLuint vertex_buffer_;
    GLuint element_buffer_;
    unsigned int num_vertices_;

 public:
    inline ScreenData(const ScreenData& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
    };

    // copy operator
    inline ScreenData& operator=(const ScreenData& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
        return *this;
    }

    ScreenData();

    inline ~ScreenData() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &element_buffer_);
    }

    inline GLuint get_vertex_buffer() const { return vertex_buffer_; }

    inline GLuint get_element_buffer() const { return element_buffer_; }

    inline unsigned int get_num_vertices() const { return num_vertices_; }
};

} // namespace terrain
