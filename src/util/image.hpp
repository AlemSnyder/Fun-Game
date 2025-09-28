#pragma once

#include "types.hpp"
#include "util/color.hpp"

#include <png.h>

#include <memory>

namespace util {

namespace image {

struct FloatPolychromeAlphaImage_data_t {
    std::shared_ptr<char[]> data;
    screen_size_t width;
    screen_size_t height;
};

template <class T, int datum_number>
std::array<png_byte, datum_number>
read_data(std::shared_ptr<char[]> data, size_t offset) {
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

class Image {
    // some data
 protected:
    size_t width_;
    size_t height_;
    size_t data_size_;
    std::shared_ptr<char[]> data_;

 public:
    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual void*
    data() const {
        return data_.get();
    }

    inline Image(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) :
        width_(width),
        height_(height), data_size_(data_size), data_(data){};

    virtual ~Image() {}
};

class MonochromeImage : public virtual Image {
 public:
    virtual png_byte get_color(size_t i, size_t j) const = 0;

    inline virtual ~MonochromeImage() {}
};

class PolychromeImage : public virtual Image {
 public:
    virtual std::array<png_byte, 3> get_color(size_t i, size_t j) const = 0;

    inline virtual ~PolychromeImage() {}
};

class PolychromeAlphaImage : public virtual Image {
 public:
    virtual std::array<png_byte, 4> get_color(size_t i, size_t j) const = 0;

    inline virtual ~PolychromeAlphaImage() {}
};

// FLOAT
class FloatMonochromeImage : public virtual MonochromeImage {
 public:
    virtual png_byte get_color(size_t i, size_t j) const override;

    virtual float get_data(size_t i, size_t j) const;

    FloatMonochromeImage(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) :
        Image(data, width, height, data_size) {}

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual ~FloatMonochromeImage() {}
};

class FloatPolychromeImage : public virtual PolychromeImage {
 public:
    virtual std::array<png_byte, 3> get_color(size_t i, size_t j) const override;

    virtual std::array<float, 3> get_data(size_t i, size_t j) const;

    FloatPolychromeImage(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) :
        Image(data, width, height, data_size) {}

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual ~FloatPolychromeImage() {}
};

class FloatPolychromeAlphaImage : public virtual PolychromeAlphaImage {
 private:
    static FloatPolychromeAlphaImage_data_t
    pad_color_data(const std::vector<std::vector<ColorFloat>>& vector_data);

    FloatPolychromeAlphaImage(FloatPolychromeAlphaImage_data_t data) :
        Image(data.data, data.width, data.height, sizeof(ColorFloat)) {}

 public:
    virtual std::array<png_byte, 4> get_color(size_t i, size_t j) const override;

    virtual std::array<float, 4> get_data(size_t i, size_t j) const;

    FloatPolychromeAlphaImage(
        std::shared_ptr<char[]> data, size_t width, size_t height, size_t data_size
    ) :
        Image(data, width, height, data_size) {}

    FloatPolychromeAlphaImage(std::vector<std::vector<ColorFloat>> data) :
        FloatPolychromeAlphaImage(pad_color_data(data)) {}

    inline virtual size_t
    get_width() const {
        return width_;
    }

    inline virtual size_t
    get_height() const {
        return height_;
    }

    inline virtual ~FloatPolychromeAlphaImage() {}
};

#if __HAVE_FLOAT16
// HALF FLOAT
class HALFFloatMonochromeImage : public virtual MonochromeImage {
 public:
    virtual png_byte get_color(size_t i, size_t j) const;
};

class HALFFloatPolychromeImage : public virtual PolychromeImage {
 public:
    virtual std::array<png_byte, 3> get_color(size_t i, size_t j) const;
};

class HALFFloatPolychromeAlphaImage : public virtual PolychromeAlphaImage {
 public:
    virtual std::array<png_byte, 4> get_color(size_t i, size_t j) const;
};
#endif

} // namespace image

} // namespace util
