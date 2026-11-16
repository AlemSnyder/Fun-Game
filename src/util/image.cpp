#include "image.hpp"

namespace util {
namespace image {

ImageVariant
make_image(
    gui::gpu_data::GPUPixelType type, gui::gpu_data::GPUPixelReadFormat format,
    size_t width, size_t height
) {
    switch (type) {
        //        case gui::gpu_data::GPUPixelType::HALF_FLOAT: log error
        case gui::gpu_data::GPUPixelType::FLOAT:
            switch (format) {
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_COMPONENT:
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_STENCIL:
                case gui::gpu_data::GPUPixelReadFormat::RED:
                case gui::gpu_data::GPUPixelReadFormat::GREEN:
                case gui::gpu_data::GPUPixelReadFormat::BLUE:
                    return FloatMonochromeImage(width, height);
                case gui::gpu_data::GPUPixelReadFormat::RGB:
                case gui::gpu_data::GPUPixelReadFormat::BGR:
                    return FloatPolychromeImage(width, height);

                case gui::gpu_data::GPUPixelReadFormat::RGBA:
                case gui::gpu_data::GPUPixelReadFormat::BGRA:
                    return FloatPolychromeAlphaImage(width, height);
            }
        case gui::gpu_data::GPUPixelType::UNSIGNED_BYTE:
            switch (format) {
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_COMPONENT:
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_STENCIL:
                case gui::gpu_data::GPUPixelReadFormat::RED:
                case gui::gpu_data::GPUPixelReadFormat::GREEN:
                case gui::gpu_data::GPUPixelReadFormat::BLUE:
                    return MonochromeImage(width, height);
                case gui::gpu_data::GPUPixelReadFormat::RGB:
                case gui::gpu_data::GPUPixelReadFormat::BGR:
                    return PolychromeImage(width, height);

                case gui::gpu_data::GPUPixelReadFormat::RGBA:
                case gui::gpu_data::GPUPixelReadFormat::BGRA:
                    return PolychromeAlphaImage(width, height);
            }
    }
    // LOR error
    abort();
}

} // namespace image

} // namespace util
