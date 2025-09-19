
#include "frame_buffer.hpp"

#include "gui/handler.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace gui {

namespace gpu_data {

FrameBufferBase::FrameBufferBase(
    screen_size_t width, screen_size_t height, FrameBufferSettings settings
) :
    width_(width),
    height_(height), settings_(settings) {
    // frame buffer (the container for the other two)
    // -----------------
    glGenFramebuffers(1, &frame_buffer);
    FrameBufferHandler::instance().bind_fbo(frame_buffer);
}

FrameBuffer::FrameBuffer(
    screen_size_t width, screen_size_t height, FrameBufferSettings settings
) :
    FrameBufferBase(width, height, settings) {
    connect_depth_texture(std::make_shared<Texture2D>(
        width_, height_,
        TextureSettings{
            .internalformat = GL_DEPTH_COMPONENT,
            .format = GL_DEPTH_COMPONENT,
            .type = GL_FLOAT}
    ));
    connect_render_texture(
        std::make_shared<Texture2D>(
            width_, height_,
            TextureSettings{
                .internalformat = GL_RGB8}
        ),
        0
    );

    status_check();
}

void
FrameBufferBase::copy_to(
    FrameBufferBase* other, GLbitfield mask, GLenum filter,
    std::array<screen_size_t, 8> params
) const {
    glBlitNamedFramebuffer(
        get_frame_buffer_id(), other->get_frame_buffer_id(),
        params[0], params[1], params[2], params[3],
        params[4], params[5], params[6], params[7], // region of framebuffer
        mask, filter // copy the color
    );
}

} // namespace gpu_data

} // namespace gui
