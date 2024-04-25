#pragma once

#include "types.hpp"

#include <GL/glew.h>

namespace gui {
namespace gpu_data {

class FrameBufferShadowMap {
 private:
    GLuint frame_buffer_;
    GLuint depth_texture_;
    screen_size_t width_;
    screen_size_t height_;

 public:

    FrameBufferShadowMap(const FrameBufferShadowMap& obj) = delete;
    FrameBufferShadowMap(const FrameBufferShadowMap&& obj) = delete;
    FrameBufferShadowMap& operator=(const FrameBufferShadowMap&& obj) = delete;

    FrameBufferShadowMap(screen_size_t width, screen_size_t height);

    inline ~FrameBufferShadowMap() {
        glDeleteTextures(1, &depth_texture_);
        glDeleteFramebuffers(1, &frame_buffer_);
    }


    /**
     * @brief Get width of frame buffer
     *
     * @return screen_size_t width
     */
    [[nodiscard]] inline screen_size_t
    get_width() const {
        return width_;
    }

    /**
     * @brief Get height of frame buffer
     *
     * @return screen_size_t height
     */
    [[nodiscard]] inline screen_size_t
    get_height() const {
        return height_;
    }

    /**
     * @brief Get id of frame buffer
     *
     * @return GLuint frame buffer id
     */
    [[nodiscard]] inline GLuint
    get_frame_buffer_id() const {
        return frame_buffer_;
    }

    /**
     * @brief Get id of depth texture
     *
     * @return GLuint depth texture id
     */
    [[nodiscard]] inline GLuint
    get_depth_texture() const {
        return depth_texture_;
    }
};

} // namespace gpu_data

} // namespace gui
