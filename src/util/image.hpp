#pragma once

#include "types.hpp"
#include "util/color.hpp"

#include <png.h>

//#include <memory>
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

struct ImageSettings {
    size_t data_size;
    size_t width_bit_alignment; // 1, 2, 4, 8
    // anything else
    // image type
};

namespace {


template<typename T>
class ImageImplementation {
 protected:
    ImageSettings settings_;
    size_t width_;
    size_t height_;
    size_t data_width_; // width with padding
    size_t width_bit_alignment_; // 1, 2, 4, 8


    std::vector<T> data_;

 public:
    template<typename T>
    ImageImplementation(size_t width, size_t height, std::vector<T> data, size_t width_bit_alignment = 1)
        : width_(width), height_(height), width_bit_alignment_(width_bit_alignment), data_(data) {
            data_width_ = width_; // TODO
    }

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    template<typename T>
    inline T ge_data(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return data_.at(j * data_width_ + i);
    }

    template<std::same_as<png_byte> T>
    inline png_byte get_color(size_t i, size_t j) const {
        return ge_data(i, j);
    }

    template<int n, std::same_as<std::array<png_byte, n>> T>
    inline std::array<png_byte, n> get_color(size_t i, size_t j) const {
        auto color = ge_data(i, j);
        std::array<png_byte, n> out_color;
        for (size_t index = 0; index < n; index++) {
            out_color[index] = color[index];
        }
        return out_color;
    }

    template<std::same_as<float> T>
    inline png_byte get_color(size_t i, size_t j) const {
        return ge_data(i, j) * 256;
    }

    template<int n, std::same_as<std::array<float, n>> T>
    inline std::array<png_byte, n> get_color(size_t i, size_t j) const {
        auto color = get(i, j);
        std::array<png_byte, n> out_color;
        for (size_t index = 0; index < n; index++) {
            out_color[index] = color[index] * 256;
        }
        return out_color;
    }
};
}

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

// BYTE // 8 bit color channels 24 or 32 bit image
class ByteMonochromeImage : public virtual MonochromeImage {
 public:
    virtual png_byte get_color(size_t i, size_t j) const override;

    //    virtual png_byte get_data(size_t i, size_t j) const;

    ByteMonochromeImage(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) : Image(data, width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    ByteMonochromeImage(void* data, size_t width, size_t height, size_t data_size) :
        Image(data, width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    ByteMonochromeImage(size_t width, size_t height, size_t data_size) :
        Image(width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    void
    draw_at(const ByteMonochromeImage& other, size_t position_x, size_t position_y);

    void
    set_color(png_byte color, size_t i, size_t j) {
        reinterpret_cast<png_byte*>(data_.get())[i * height_ + j] = color;
    }

    inline void
    transpose() {
        auto temp = width_;
        width_ = height_;
        height_ = temp;
    }

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual ~ByteMonochromeImage() {}
};

class BytePolychromeImage : public virtual PolychromeImage {
 public:
    virtual std::array<png_byte, 3> get_color(size_t i, size_t j) const override;

    //    virtual std::array<png_byte, 3> get_data(size_t i, size_t j) const;
    // if needed make inline get color
    BytePolychromeImage(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) : Image(data, width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    BytePolychromeImage(void* data, size_t width, size_t height, size_t data_size) :
        Image(data, width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    BytePolychromeImage(size_t width, size_t height, size_t data_size) :
        Image(width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual ~BytePolychromeImage() {}
};

// TODO
// Everything commented out is a todo
class BytePolychromeAlphaImage : public virtual PolychromeAlphaImage {
 private:
    //    static BytePolychromeAlphaImage_data_t
    //    pad_color_data(const std::vector<std::vector<ColorByte>>& vector_data);

    //    BytePolychromeAlphaImage(BytePolychromeAlphaImage_data_t data) :
    //        Image(data.data, data.width, data.height, sizeof(ColorByte)) {}

 public:
    virtual std::array<png_byte, 4> get_color(size_t i, size_t j) const override;

    //    virtual std::array<png_byte, 4> get_data(size_t i, size_t j) const;

    BytePolychromeAlphaImage(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) : Image(data, width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    BytePolychromeAlphaImage(
        void* data, size_t width, size_t height, size_t data_size
    ) : Image(data, width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    BytePolychromeAlphaImage(size_t width, size_t height, size_t data_size) :
        Image(width, height, data_size) {
        assert(
            data_size == sizeof(unsigned char) && "data size must match expected size"
        );
    }

    //    BytePolychromeAlphaImage(std::vector<std::vector<ColorByte>> data) :
    //        BytePolychromeAlphaImage(pad_color_data(data)) {}

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual ~BytePolychromeAlphaImage() {}
};

} // namespace image

} // namespace util
