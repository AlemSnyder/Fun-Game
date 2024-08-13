#pragma once

#include <filesystem>
#include <fstream>
#include <optional>

namespace files {

/**
 * @brief Get the path to the root loading dir.
 */
std::filesystem::path get_root_path();

/**
 * @brief Get the path to a data file
 */
inline std::filesystem::path
get_data_path() {
    return get_root_path() / "data";
}

/**
 * @brief Open a given path
 */
std::optional<std::ifstream> open_file(std::filesystem::path path);

/**
 * @brief Get the path to a data file
 */
inline std::optional<std::ifstream>
open_data_file(std::filesystem::path path) {
    return open_file(get_data_path() / path);
}

/**
 * @brief Get the path to a resource file
 */
inline std::filesystem::path
get_resources_path() {
    return get_root_path() / "resources";
}

/**
 * @brief Open a resource file
 */
inline std::optional<std::ifstream>
open_resource_file(std::filesystem::path path) {
    return open_file(get_resources_path() / path);
}

inline std::filesystem::path
get_argument_path(std::filesystem::path path) {
    if (path.is_absolute()){
        return path;
    }
    else {
        return std::filesystem::current_path() / path;
    }
}

} // namespace files
