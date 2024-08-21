#include "texture.hpp"

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

Texture2D::Texture2D(const texture2D_data_t& color_data) {
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this, color_data]() {
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
            GL_TEXTURE_2D, 0, GL_RGBA32F, color_data.width, color_data.height, 0,
            GL_RGBA, GL_FLOAT, color_data.data.data()
        );
        glGenerateMipmap(GL_TEXTURE_2D);
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
