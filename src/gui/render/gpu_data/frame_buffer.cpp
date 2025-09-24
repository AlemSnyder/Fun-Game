
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
            .internal_format = GPUPixelStorageFormat::DEPTH,
            .read_format = GPUPixelReadFormat::DEPTH_COMPONENT}
    ));
    connect_render_texture(
        std::make_shared<Texture2D>(
            width_, height_,
            TextureSettings{.internal_format = GPUPixelStorageFormat::RGB8}
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
        get_frame_buffer_id(), other->get_frame_buffer_id(), params[0], params[1],
        params[2], params[3], params[4], params[5], params[6],
        params[7],   // region of framebuffer
        mask, filter // copy the color
    );
}

std::shared_ptr<util::image::Image>
FrameBufferBase::read_data(int8_t color_component) const {
    return read_data(0, 0, width_, height_, color_component);
}

std::shared_ptr<util::image::Image>
FrameBufferBase::read_data(
    screen_size_t start_w, screen_size_t start_h, screen_size_t image_w,
    screen_size_t image_h, int8_t color_component
) const {
    GPUPixelType type;
    GPUPixelStorageFormat format;
    GPUPixelReadFormat read_format;

    FrameBufferHandler& fbh = FrameBufferHandler::instance();
    fbh.bind_fbo(frame_buffer); // need to crate a locking accessor

    if (color_component == DEPTH_COMPONENT_ID) {
        if (!depth_buffer_) {
            LOG_ERROR(logging::opengl_logger, "No depth Component to read from");
            return {};
        }
        type = depth_buffer_->get_type();
        format = depth_buffer_->get_format();
        read_format = GPUPixelReadFormat::DEPTH_COMPONENT;

    } /* else if stencil */ else {
        if (!render_texture_.contains(color_component)) {
            LOG_ERROR(
                logging::opengl_logger, "Color component {} does not exist.",
                color_component
            );
            return {};
        }
        const auto& color_texture = render_texture_.at(color_component);
        if (!color_texture) {
            LOG_ERROR(
                logging::opengl_logger, "Color component {} exist, but has no value.",
                color_component
            );
            return {};
        }
        type = color_texture->get_type();
        format = color_texture->get_format();
        switch (format) {
            case GPUPixelStorageFormat::R:
                read_format = GPUPixelReadFormat::RED;
                break;
            case GPUPixelStorageFormat::RGB:
            case GPUPixelStorageFormat::RGB8:
                read_format = GPUPixelReadFormat::RGB;
                break;
            case GPUPixelStorageFormat::RGBA:
                read_format = GPUPixelReadFormat::RGBA;
                break;
            default:
                LOG_ERROR(logging::opengl_logger, "Cannot read from depth texture");
                return {};
        }

        // GLuint attachment = GL_COLOR_ATTACHMENT0 + color_component;
    }
    // type
    // number of
    size_t format_size = get_size(read_format);
    size_t type_size = get_size(type) / sizeof(char);
    std::shared_ptr<char[]> data(new char[image_w * image_h * format_size * type_size]);

    glReadPixels(
        start_w, start_h, image_w, image_h, static_cast<GLenum>(read_format),
        static_cast<GLenum>(type), data.get()
    );

    if (type != GPUPixelType::FLOAT) {
        LOG_ERROR(logging::opengl_logger, "NOT IMPLEMENTED MUST USE FLOAT");
    }

    switch (format) {
        case GPUPixelStorageFormat::R:
        case GPUPixelStorageFormat::DEPTH:
            // in this case format_size should be 1
            return std::make_shared<util::image::FloatMonochromeImage>(
                data, image_w, image_h, format_size
            );
        case GPUPixelStorageFormat::RGB:
            return std::make_shared<util::image::FloatPolychromeImage>(
                data, image_w, image_h, format_size
            );
        case GPUPixelStorageFormat::RGBA:
            return std::make_shared<util::image::FloatPolychromeAlphaImage>(
                data, image_w, image_h, format_size
            );

        default:
            return {};
    }
}

} // namespace gpu_data

} // namespace gui
