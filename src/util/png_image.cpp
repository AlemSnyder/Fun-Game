#include "png_image.hpp"

#include "../exceptions.hpp"
#include "../logging.hpp"
#include "image.hpp"

#include <png.h>

#include <expected>

namespace image {

void
log_result(write_result_t result, const std::filesystem::path& path) {
    switch (result) {
        case write_result_t::WR_OK:
            return;
        case write_result_t::WR_FOPEN_FAILED:
            LOG_ERROR(
                logging::file_io_logger, "Could not open file {}",
                path.lexically_normal().string()
            );
            return;
        case write_result_t::WR_CREATE_WRITE_STRUCT_FAILED:
            LOG_ERROR(logging::file_io_logger, "Create write strut failed.");
            return;
        case write_result_t::WR_CREATE_INFO_STRUCT_FAILED:
            LOG_ERROR(logging::file_io_logger, "Create info struct failed.");
            return;
        case write_result_t::WR_SETJMP_PNG_JMPBUF_FAILED:
            LOG_ERROR(logging::file_io_logger, "Set jump buffer failed.");
            return;
        case write_result_t::WR_ROW_MALLOC_FAILED:
            LOG_ERROR(logging::file_io_logger, "Row Malloc Failed.");
            return;

        default:
            LOG_ERROR(
                logging::file_io_logger, "UNKNOWN PNG WRITE STATUS {}",
                static_cast<int>(result)
            );
            return;
    }
}

#if DEBUG()
// Literally just testing if this builds
void
test_function() {
    [[maybe_unused]] auto result = write_image(ColorImageTest({}), "path");
}

#endif

class PNG_read_info {
    public:
    png_structp png_ptr = nullptr;
    png_infop png_info = nullptr;

    PNG_read_info() {
        // Create our write struct
        // TODO these nullptr should be function pointers
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png_ptr) {return ;}
        // Create our info struct for this png image
        png_info = png_create_info_struct(png_ptr);
    }

    ~PNG_read_info() {
        auto temp_ptr_struct = (png_ptr) ? &png_ptr : nullptr;
        auto temp_png_info = (png_info) ? &png_info : nullptr;
        png_destroy_read_struct(temp_ptr_struct, temp_png_info, nullptr);
    }
};

std::expected<util::image::ImageVariant, int>
read_image(std::filesystem::path path) {
    path = std::filesystem::absolute(path);
    std::string path_string = path.string();
    LOG_BACKTRACE(logging::file_io_logger, "Reading image from {}.", path_string);

    // Read the tiles from the path specified, and save
    if (!std::filesystem::exists(path)) {
        LOG_ERROR(
            logging::file_io_logger,
            "Could not open {}. Are you in the right directory?", path_string
        );
        return std::unexpected(1);
    }

    std::unique_ptr<std::FILE, void(*)(std::FILE*)> file (
        fopen(path_string.c_str(), "rb"), [](std::FILE* file){
            fclose(file);
        }
    );
    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    int color_type;
    int interlace_method;
    int compression_method;
    int filter_method;

    unsigned char signal[8];

    fread(signal, 1, 8, file.get());

    if (!png_check_sig(signal, 8)) {
        LOG_ERROR(logging::file_io_logger, "Failed due to: Bad Signal");
        return std::unexpected(2);
    }

    auto struct_ptr = std::unique_ptr<png_struct, void(*)(png_struct*)>(
        png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr),
        [](png_struct* ptr){
            png_destroy_read_struct(&ptr, nullptr, nullptr);
        }
    );
    PNG_read_info info;
    if (!info.png_ptr) {
        LOG_ERROR(logging::file_io_logger, "Failed due to: Out of Memory");
        return std::unexpected(4);
    }
    if (!info.png_info) {
        LOG_ERROR(logging::file_io_logger, "Failed due to: Out of Memory");
        return std::unexpected(4);
    }

    png_init_io(info.png_ptr, file.get());
    png_set_sig_bytes(info.png_ptr, 8);
    png_read_info(info.png_ptr, info.png_info);

    png_get_IHDR(
        info.png_ptr, info.png_info, &width, &height, &bit_depth, &color_type, &interlace_method,
        &compression_method, &filter_method
    );

    if (color_type != PNG_COLOR_TYPE_RGB_ALPHA) {
        LOG_WARNING(logging::file_io_logger, "Color type not RGBA");
    }

    /* I don't really care about a background color, but if I did
    if (!png_get_valid(info.png_ptr, info.png_info, PNG_INFO_bKGD)) {
        LOG_ERROR(logging::file_io_logger, "Failed due to: Out of Memory");
        return std::unexpected(1);
    }*/

    std::shared_ptr<png_bytep[]> row_pointers(new png_bytep[height]);

    png_uint_32 row_bytes = png_get_rowbytes(info.png_ptr, info.png_info);
    std::vector<std::array<png_byte, 4>> data (width * height);

    for (unsigned int i = 0; i < height; i++) {
        row_pointers[i] = reinterpret_cast<png_bytep>(data.data()) + i * row_bytes;
    }

    png_read_image(info.png_ptr, row_pointers.get());

    png_read_end(info.png_ptr, nullptr);

    // todo do things for bit depth
    auto image =
        util::image::PolychromeAlphaImage(width, height, data, 1);

#if DEBUG()
    for (unsigned int i = 0; i < image.get_width(); i++) {
        for (unsigned int j = 0; j < image.get_height(); j++) {
            auto raw_image_color0 = row_pointers[i][j * 4];
            auto raw_image_color1 = row_pointers[i][j * 4 + 1];
            auto raw_image_color2 = row_pointers[i][j * 4 + 2];
            auto raw_image_color3 = row_pointers[i][j * 4 + 3];

            auto data_color0 =
                reinterpret_cast<png_bytep>(data.data())[i * row_bytes + j * 4];
            auto data_color1 =
                reinterpret_cast<png_bytep>(data.data())[i * row_bytes + j * 4 + 1];
            auto data_color2 =
                reinterpret_cast<png_bytep>(data.data())[i * row_bytes + j * 4 + 2];
            auto data_color3 =
                reinterpret_cast<png_bytep>(data.data())[i * row_bytes + j * 4 + 3];
            if (raw_image_color0 != data_color0 || raw_image_color1 != data_color1
                || raw_image_color2 != data_color2 || raw_image_color3 != data_color3) {
                LOG_WARNING(logging::file_io_logger, "Pixel incorrect at {}{}", i, j);
                LOG_WARNING(
                    logging::file_io_logger, "[{},{},{},{}]", data_color0, data_color1,
                    data_color2, data_color3
                );
                LOG_WARNING(
                    logging::file_io_logger, "[{},{},{},{}]", raw_image_color0,
                    raw_image_color1, raw_image_color2, raw_image_color3
                );
            }

            auto color = image.get_color(i, j);

            if (raw_image_color0 != color[0] || raw_image_color1 != color[1]
                || raw_image_color2 != color[2] || raw_image_color3 != color[3]) {
                LOG_WARNING(logging::file_io_logger, "Pixel incorrect at {}{}", i, j);
                LOG_WARNING(
                    logging::file_io_logger, "[{},{},{},{}]", color[0], color[1],
                    color[2], color[3]
                );
                LOG_WARNING(
                    logging::file_io_logger, "[{},{},{},{}]", raw_image_color0,
                    raw_image_color1, raw_image_color2, raw_image_color3
                );
            }
        }
    }
#endif

    return image;
}

} // namespace image
