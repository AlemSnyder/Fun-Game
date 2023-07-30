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
 * @file frame_buffer_multisample.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines FrameBufferMultisample
 *
 * @ingroup GUI  DATA_STRUCTURES
 *
 */

#pragma once

#include "../../../types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gui {

namespace data_structures {

/**
 * @brief Handles Frame Buffer
 *
 * @details Creates and handles a GPU frame buffer. Creates color texture,
 * and depth texture.
 *
 */
class FrameBufferMultisample {
 private:
    GLuint frame_buffer;
    GLuint render_texture;
    GLuint depth_buffer;
    GLuint frame_buffer_single;
    GLuint render_texture_single;
    screen_size_t width_;
    screen_size_t height_;
    uint32_t samples_;

 public:
    /**
     * @brief Delete constructor method
     *
     * @param obj
     */
    FrameBufferMultisample(const FrameBufferMultisample& obj) = delete;

    /**
     * @brief Delete constructor method
     *
     * @param screen_size_t width of frame buffer
     * @param screen_size_t height of frame buffer
     */
    FrameBufferMultisample(screen_size_t width, screen_size_t height, uint32_t samples);

    ~FrameBufferMultisample() {
        glDeleteRenderbuffers(1, &depth_buffer);
        glDeleteTextures(1, &render_texture);
        glDeleteFramebuffers(1, &frame_buffer);
        glDeleteTextures(1, &render_texture_single);
        glDeleteFramebuffers(1, &frame_buffer_single);
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
     * @brief Get the number of samples in each pixel
     *
     * @return GLuint number of samples
     */
    [[nodiscard]] inline GLuint
    get_num_samples() {
        return samples_;
    }

    /**
     * @brief Get id of frame buffer
     *
     * @return GLuint frame buffer id
     */
    [[nodiscard]] inline GLuint
    get_frame_buffer_name() const {
        return frame_buffer;
    }

    /**
     * @brief Get id of multisample image texture
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

    /**
     * @brief Get id of single-sample image texture
     *
     * @return GLuint render texture single id
     */
    [[nodiscard]] inline GLuint
    get_single_sample_texture() {
        return render_texture_single;
    }

    /**
     * @brief Get id of single-sample frame buffer
     *
     * @return GLuint frame buffer single id
     */
    [[nodiscard]] inline GLuint
    get_frame_buffer_single() {
        return frame_buffer_single;
    }
};

} // namespace data_structures

} // namespace gui
