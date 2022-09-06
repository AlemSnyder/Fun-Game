#include "files.hpp"

#include "config.h"

#include <whereami.h>

#include <filesystem>
#include <iostream>

namespace files {

std::filesystem::path
get_root_path() {
    int length = wai_getExecutablePath(NULL, 0, NULL);
    std::string dir_path = std::string(length + 1, '\0');

    int dirname_length;
    wai_getExecutablePath(dir_path.data(), length, &dirname_length);
    dir_path.resize(dirname_length);  // remove the executable name

    std::filesystem::path root_path = std::filesystem::path(dir_path) / ROOT_DIR_PATH;
    return std::filesystem::absolute(root_path);
}

} // namespace files
