#include "files.hpp"

#include "config.h"

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
            std::filesystem::absolute(std::filesystem::path(dir_path) / ROOT_DIR_PATH);
    }

    return root_path;
}

} // namespace files
