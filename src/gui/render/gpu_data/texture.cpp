#include "texture.hpp"
#include "gui/handler.hpp"

#include "util/color.hpp"

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
    FrameBufferHandler& handler = FrameBufferHandler::instance();
    handler.bind_fbo(framebuffer_ID);
    GLenum target = settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + texture_attachment, target, texture_ID_, 0);
}

void
Texture2D::connect_depth_texture(GLuint framebuffer_ID) {
    FrameBufferHandler& handler = FrameBufferHandler::instance();
    handler.bind_fbo(framebuffer_ID);
    GLenum target = settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture_ID_, 0);
}



Texture2D::Texture2D(int width, int height, TextureSettings settings) : width_(width), height_(height), settings_(settings) {

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



    if (settings_.multisample) {
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, settings_.samples, settings_.internalformat, width, height,
            GL_TRUE
        );
    } else {
        glTexImage2D(
            GL_TEXTURE_2D, 0, settings_.internalformat, width, height, 0,
            settings_.format, settings_.type, nullptr
        );
    }
}

Texture2D::Texture2D(const texture2D_data_t& color_data, TextureSettings settings) {
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this, color_data]() {

        width_ = color_data.width;
        height_ = color_data.height;
    
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



    if (settings_.multisample) {
        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, settings_.samples, settings_.internalformat, width_, height_,
            GL_TRUE
        );
    } else {
        glTexImage2D(
            GL_TEXTURE_2D, 0, settings_.internalformat, width_, height_, 0,
            settings_.format, settings_.type, nullptr
        );
    }

        if (settings_.multisample) {
            LOG_ERROR(logging::opengl_logger, "Cannot write data to multisample texture");
        } else {
            glBindTexture(GL_TEXTURE_2D, texture_ID_);
            glTexImage2D(
                GL_TEXTURE_2D, 0, settings_.internalformat, width_,
                height_, 0, settings_.format, settings_.type, color_data.data.data()
            );
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    });
}

texture2D_data_t
Texture2D::pad_color_data(const std::vector<std::vector<ColorFloat>>& vector_data) {
    uint width = 0;
    uint height = vector_data.size();

    for (const auto& row : vector_data) {
        if (row.size() > width) {
            width = row.size();
        }
    }

    std::vector<ColorFloat> data;

    for (const auto& row : vector_data) {
        for (const auto& color : row) {
            data.push_back(color);
        }
        for (size_t i = 0; i < width - row.size(); i++) {
            data.push_back(color::black);
        }
    }

    return texture2D_data_t(std::move(data), width, height);
}

} // namespace gpu_data

} // namespace gui
