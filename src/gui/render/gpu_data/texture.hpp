
#pragma once

#include "data_types.hpp"
#include "global_context.hpp"
#include "logging.hpp"
#include "types.hpp"
#include "util/color.hpp"

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

struct texture2D_data_t {
    std::vector<ColorFloat> data;
    uint width;
    uint height;
};

struct TextureSettings {
    uint8_t samples = 1;
    bool multisample = false;
    
    GLenum internalformat = GL_RGBA32F;
    GLenum format = GL_RGBA;
    GLenum type = GL_FLOAT;

    GLenum wrap_s = GL_CLAMP_TO_EDGE;
    GLenum wrap_t = GL_CLAMP_TO_EDGE;
    GLenum min_filter = GL_LINEAR_MIPMAP_LINEAR;
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
    TextureSettings settings_;
    GLuint texture_ID_;

    [[nodiscard]] texture2D_data_t static pad_color_data(
        const std::vector<std::vector<ColorFloat>>& vector_data
    );

    Texture2D();

    Texture2D(const texture2D_data_t& color_data, TextureSettings settings = {});

    
    public:
    /**
     * @brief Construct a new Texture2D from a vector of vectors of colors
     *
     * @param const std::vector<std::vector<ColorFloat>>& color_data
     */
    inline Texture2D(const std::vector<std::vector<ColorFloat>>& color_data, TextureSettings settings = {}) :
    Texture2D(pad_color_data(color_data), settings) {}
    
    Texture2D(screen_size_t width, screen_size_t height, TextureSettings settings = {});
    
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
        GLenum target = settings_.multisample ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        glBindTexture(target, texture_ID_);
    }
};

} // namespace gpu_data

} // namespace gui
