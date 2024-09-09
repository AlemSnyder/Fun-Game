#pragma once

#include "gui/handler.hpp"

#include <GL/glew.h>

namespace gui {

namespace gpu_data {

class VertexArrayObject {
 private:
    GLuint vertex_array_;

 public:
    inline VertexArrayObject() {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { glGenVertexArrays(1, &vertex_array_); });
    };

    // copy and move constructor
    VertexArrayObject(const VertexArrayObject& other) = delete;
    VertexArrayObject(VertexArrayObject&& other) = default;
    // assignment operator
    VertexArrayObject& operator=(const VertexArrayObject& other) = delete;
    VertexArrayObject& operator=(VertexArrayObject&& other) = default;

    inline ~VertexArrayObject() {
        auto vertex_array = std::make_shared<GLuint>(vertex_array_);
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([vertex_array]() {
            glDeleteVertexArrays(1, vertex_array.get());
        });
    };

    [[nodiscard]] inline GLuint
    get() {
        return vertex_array_;
    }

    inline void
    bind() const {
        auto& handler = VertexBufferHandler::instance();
        handler.bind_vertex_buffer(vertex_array_);
    }

    inline void
    release() const {
        auto& handler = VertexBufferHandler::instance();
        handler.unbind_vertex_buffer();
    }
};

} // namespace gpu_data

} // namespace gui
