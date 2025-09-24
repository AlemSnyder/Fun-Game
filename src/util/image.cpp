#include "image.hpp"

namespace util {

namespace image{

    png_byte FloatMonochromeImage::get_color(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data<float, 1>(data_, i * height_ + j)[0];
    }
    
    float FloatMonochromeImage::get_data(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data_float<float, 1>(data_, i * height_ + j)[0];
    }
    
    
    std::array<png_byte, 3> FloatPolychromeImage::get_color(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data<float, 3>(data_, i * height_ + j);
    }

    std::array<float, 3> FloatPolychromeImage::get_data(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data_float<float, 3>(data_, i * height_ + j);
    }
    
    
    std::array<png_byte, 4> FloatPolychromeAlphaImage::get_color(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data<float, 4>(data_, i * height_ + j);
    }

    std::array<float, 4> FloatPolychromeAlphaImage::get_data(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data_float<float, 4>(data_, i * height_ + j);
    }
    
    # if __HAVE_FLOAT16
    // HALF FLOAT
    png_byte HALFFloatMonochromeImage::get_color(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data<_Float16, 1>(data_, i * height_ + j)[0];
    }
    
    std::array<png_byte, 3> HALFFloatPolychromeImage::get_color(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data<_Float16, 3>(data_, i * height_ + j);
    }
    
    
    std::array<png_byte, 4> HALFFloatPolychromeAlphaImage::get_color(size_t i, size_t j) const {
        assert(i < width_ && j < height_ && "Position must be within image.");
        return read_data<_Float16, 4>(data_, i * height_ + j);
    }
    #endif

}

}