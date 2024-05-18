#include "frame_buffer_shadow_map.hpp"

#include "../../handler.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace gui {

namespace gpu_data {

FrameBufferShadowMap::FrameBufferShadowMap(screen_size_t width, screen_size_t height) :
    width_(width), height_(height) {

    glGenFramebuffers(1, &frame_buffer_);
    FrameBufferHandler::instance().bind_fbo(frame_buffer_);

    // Depth texture. Slower than a depth buffer, but you can sample it later in
    // your shader

    glGenTextures(1, &depth_texture_);
    glBindTexture(GL_TEXTURE_2D, depth_texture_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width_, height_, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture_, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_CRITICAL(logging::opengl_logger, "Framebuffer is not ok");
        abort();
    }
}

} // namespace gpu_data

} // namespace gui
