#include "texture.hpp"

#include "util/color.hpp"

namespace gui {

namespace gpu_data {

texture2D_data
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

    return texture2D_data(std::move(data), width, height);
}

} // namespace gpu_data

} // namespace gui
