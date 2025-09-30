#include "image.hpp"

#include "util/color.hpp"

namespace util {

namespace image {

png_byte
FloatMonochromeImage::get_color(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data<float, 1>(data_, i * height_ + j)[0];
}

float
FloatMonochromeImage::get_data(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data_float<float, 1>(data_, i * height_ + j)[0];
}

std::array<png_byte, 3>
FloatPolychromeImage::get_color(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data<float, 3>(data_, i * height_ + j);
}

std::array<float, 3>
FloatPolychromeImage::get_data(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data_float<float, 3>(data_, i * height_ + j);
}

FloatPolychromeAlphaImage_data_t
FloatPolychromeAlphaImage::pad_color_data(
    const std::vector<std::vector<ColorFloat>>& vector_data
) {
    uint width = 0;
    uint height = vector_data.size();

    for (const auto& row : vector_data) {
        if (row.size() > width) {
            width = row.size();
        }
    }

    std::shared_ptr<char[]> data(
        new char[width * height * sizeof(ColorFloat) / sizeof(char)]
    );

    size_t j = 0;
    ColorFloat* float_color_data = reinterpret_cast<ColorFloat*>(data.get());

    for (const auto& row : vector_data) {
        for (const auto& color_f : row) {
            // data.push_back(color);
            float_color_data[j] = color_f;
            j++;
        }
        for (size_t i = 0; i < width - row.size(); i++) {
            float_color_data[j] = color::black;
            j++;
        }
    }

    return FloatPolychromeAlphaImage_data_t(data, width, height);
}

std::array<png_byte, 4>
FloatPolychromeAlphaImage::get_color(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data<float, 4>(data_, i * height_ + j);
}

std::array<float, 4>
FloatPolychromeAlphaImage::get_data(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data_float<float, 4>(data_, i * height_ + j);
}

#if __HAVE_FLOAT16
// HALF FLOAT
png_byte
HALFFloatMonochromeImage::get_color(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data<_Float16, 1>(data_, i * height_ + j)[0];
}

std::array<png_byte, 3>
HALFFloatPolychromeImage::get_color(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data<_Float16, 3>(data_, i * height_ + j);
}

std::array<png_byte, 4>
HALFFloatPolychromeAlphaImage::get_color(size_t i, size_t j) const {
    assert(i < width_ && j < height_ && "Position must be within image.");
    return read_data<_Float16, 4>(data_, i * height_ + j);
}
#endif

} // namespace image

} // namespace util