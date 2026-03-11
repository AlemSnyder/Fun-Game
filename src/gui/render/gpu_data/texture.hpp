
#pragma once

#include "data_types.hpp"
#include "types.hpp"
#include "util/image.hpp"
#include <expected>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

class Texture1D {
 private:
    GLuint texture_ID_;

 public:
    inline Texture1D() {
        // create one texture and save the id to color_texture_
        glGenTextures(1, &texture_ID_);
    }

    /**
     * @brief Bind texture to given texture index
     *
     * @param texture_index
     */
    inline void
    bind(GLuint texture_index) const {
        glActiveTexture(GL_TEXTURE0 + texture_index);
        glBindTexture(GL_TEXTURE_1D, texture_ID_);
    }

    /**
     * @brief Construct a new Texture1D from a vector of colors
     *
     * @param std::vector<ColorFloat>& color_data
     */
    Texture1D(const std::vector<ColorFloat>& color_data);

    inline ~Texture1D() { glDeleteTextures(1, &texture_ID_); }

    /**
     * @brief Get the texture id
     *
     * @return GLuint the texture id
     */
    [[nodiscard]] inline GLuint
    value() const {
        return texture_ID_;
    }
};

struct TextureSettings {
    uint8_t samples = 1;
    bool multisample = false;

    GPUPixelStorageFormat internal_format = GPUPixelStorageFormat::RGBA;
    GPUPixelReadFormat read_format = GPUPixelReadFormat::RGBA;
    GPUPixelType type = GPUPixelType::FLOAT;

    GLenum wrap_s = GL_CLAMP_TO_EDGE;
    GLenum wrap_t = GL_CLAMP_TO_EDGE;
    GLenum min_filter = GL_LINEAR;
    GLenum mag_filter = GL_LINEAR;

    GLenum compare_funct = GL_LEQUAL;
    GLenum compare_mode = GL_COMPARE_R_TO_TEXTURE;
};

class Texture2D : virtual public GPUDataRenderBuffer {
 private:
    // four for the color (RGBA)
    // then four more for the reflection ambient, diffuse, metallic/specular, and glow
    // static const size_t height = 2;
    screen_size_t width_;
    screen_size_t height_;
    const TextureSettings settings_;
    GLuint texture_ID_;

    void load_settings();

    void setup();

    void bind();

    template<class T>
    void load_data_(T image) {
        if (settings_.multisample) {
            LOG_ERROR(logging::opengl_logger, "Cannot write data to multisample texture");
            return;
        }
        if (settings_.internal_format == gpu_data::GPUPixelStorageFormat::DEPTH) {
            // Not sure what I wanted to put here
        }
        width_ = image.get_width();
        height_ = image.get_height();
        glTexImage2D(
            GL_TEXTURE_2D, 0, static_cast<GLenum>(settings_.internal_format),
            width_, height_, 0,
            static_cast<GLenum>(settings_.read_format), static_cast<GLenum>(settings_.type),
            image.get_raw_data()
        );
        if (settings_.type == GPUPixelType::FLOAT
            || settings_.type == GPUPixelType::HALF_FLOAT) {
            glGenerateMipmap(GL_TEXTURE_2D);
        }
    }

 public:
    Texture2D(
        screen_size_t width, screen_size_t height, TextureSettings settings = {},
        bool differed = true
    );

    Texture2D(
        util::image::ImageVariant Image, TextureSettings settings = {},
        bool differed = true
    );

    inline void load_data(util::image::MonochromeImage image) {
        return load_data_(image);
    }
    inline void load_data(util::image::PolychromeImage image) {
        return load_data_(image);
    }
    inline void load_data(util::image::PolychromeAlphaImage image) {
        return load_data_(image);
    }
    inline void load_data(util::image::FloatMonochromeImage image) {
        return load_data_(image);
    }
    inline void load_data(util::image::FloatPolychromeImage image) {
        return load_data_(image);
    }
    inline void load_data(util::image::FloatPolychromeAlphaImage image) {
        return load_data_(image);
    }
    
    void load_image(util::image::ImageVariant image);

    ~Texture2D() { glDeleteTextures(1, &texture_ID_); }

    /**
     * @brief Get the texture id
     *
     * @return GLuint the texture id
     */
    [[nodiscard]] inline GLuint
    value() const override {
        return texture_ID_;
    }

    virtual void
    connect_texture(GLuint framebuffer_ID, uint8_t texture_attachment) override;

    virtual void connect_depth_texture(GLuint framebuffer_ID) override;

    inline virtual GPUPixelType
    get_type() const override {
        return settings_.type;
    }

    inline virtual GPUPixelStorageFormat
    get_format() const override {
        return settings_.internal_format;
    }

    /**
     * @brief Bind texture to given texture index
     *
     * I think this is to read from.
     *
     * @param texture_index
     */
    inline void
    bind(GLuint texture_index) const {
        glActiveTexture(GL_TEXTURE0 + texture_index);
        GLenum target =
            settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glBindTexture(target, texture_ID_);
    }

    // instead of ints could return error enums
    [[nodiscard]] std::expected<util::image::ImageVariant, int>
    get_image() const;
};

} // namespace gpu_data

} // namespace gui
