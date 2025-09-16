
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
            .samples = 1,
            .multisample = false,
            .internalformat = GL_DEPTH_COMPONENT,
            .format = GL_DEPTH_COMPONENT,
            .type = GL_FLOAT}
    ));
    connect_render_texture(
        std::make_shared<Texture2D>(
            width_, height_,
            TextureSettings{
                .samples = 1,
                .multisample = false,
                .internalformat = GL_RGB8}
        ),
        0
    );

    status_check();
}

} // namespace gpu_data

} // namespace gui
