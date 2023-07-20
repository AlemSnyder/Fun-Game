
#include "frame_buffer_multisample.hpp"

#include "../../../logging.hpp"
#include "../../handler.hpp"

gui::data_structures::FrameBufferMultisample::FrameBufferMultisample(
    uint32_t width, uint32_t height, uint32_t samples
) {
    width_ = width;
    height_ = height;
    samples_ = samples;

    // generates a frame buffer, screen texture, and and a depth buffer

    // frame buffer (the container for the other two)
    glGenFramebuffers(1, &frame_buffer);
    gui::FrameBufferHandler::bind_fbo(frame_buffer);

    // texture (what it looks like)
    glGenTextures(1, &render_texture);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, render_texture);
    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB8, width_, height_, GL_TRUE
    );

    // depth buffer (how far thins are from the screen for depth)
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, width_, height_
    );
    // connect the render buffer to the frame buffer
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer
    );
    // connect the texture to the frame buffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, render_texture,
        0
    );

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    GLuint framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        // log some error
        LOG_CRITICAL(
            logging::opengl_logger, "Framebuffer Incomplete with code {}",
            framebuffer_status
        );
    }

    // frame buffer (the container for the other two)
    glGenFramebuffers(1, &frame_buffer_single);
    gui::FrameBufferHandler::bind_fbo(frame_buffer_single);

    // texture (what it looks like)
    glGenTextures(1, &render_texture_single);

    glBindTexture(GL_TEXTURE_2D, render_texture_single);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, 0
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // connect the texture to the frame buffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_texture_single, 0
    );

    DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);

    framebuffer_status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
        // log some error
        LOG_CRITICAL(
            logging::opengl_logger, "Framebuffer Incomplete with code {}",
            framebuffer_status
        );
    }
}
