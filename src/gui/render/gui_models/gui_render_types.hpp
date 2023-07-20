#pragma once

namespace gui {

namespace render_to {

/*
 * Defines virtual classes for types of render methods
 */

class frame_buffer {
    virtual int render_frame_buffer() const = 0;
};

class frame_buffer_multisample {
    virtual int render_frame_buffer_multisample() const = 0;
};

class shadow_map {
    virtual int render_shadow_map() const = 0;
};

// blume

} // namespace render_to

} // namespace gui
