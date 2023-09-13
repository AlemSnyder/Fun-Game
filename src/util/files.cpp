#include "files.hpp"

#include "config.h"
#include "../logging.hpp"

#include <whereami.h>

#include <filesystem>
#include <iostream>

namespace files {

std::filesystem::path
get_root_path() {
    static std::filesystem::path root_path;

    if (root_path.empty()) {
        // Not cached
        int length = wai_getExecutablePath(nullptr, 0, nullptr);
        std::string dir_path = std::string(length + 1, '\0');

        int dirname_length;
        wai_getExecutablePath(dir_path.data(), length, &dirname_length);
        dir_path.resize(dirname_length); // remove the executable name

        root_path =
            (std::filesystem::path(dir_path) / ROOT_DIR_PATH).lexically_normal();
    }

    return root_path;
}

std::optional<std::ifstream>
open_file(std::filesystem::path path) {
    if (!std::filesystem::exists(path)) {
        LOG_ERROR(logging::file_io_logger, "File, {}, not found", path.string());
        return {};
    }
    return std::ifstream(path);
}

} // namespace files
