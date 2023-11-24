#include "handler.hpp"

namespace gui {

void
FrameBufferHandler::bind_fbo(GLuint new_fbo) {
    if (new_fbo == fbo_id) {
        // already bound
        return;
    } else {
        fbo_id = new_fbo;
        glBindFramebuffer(GL_FRAMEBUFFER, new_fbo);
    }
}

} // namespace gui
