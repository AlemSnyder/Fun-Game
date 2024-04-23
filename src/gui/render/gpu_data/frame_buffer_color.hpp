#pragma once

#include "types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gui {

namespace gpu_data {

class FrameBufferColor {
 private:
    screen_size_t width_;
    screen_size_t height_;

    GLuint frame_buffer_;
    GLuint render_texture_;

 public:
    /**
     * @brief Delete constructor method
     *
     * @param FrameBufferColor
     */
    FrameBufferColor(const FrameBufferColor& obj) = delete;
    FrameBufferColor(const FrameBufferColor&& obj) = delete;
    FrameBufferColor& operator=(const FrameBufferColor&& obj) = delete;


    /**
     * @brief Delete constructor method
     *
     * @param screen_size_t width of frame buffer
     * @param screen_size_t height of frame buffer
     */
    FrameBufferColor(screen_size_t width, screen_size_t height);

    inline ~FrameBufferColor() {
        glDeleteTextures(1, &render_texture_);
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
     * @brief Get id of multisample image texture
     *
     * @return GLuint render texture id
     */
    [[nodiscard]] inline GLuint
    get_texture_name() const {
        return render_texture_;
    }

};

} // namespace gpu_data

} // namespace gui
