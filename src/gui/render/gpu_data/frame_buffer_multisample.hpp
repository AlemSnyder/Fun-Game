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
 * @ingroup GUI  GPU_DATA
 *
 */

#pragma once

#include "frame_buffer.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gui {

namespace gpu_data {

class FrameBufferMultisample : public FrameBufferBase {
 public:
    FrameBufferMultisample(
        screen_size_t width, screen_size_t height, FrameBufferSettings settings
    );
};

} // namespace gpu_data

} // namespace gui
