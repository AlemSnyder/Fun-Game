#include "types.hpp"
#include "data_types.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace gui {

namespace gpu_data {

struct RenderBufferSettings {
    uint8_t samples = 1;
    bool multisample = false;
    GLenum internalformat = GL_RGB8;
};

class RenderBuffer : virtual public GPUDataRenderBuffer {
 private:
    screen_size_t width_;
    RenderBufferSettings settings_;
    GLuint buffer_ID_;
 public:
    inline RenderBuffer(RenderBufferSettings settings = {}) : settings_(settings) {
        glGenRenderbuffers(1, &buffer_ID_);
    }


    inline RenderBuffer(screen_size_t width, screen_size_t height, RenderBufferSettings settings = {.internalformat = GL_DEPTH_ATTACHMENT}) : RenderBuffer(settings) {
        bind();
        if (settings_.multisample) {
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, settings_.samples, settings_.internalformat, width, height);
        } else {
            assert(settings_.samples == 1 && "Not multisampled. Samples should be one.");
            glRenderbufferStorage(GL_RENDERBUFFER, settings_.internalformat, width, height);
        }
    }
    inline virtual ~RenderBuffer() {
        glDeleteRenderbuffers(1, &buffer_ID_);
    }

    inline virtual GLuint value() const {
        return buffer_ID_;
    }

    virtual void connect_texture(GLuint framebuffer_ID, uint8_t texture_attachment);

    virtual void connect_depth_texture(GLuint framebuffer_ID);

 private:
    inline void bind() {
        // TODO create binder
        glBindRenderbuffer(GL_RENDERBUFFER, buffer_ID_);
    }
};



}

}
