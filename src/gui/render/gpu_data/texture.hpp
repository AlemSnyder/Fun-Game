
#pragma once

#include "global_context.hpp"
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

class Texture2D {
 private:
    // four for the color (RGBA)
    // then four more for the reflection ambient, diffuse, metallic/specular, and glow
    // static const size_t height = 2;
    GLuint texture_ID_;

    [[nodiscard]] texture2D_data_t static pad_color_data(
        const std::vector<std::vector<ColorFloat>>& vector_data
    );

    Texture2D(const texture2D_data_t& color_data);

 public:
    /**
     * @brief Construct a new Texture2D from a vector of vectors of colors
     *
     * @param const std::vector<std::vector<ColorFloat>>& color_data
     */
    inline Texture2D(const std::vector<std::vector<ColorFloat>>& color_data) :
        Texture2D(pad_color_data(color_data)) {}

    Texture2D(uint width, uint height);

    ~Texture2D() { glDeleteTextures(1, &texture_ID_); }

    /**
     * @brief Get the texture id
     *
     * @return GLuint the texture id
     */
    [[nodiscard]] inline GLuint
    value() const {
        return texture_ID_;
    }

    /**
     * @brief Bind texture to given texture index
     *
     * @param texture_index
     */
    inline void
    bind(GLuint texture_index) const {
        glActiveTexture(GL_TEXTURE0 + texture_index);
        glBindTexture(GL_TEXTURE_2D, texture_ID_);
    }
};

} // namespace gpu_data

} // namespace gui
