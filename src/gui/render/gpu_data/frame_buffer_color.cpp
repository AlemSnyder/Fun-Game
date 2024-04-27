#include "frame_buffer_color.hpp"

#include "../../handler.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace gui {

namespace gpu_data {

FrameBufferColor::FrameBufferColor(screen_size_t width, screen_size_t height) :
    width_(width), height_(height) {
    glGenFramebuffers(1, &frame_buffer_);
    FrameBufferHandler::instance().bind_fbo(frame_buffer_);

    // texture
    glGenTextures(1, &render_texture_);
    glBindTexture(GL_TEXTURE_2D, render_texture_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT,
        nullptr
    );

    // connect the texture to the frame buffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture_, 0
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
