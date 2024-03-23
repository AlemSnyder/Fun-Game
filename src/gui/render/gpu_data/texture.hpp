
#pragma once

#include "logging.hpp"
#include "types.hpp"

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

    inline void
    bind(GLuint target) const {
        glActiveTexture(target);
        glBindTexture(GL_TEXTURE_1D, texture_ID_);
    }

    inline Texture1D(const std::vector<ColorFloat>& color_data) {
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
    }

    inline ~Texture1D() { glDeleteTextures(1, &texture_ID_); }

    [[nodiscard]] inline GLuint
    value() const {
        return texture_ID_;
    }
};

/*
class Texture2D {
 private:
    // four for the color (RGBA)
    // then four more for the reflection ambient, diffuse, metallic/specular, and glow
    static const size_t height = 2;
    GLuint texture_ID_;

 public:
    Texture2D(std::vector<vec8> color_data) {
        // create one texture and save the id to color_texture_
        glGenTextures(1, &texture_ID_);
        // bind to color_texture_
        glBindTexture(GL_TEXTURE_2D, texture_ID_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        // set texture warp. values outside of texture bounds are clamped to
        // edge of texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // no minimapping so this one don't matter
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // set magnification filter to linear. Interpolate between pixels in mat
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // load and generate the texture
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA32F, color_data.size(), height, 0, GL_RGBA,
GL_FLOAT, color_data.data()
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    ~Texture2D() { glDeleteTextures(1, &texture_ID_); }
};
*/
} // namespace gpu_data

} // namespace gui
