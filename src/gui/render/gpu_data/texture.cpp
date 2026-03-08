#include "texture.hpp"

#include "gui/handler.hpp"
#include "logging.hpp"

namespace gui {

namespace gpu_data {

Texture1D::Texture1D(const std::vector<ColorFloat>& color_data) {
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this, color_data]() {
        // create one texture and save the id to color_texture_
        glGenTextures(1, &texture_ID_);
        // bind to color_texture_
        glBindTexture(GL_TEXTURE_1D, texture_ID_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // set texture warp. values outside of texture bounds are clamped to
        // edge of texture
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // no minimapping so this one don't matter
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // set magnification filter to linear. Interpolate between pixels in mat
        glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        glTexImage1D(
            GL_TEXTURE_1D, 0, GL_RGBA32F, color_data.size(), 0, GL_RGBA, GL_FLOAT,
            color_data.data()
        );
        glGenerateMipmap(GL_TEXTURE_1D);
    });
}

void
Texture2D::connect_texture(GLuint framebuffer_ID, uint8_t texture_attachment) {
#if DEBUG()
    GlobalContext& context = GlobalContext::instance();
    if (!context.is_main_thread()) {
        LOG_CRITICAL(
            logging::opengl_logger,
            "Opengl interface calls must be done on the main thread."
        );
    }
#endif

    FrameBufferHandler& handler = FrameBufferHandler::instance();
    handler.bind_fbo(framebuffer_ID);
    GLenum target = settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_attachment, target, texture_ID_,
        0
    );
}

void
Texture2D::connect_depth_texture(GLuint framebuffer_ID) {
#if DEBUG()
    GlobalContext& context = GlobalContext::instance();
    if (!context.is_main_thread()) {
        LOG_CRITICAL(
            logging::opengl_logger,
            "Opengl interface calls must be done on the main thread."
        );
    }
#endif

    FrameBufferHandler& handler = FrameBufferHandler::instance();
    handler.bind_fbo(framebuffer_ID);
    GLenum target = settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture_ID_, 0);
}

void
Texture2D::load_settings() {
#if DEBUG()
    GlobalContext& context = GlobalContext::instance();
    if (!context.is_main_thread()) {
        LOG_CRITICAL(
            logging::opengl_logger,
            "Opengl interface calls must be done on the main thread."
        );
    }
#endif

    // must set width and height before calling this function.
    GLenum target = settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

    glGenTextures(1, &texture_ID_);
    glBindTexture(target, texture_ID_);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (!settings_.multisample) {
        glTexParameteri(target, GL_TEXTURE_WRAP_S, settings_.wrap_s);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, settings_.wrap_t);
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, settings_.min_filter);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, settings_.mag_filter);
        glTexParameteri(target, GL_TEXTURE_COMPARE_FUNC, settings_.compare_funct);
        glTexParameteri(target, GL_TEXTURE_COMPARE_MODE, settings_.compare_mode);
    }
    // set other paremeters
}

void
Texture2D::setup() {
    // bind
    if (settings_.multisample) {
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, settings_.samples,
            static_cast<GLenum>(settings_.internal_format), width_, height_, GL_TRUE
        );
    } else {
        glTexImage2D(
            GL_TEXTURE_2D, 0, static_cast<GLenum>(settings_.internal_format),
            width_, height_, 0, static_cast<GLenum>(settings_.read_format),
            static_cast<GLenum>(settings_.type), nullptr
        );
    }
}

Texture2D::Texture2D(
    screen_size_t width, screen_size_t height, TextureSettings settings, bool differed
) : width_(width), height_(height), settings_(settings) {
    if (differed) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this]() { load_settings(); setup(); });
    } else {
        load_settings();
        setup();
    }
}

Texture2D::Texture2D(
    util::image::ImageVariant image, TextureSettings settings, bool differed
) :
    settings_(settings) {
//    width_ = image->get_width();
//    height_ = image->get_height();

    if (differed) {
        GlobalContext& context = GlobalContext::instance();
        context.push_opengl_task([this, image]() { load_settings(); load_image(image); });
    } else {
        load_settings();
        load_image(image);
    }
}

void
Texture2D::load_image(util::image::ImageVariant image) {
    const auto visitor = util::image::ImageVisitor(
        [this](auto&& image) {this->load_data(image);}
    );

    std::visit(visitor, image);
}

std::expected<util::image::ImageVariant, int>
Texture2D::get_image() const {
    // multiplying by 2 fixes the memory error, but that can't possibly be correct.
    // TODO GL_PACK_ALIGNMENT
    size_t data_size = width_ * height_ * get_size(settings_.type)
                       * get_size(settings_.read_format) * 2;

//    std::shared_ptr<char[]> data = std::make_shared<char[]>(data_size);

    if (settings_.multisample) {
        LOG_ERROR(logging::opengl_logger, "Cannot load multisample texture to image.");
        return std::unexpected(1);
    }
    glBindTexture(GL_TEXTURE_2D, texture_ID_);

#if DEBUG()
    int width;
    int height;
    int opengl_type;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
    glGetTexLevelParameteriv(
        GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &opengl_type
    );

    if (width != width_ || height != height_) {
        LOG_WARNING(
            logging::opengl_logger,
            "Width or Height don't match correct value. Actual {}, {}, Saved {}, {}.",
            width, height, width_, height_
        );
    }

    if (gui::gpu_data::GPUPixelStorageFormat(opengl_type)
        != settings_.internal_format) {
        LOG_WARNING(
            logging::opengl_logger, "Type sizes don't match. Actual {}, Given {}.",
            get_size(gui::gpu_data::GPUPixelStorageFormat(opengl_type)),
            get_size(settings_.internal_format)
        );
    }

#endif

    util::image::ImageVariant out = util::image::make_image(settings_.type, settings_.read_format, width_, height_);

    const auto visitor = util::image::ImageVisitor(
        [this](auto&& image) {
            glGetTexImage(
                GL_TEXTURE_2D, 0, static_cast<GLenum>(this->settings_.read_format),
                static_cast<GLenum>(this->settings_.type), static_cast<void*>(image.get_raw_data())
            );
        }
    );

    std::visit(visitor, out);

    return out;

    }
}

} // namespace gpu_data

} // namespace gui
