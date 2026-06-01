#pragma once

#include "types.hpp"
#include "util/color.hpp"
#include "gui/render/gl_enums.hpp"

#include <png.h>

#include <vector>
#include <variant>

namespace util {

namespace image {

/*
Image
v
different types, byte float [0, 1)

get_width, get_height, get_pixel(), and finally a settings struct

byte and float have get_data() which returns a byte of float


*/

//namespace {

template<size_t n>
[[maybe_unused]] inline std::array<png_byte, n> 
convert_to_color(std::array<float, n>& data) {
    std::array<png_byte, n> out_color;
    for (size_t index = 0; index < n; index++) {
        out_color[index] = data[index] * 256;
    }
    return out_color;
}

[[maybe_unused]] inline png_byte 
convert_to_color(float data) {
    return data * 255;
}

template<size_t n>
[[maybe_unused]] inline std::array<png_byte, n> 
convert_to_color(const std::array<png_byte, n>& data) {
        return data;
}

template<size_t n>
[[maybe_unused]] std::array<png_byte, n> 
convert_to_color(std::array<png_byte, n>&& data) {
        return data;
}

[[maybe_unused]] inline png_byte 
convert_to_color(png_byte data) {
    return data;
}

template<typename T, typename D>
inline
std::vector<T> convert(const std::vector<D>& data) {
    static_assert(sizeof(T) == sizeof(D), "must be same size");
    std::vector<T> out(data.size());
    // I give up
    memcpy(out.data(), data.data(), data.size() * sizeof(D));
    return out;
}

template<typename T>
struct intermediate {
    size_t width;
    size_t height;
//    size_t data_width;
    size_t width_bit_alignment;

    std::vector<T> data;
};

template<typename T>
intermediate<T>
inline
make_intermediate(const std::vector<std::vector<T>>& data, size_t width_bit_alignment) {

    std::vector<T> data_out;

    size_t width = 0;
    size_t height = data.size();
    
    for (const auto& row : data) {
    if (row.size() > width) {
        width = row.size();
    }
    }
    
    data_out.resize(width * height);
    for (size_t i = 0; i < height; i++) {
    data_out.insert(data_out.end(), data[i].begin(), data[i].end());
    data_out.insert(data_out.end(), width - data[i].size(), T());
    }

    return intermediate(width, height, width_bit_alignment, data_out);
}

template<typename T>
class ImageImplementation {
 protected:
    size_t width_;
    size_t height_;
    size_t data_width_; // width with padding
    size_t width_bit_alignment_; // 1, 2, 4, 8


    std::vector<T> data_;

    // private intermediate
    template<typename D>
    inline
    ImageImplementation(intermediate<D> inter) : ImageImplementation(inter.width, inter.height, inter.data, inter.width_bit_alignment) {}

 public:
    template<typename D>
    inline
    ImageImplementation(size_t width, size_t height, const std::vector<D>& data, size_t width_bit_alignment = 1)
        : width_(width), height_(height), width_bit_alignment_(width_bit_alignment), data_(convert<T,D>(data)) {
            data_width_ = width_; // TODO
    }
    template<typename D>
    inline
    ImageImplementation(const std::vector<std::vector<D>>& data, size_t width_bit_alignment = 1)
        : ImageImplementation(make_intermediate(data, width_bit_alignment)) { }

    ImageImplementation(size_t width, size_t height, size_t width_bit_alignment = 1)
        : ImageImplementation<T>(width, height, std::vector<T>(), width_bit_alignment) {}

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

//    template<typename T>
    inline const T* get_raw_data() const {
        return data_.data();
    }

//    template<typename T>
    inline T* get_raw_data() {
        return data_.data();
    }

//    template<typename T>
    inline T get_data(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return data_.at(j * data_width_ + i);
    }

    inline auto get_color(size_t i, size_t j) const {
        return convert_to_color(get_data(i, j));
    }

    // TODO
    inline void draw_at(ImageImplementation<T> other, size_t x, size_t y) {
        return;
    }

    // not sure what this does
    inline void transpose() {
        return;
    }
};

//}

struct FloatPolychromeAlphaImage_data_t {
    std::shared_ptr<char[]> data;
    screen_size_t width;
    screen_size_t height;
};

// t represents the underlying type of the data structure;
template <class T, int datum_number>
std::array<png_byte, datum_number>
read_data(std::shared_ptr<char[]> data, size_t offset) {
    // assert(sizeof(T) == data_size_)
    size_t bit_offset = offset * sizeof(T) * datum_number;
    char* data_ptr = &data[bit_offset];
    T* pixel_data = reinterpret_cast<T*>(data_ptr);
    std::array<png_byte, datum_number> out;
    for (size_t i = 0; i < datum_number; i++) {
        // normalize data to size of png_byte
        out[i] = pixel_data[i] * (2 >> sizeof(png_byte));
    }
    return out;
}

template <class T, int datum_number>
std::array<float, datum_number>
read_data_float(std::shared_ptr<char[]> data, size_t offset) {
    size_t bit_offset = offset * sizeof(T) * datum_number;
    char* data_ptr = &data[bit_offset];
    T* pixel_data = reinterpret_cast<T*>(data_ptr);
    std::array<float, datum_number> out;
    for (size_t i = 0; i < datum_number; i++) {
        out[i] = pixel_data[i];
    }
    return out;
}

using MonochromeImage = ImageImplementation<png_byte>;
using PolychromeImage = ImageImplementation<std::array<png_byte, 3>>;
using PolychromeAlphaImage = ImageImplementation<std::array<png_byte, 4>>;

using FloatMonochromeImage = ImageImplementation<float>;
using FloatPolychromeImage = ImageImplementation<std::array<float, 3>>;
using FloatPolychromeAlphaImage = ImageImplementation<std::array<float, 4>>;

using ImageVariant = std::variant<MonochromeImage, PolychromeImage, PolychromeAlphaImage, FloatMonochromeImage, FloatPolychromeImage, FloatPolychromeAlphaImage>;

#if 0
// HALF FLOAT

using HalfFloatMonochromeImage = ImageImplementation<halffloat>;
using HalfFloatPolychromeImage = ImageImplementation<std::array<halffloat, 3>>;
using HalfFloatPolychromeAlphaImage = ImageImplementation<std::array<halffloat, 4>>;

#endif

template<class... Ts>
struct ImageVisitor : Ts... { using Ts::operator()...;};

ImageVariant
make_image(gui::gpu_data::GPUPixelType type, gui::gpu_data::GPUPixelReadFormat format, size_t width, size_t height, size_t width_bit_alignment = 1);

template<typename T>
ImageVariant
make_image(size_t width, size_t height, size_t width_bit_alignment = 1) {
    return ImageImplementation<T>(width, height, {}, width_bit_alignment);
}

} // namespace image

} // namespace util
