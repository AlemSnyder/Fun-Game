
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#pragma once

namespace gui {

class FrameBufferMultisample {
 private:
    GLuint frame_buffer;
    GLuint render_texture;
    GLuint depth_buffer;
    GLuint single_sample_texture;
    GLuint frame_buffer_single;
    uint32_t width_;
    uint32_t height_;
    uint32_t samples_;

 public:
    FrameBufferMultisample(const FrameBufferMultisample& obj) = delete;

    FrameBufferMultisample(uint32_t width, uint32_t height, uint32_t samples);

    ~FrameBufferMultisample() {
        glDeleteRenderbuffers(1, &depth_buffer);
        glDeleteTextures(1, &render_texture);
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteTextures(1, &single_sample_texture);
        glDeleteFramebuffers(1, &frame_buffer_single);
    }

    [[nodiscard]] inline uint32_t
    get_width() const {
        return width_;
    }

    [[nodiscard]] inline uint32_t
    get_height() const {
        return height_;
    }

    [[nodiscard]] inline GLuint
    get_num_samples() {
        return samples_;
    }

    [[nodiscard]] inline GLuint
    get_frame_buffer_name() const {
        return frame_buffer;
    }

    [[nodiscard]] inline GLuint
    get_texture_name() const {
        return render_texture;
    }

    [[nodiscard]] inline GLuint
    get_depth_buffer_name() const {
        return depth_buffer;
    }

    [[nodiscard]] inline GLuint
    get_single_sample_texture() {
        return single_sample_texture;
    }

    [[nodiscard]] inline GLuint
    get_frame_buffer_single() {
        return frame_buffer_single;
    }
};

} // namespace gui
