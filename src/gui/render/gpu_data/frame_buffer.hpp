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
 * @brief Defines FrameBufferBase
 *
 * @ingroup GUI  GPU_DATA
 *
 */

#pragma once

#include "data_types.hpp"
#include "render_buffer.hpp"
#include "texture.hpp"
#include "types.hpp"
#include "logging.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gui {

namespace gpu_data {

struct FrameBufferSettings {
    uint8_t samples;
};

/**
 * @brief Multisample Framebuffer object
 *
 * @details Creates and handles a GPU frame buffer. Creates color texture,
 * and depth texture.
 *
 */
class FrameBufferBase {
 protected:
    screen_size_t width_;
    screen_size_t height_;
    FrameBufferSettings settings_;
    GLuint frame_buffer;
    std::shared_ptr<GPUDataRenderBuffer> depth_buffer_;
    std::unordered_map<uint8_t, std::shared_ptr<GPUDataRenderBuffer>> render_texture_;

 public:
    /**
     * @brief Delete constructor method
     *
     * @details This is effectively a pointer. Copping this will delete the framebuffer
     * and leave the class in an invalid state.
     *
     * @param FrameBufferBase
     */
    FrameBufferBase(const FrameBufferBase& obj) = delete;
    FrameBufferBase& operator=(const FrameBufferBase& obj) = delete;

    /**
     * @brief Move constructors
     *
     * @details Move constructors are valid because the deconstructor will not be
     * called.
     *
     * @param FrameBufferBase
     */
    FrameBufferBase(FrameBufferBase&& obj) = default;
    FrameBufferBase& operator=(FrameBufferBase&& obj) = default;

    /**
     * @brief Delete constructor method
     *
     * @param screen_size_t width of frame buffer
     * @param screen_size_t height of frame buffer
     */
    FrameBufferBase(
        screen_size_t width, screen_size_t height, FrameBufferSettings settings = {}
    );

    ~FrameBufferBase() { glDeleteFramebuffers(1, &frame_buffer); }

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
        return settings_.samples;
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
     * @brief Get id of multisample image texture
     *
     * @return GLuint render texture id
     */
    [[nodiscard]] inline const std::shared_ptr<GPUDataRenderBuffer>
    get_texture(uint8_t id) const {
        return render_texture_.at(id);
    }

    /**
     * @brief Get id of depth buffer
     *
     * @return GLuint depth buffer id
     */
    [[nodiscard]] inline std::shared_ptr<GPUDataRenderBuffer>
    get_depth_buffer() const {
        return depth_buffer_;
    }

    inline void
    connect_depth_texture(std::shared_ptr<GPUDataRenderBuffer> depth_texture) {
        depth_buffer_ = depth_texture;
        depth_texture->connect_depth_texture(frame_buffer);
    }

    inline void
    connect_render_texture(
        std::shared_ptr<GPUDataRenderBuffer> texture, uint8_t texture_attachment
    ) {
        render_texture_[texture_attachment] = texture;
        texture->connect_texture(frame_buffer, texture_attachment);
    }

    inline bool
    status_check() const {
        GLuint framebuffer_status =
            glCheckNamedFramebufferStatus(frame_buffer, GL_FRAMEBUFFER);

        if (framebuffer_status != GL_FRAMEBUFFER_COMPLETE) {
            // log some error
            LOG_CRITICAL(
                logging::opengl_logger, "Framebuffer {} Incomplete with code {}",
                frame_buffer, framebuffer_status
            );
            return false;
        }
        return true;
    }
};

class FrameBuffer : public FrameBufferBase {
 public:
    FrameBuffer(
        screen_size_t width, screen_size_t height, FrameBufferSettings settings = {}
    );
};


} // namespace gpu_data

} // namespace gui
