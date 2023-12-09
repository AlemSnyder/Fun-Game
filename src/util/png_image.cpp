#include "png_image.hpp"

#include "../logging.hpp"

namespace image {

int
image_result_logger(write_result_t result, const std::filesystem::path& path) {
    switch (result) {
        case write_result_t::WR_OK:
            return 0;
        case write_result_t::WR_FOPEN_FAILED:
            LOG_ERROR(
                logging::file_io_logger, "Could not open file {}",
                path.lexically_normal().string()
            );
            return 1;
        case write_result_t::WR_CREATE_WRITE_STRUCT_FAILED:
            LOG_ERROR(logging::file_io_logger, "Create write strut failed.");
            return 1;
        case write_result_t::WR_CREATE_INFO_STRUCT_FAILED:
            LOG_ERROR(logging::file_io_logger, "Create info struct failed.");
            return 1;
        case write_result_t::WR_SETJMP_PNG_JMPBUF_FAILED:
            LOG_ERROR(logging::file_io_logger, "Set jump buffer failed.");
            return 1;
        case write_result_t::WR_ROW_MALLOC_FAILED:
            LOG_ERROR(logging::file_io_logger, "Row Malloc Failed.");
            return 1;

        default:
            LOG_ERROR(logging::file_io_logger, "UNKNOWN PNG WRITE STATUS.");
            return 1;
    }
}

} // namespace image
