
#include "frame_buffer_multisample.hpp"

#include "gui/handler.hpp"
#include "logging.hpp"
#include "types.hpp"

namespace gui {

namespace gpu_data {

FrameBufferMultisample::FrameBufferMultisample(
    screen_size_t width, screen_size_t height, FrameBufferSettings settings
) :
    FrameBufferBase(width, height, settings) {
    connect_depth_texture(std::make_shared<RenderBuffer>(
        width_, height_,
        RenderBufferSettings{
            .samples = settings_.samples,
            .multisample = settings_.samples > 1,
            .internal_format = GPUPixelStorageFormat::DEPTH}
    ));
    connect_render_texture(
        std::make_shared<Texture2D>(
            width_, height_,
            TextureSettings{
                .samples = settings.samples,
                .multisample = (settings_.samples > 1),
                .internal_format = GPUPixelStorageFormat::RGB8},
            false
        ),
        0
    );

    status_check();
}

} // namespace gpu_data

} // namespace gui
