// handles fbo, and other things if necessary

#pragma once

#include <GL/glew.h>

namespace gui {

class FrameBufferHandler {
 private:
    GLuint fbo_id;

    // Private CTOR as this is a singleton
    FrameBufferHandler() : fbo_id(0) {}

 public:
    // Delete all CTORs and CTOR-like operators
    FrameBufferHandler(FrameBufferHandler&&) = delete;
    FrameBufferHandler(FrameBufferHandler const&) = delete;

    void operator=(FrameBufferHandler&&) = delete;
    void operator=(FrameBufferHandler const&) = delete;

    // Instance accessor
    static inline FrameBufferHandler&
    getInstance() {
        static FrameBufferHandler obj;
        return obj;
    }

    inline GLuint
    get_current_fbo() {
        return fbo_id;
    };

    void bind_fbo(GLuint new_fbo_id);
};

} // namespace gui
