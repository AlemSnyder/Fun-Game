#pragma once

#include <filesystem>
#include <fstream>

namespace files {

/**
 * @brief Get the path to the root loading dir.
 */
std::filesystem::path
get_root_path();

/**
 * @brief Get the path to a data file
 */
inline std::filesystem::path
get_data_path() {
    return get_root_path() / "data";
}

/**
 * @brief Get the path to a data file
 */
inline std::ifstream
open_data_file(std::filesystem::path path) {
    return std::ifstream(get_data_path() / path);
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
inline std::ifstream
open_resource_file(std::filesystem::path path) {
    return std::ifstream(get_resources_path() / path);
}

} // namespace files
