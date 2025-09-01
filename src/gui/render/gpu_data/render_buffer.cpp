#include "render_buffer.hpp"
#include "logging.hpp"
#include "gui/handler.hpp"

namespace gui {

namespace gpu_data {

void
RenderBuffer::connect_texture(GLuint framebuffer_ID, uint8_t texture_attachment) {
    LOG_BACKTRACE(logging::opengl_logger, "Attaching {} to depth texture.", buffer_ID_);
    FrameBufferHandler::instance().bind_fbo(framebuffer_ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_attachment, GL_RENDERBUFFER, buffer_ID_, 0);
}

void
RenderBuffer::connect_depth_texture(GLuint framebuffer_ID) {
    // lots of ways to mess this up
    LOG_BACKTRACE(logging::opengl_logger, "Attaching {} to depth texture.", buffer_ID_);
    FrameBufferHandler::instance().bind_fbo(framebuffer_ID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, buffer_ID_, 0);

}

}

}
