// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */

/**
 * @file frame_buffer.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines FrameBuffer
 *
 * @ingroup GUI  GPU_DATA
 *
 */

#pragma once

#include "types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gui {

namespace gpu_data {

/**
 * @brief  Framebuffer object
 *
 * @details Creates and handles a GPU frame buffer. Creates color texture,
 * and depth texture.
 *
 */
class FrameBuffer {
 private:
    GLuint frame_buffer;
    GLuint render_texture;
    GLuint depth_buffer;
    screen_size_t width_;
    screen_size_t height_;

 public:
    /**
     * @brief Delete constructor method
     *
     * @details This is effectively a pointer. Copping this will delete the framebuffer
     * and leave the class in an invalid state.
     *
     * @param FrameBuffer
     */
    FrameBuffer(const FrameBuffer& obj) = delete;
    FrameBuffer& operator=(const FrameBuffer& obj) = delete;

    /**
     * @brief Move constructors
     *
     * @details Move constructors are valid because the deconstructor will not be
     * called.
     *
     * @param FrameBuffer
     */
    FrameBuffer(FrameBuffer&& obj) = default;
    FrameBuffer& operator=(FrameBuffer&& obj) = default;

    /**
     * @brief constructor method
     *
     * @param screen_size_t width of frame buffer
     * @param screen_size_t height of frame buffer
     */
    FrameBuffer(screen_size_t width, screen_size_t height);

    ~FrameBuffer() {
        glDeleteTextures(1, &depth_buffer);
        glDeleteTextures(1, &render_texture);
        glDeleteFramebuffers(1, &frame_buffer);
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
        return frame_buffer;
    }

    /**
     * @brief Get id of  image texture
     *
     * @return GLuint render texture id
     */
    [[nodiscard]] inline GLuint
    get_texture_name() const {
        return render_texture;
    }

    /**
     * @brief Get id of depth buffer
     *
     * @return GLuint depth buffer id
     */
    [[nodiscard]] inline GLuint
    get_depth_buffer_name() const {
        return depth_buffer;
    }
};

} // namespace gpu_data

} // namespace gui
