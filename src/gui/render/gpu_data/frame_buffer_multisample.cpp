
#include "frame_buffer_multisample.hpp"

#include "gui/handler.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace gui {

namespace gpu_data {

FrameBuffer::FrameBuffer(
    screen_size_t width, screen_size_t height, FrameBufferSettings settings
) : width_(width), height_(height), settings_(settings) {

    // frame buffer (the container for the other two)
    // -----------------
    glGenFramebuffers(1, &frame_buffer);
    FrameBufferHandler::instance().bind_fbo(frame_buffer);
}

FrameBufferMultisample::FrameBufferMultisample(
        screen_size_t width, screen_size_t height, FrameBufferSettings settings
) : FrameBuffer(width, height, settings){

    connect_depth_texture(std::make_shared<RenderBuffer>(width_, height_, RenderBufferSettings{.samples = settings_.samples} ));
    connect_render_texture(std::make_shared<Texture2D>(width_, height_, TextureSettings{.samples = settings.samples}), 0);


}

} // namespace gpu_data

} // namespace gui
