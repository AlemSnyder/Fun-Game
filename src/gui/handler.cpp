#include "handler.hpp"
#include "logging.hpp"

namespace gui {

void
FrameBufferHandler::bind_fbo(GLuint new_fbo) {
    if (new_fbo == fbo_id) {
        // already bound
        return;
    } else {
        fbo_id = new_fbo;
        glBindFramebuffer(GL_FRAMEBUFFER, new_fbo);
    }
    LOG_BACKTRACE(logging::opengl_logger, "Framebuffer {} bound to GL_FRAMEBUFFER", fbo_id);
}

void
VertexBufferHandler::bind_vertex_buffer(GLuint new_vertex_buffer) {
    if (new_vertex_buffer == vertex_buffer) {
        // already bound
        return;
    }
    vertex_buffer = new_vertex_buffer;
    glBindVertexArray(vertex_buffer);
}

} // namespace gui
