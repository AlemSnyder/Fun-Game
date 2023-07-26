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
