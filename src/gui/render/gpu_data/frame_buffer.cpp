
#include "frame_buffer.hpp"

#include "gui/handler.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace gui {

namespace gpu_data {

FrameBuffer::FrameBuffer(screen_size_t width, screen_size_t height) {
    // Set the frame buffer and texture width and height
    width_ = width;
    height_ = height;

    // generates a frame buffer, screen texture, and and a depth buffer

    // frame buffer (the container for the other two)
    // -----------------
    glGenFramebuffers(1, &frame_buffer);
    FrameBufferHandler::instance().bind_fbo(frame_buffer);

    // texture
    glGenTextures(1, &render_texture);
    glBindTexture(GL_TEXTURE_2D, render_texture);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, 0
    );

    // depth buffer (how far thins are from the screen for depth)
    glGenTextures(1, &depth_buffer);
    glBindTexture(GL_TEXTURE_2D, depth_buffer);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT,
        GL_FLOAT, 0
    );
    // connect the render buffer to the frame buffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_buffer, 0
    );
    // connect the texture to the frame buffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture, 0
    );

    GLuint framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        // log some error
        LOG_CRITICAL(
            logging::opengl_logger, "Framebuffer Incomplete with code {}",
            framebuffer_status
        );
    }
}

} // namespace gpu_data

} // namespace gui
