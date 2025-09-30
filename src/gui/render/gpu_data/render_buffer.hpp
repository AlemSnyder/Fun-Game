#include "data_types.hpp"
#include "gui/render/gl_enums.hpp"
#include "types.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace gui {

namespace gpu_data {

struct RenderBufferSettings {
    uint8_t samples = 0;
    bool multisample = false;
    GPUPixelStorageFormat internal_format = GPUPixelStorageFormat::RGB8;
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

    inline RenderBuffer(
        screen_size_t width, screen_size_t height, RenderBufferSettings settings
    ) :
        RenderBuffer(settings) {
        bind();
        if (settings_.multisample) {
            glRenderbufferStorageMultisample(
                GL_RENDERBUFFER, settings_.samples,
                static_cast<GLenum>(settings_.internal_format), width, height
            );
        } else {
            assert(
                settings_.samples <= 1 && "Not multisampled. Samples should be one."
            );
            glRenderbufferStorage(
                GL_RENDERBUFFER, static_cast<GLenum>(settings_.internal_format), width,
                height
            );
        }
    }

    inline virtual ~RenderBuffer() { glDeleteRenderbuffers(1, &buffer_ID_); }

    inline virtual GLuint
    value() const {
        return buffer_ID_;
    }

    virtual void connect_texture(GLuint framebuffer_ID, uint8_t texture_attachment);

    virtual void connect_depth_texture(GLuint framebuffer_ID);

    // render buffers don't really have an format.
    // I don't think one can read from one.
    inline virtual GPUPixelType
    get_type() const {
        return GPUPixelType::NONE;
    }

    inline virtual GPUPixelStorageFormat
    get_format() const {
        return settings_.internal_format;
    }

 private:
    inline void
    bind() {
        // TODO create binder
        glBindRenderbuffer(GL_RENDERBUFFER, buffer_ID_);
    }
};

} // namespace gpu_data

} // namespace gui
