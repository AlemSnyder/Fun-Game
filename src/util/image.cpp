#include "image.hpp"

namespace util {
namespace image {
    
ImageVariant
make_image(gui::gpu_data::GPUPixelType type, gui::gpu_data::GPUPixelReadFormat format, size_t width, size_t height, size_t width_bit_alignment = 1){
    switch (type) {
        case gui::gpu_data::GPUPixelType::FLOAT:
        case gui::gpu_data::GPUPixelType::HALF_FLOAT:
            switch (format) {
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_COMPONENT:
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_STENCIL:
                case gui::gpu_data::GPUPixelReadFormat::RED:
                case gui::gpu_data::GPUPixelReadFormat::GREEN:
                case gui::gpu_data::GPUPixelReadFormat::BLUE:
                    return MonochromeImage(width, height, width_bit_alignment);
                case gui::gpu_data::GPUPixelReadFormat::RGB:
                case gui::gpu_data::GPUPixelReadFormat::BGR:
                    return MonochromeImage(width, height, width_bit_alignment);

                case gui::gpu_data::GPUPixelReadFormat::RGBA:
                case gui::gpu_data::GPUPixelReadFormat::BGRA:
                    return MonochromeImage(width, height, width_bit_alignment);
            }
        case gui::gpu_data::GPUPixelType::UNSIGNED_BYTE:
            switch (format) {
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_COMPONENT:
                case gui::gpu_data::GPUPixelReadFormat::DEPTH_STENCIL:
                case gui::gpu_data::GPUPixelReadFormat::RED:
                case gui::gpu_data::GPUPixelReadFormat::GREEN:
                case gui::gpu_data::GPUPixelReadFormat::BLUE:
                    return MonochromeImage(
                        width, height, width_bit_alignment
                    );
                case gui::gpu_data::GPUPixelReadFormat::RGB:
                case gui::gpu_data::GPUPixelReadFormat::BGR:
                    return PolychromeImage(
                         width, height, width_bit_alignment
                    );

                case gui::gpu_data::GPUPixelReadFormat::RGBA:
                case gui::gpu_data::GPUPixelReadFormat::BGRA:
                    return PolychromeAlphaImage(
                        width, height, width_bit_alignment
                    );
            }

}
    // LOR error
    abort();
}

}

}

