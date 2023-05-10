#include "handler.hpp"

namespace gui {

GLuint FrameBufferHandler::fbo_name = 0;

void
FrameBufferHandler::bind_fbo(GLuint new_fbo) {
    if (new_fbo == fbo_name) {
        // already bound
        return;
    } else {
        fbo_name = new_fbo;
        glBindFramebuffer(GL_FRAMEBUFFER, new_fbo);
    }
}

} // namespace gui
