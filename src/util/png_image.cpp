#include "png_image.hpp"

#include "../logging.hpp"

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
            LOG_ERROR(logging::file_io_logger, "UNKNOWN PNG WRITE STATUS {}", static_cast<int>(result));
            return;
    }
}

} // namespace image
