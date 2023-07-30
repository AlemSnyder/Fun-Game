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
 * @file gui_render_types.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines render types virtual classes
 *
 * @ingroup GUI  RENDER_TO
 *
 */

#include <GLFW/glfw3.h>

#pragma once

namespace gui {

namespace render_to {

/*
 * Defines virtual classes for types of render methods
 */

class frame_buffer {
 public:
    virtual int render_frame_buffer(GLFWwindow* window, GLuint frame_buffer) const = 0;
};

class frame_buffer_multisample {
 public:
    virtual int
    render_frame_buffer_multisample(GLFWwindow* window, GLuint frame_buffer) const = 0;
};

class shadow_map {
 public:
    virtual int render_shadow_map(
        int shadow_width_, int shadow_height_, GLuint frame_buffer
    ) const = 0;
};

// blume

} // namespace render_to

} // namespace gui
