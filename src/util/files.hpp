// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file files.hpp
 *
 * @brief Defines filesystem interactions.
 *
 * @ingroup Util
 *
 */

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
 * @brief Get the path to a resource file
 */
inline std::filesystem::path
get_manifest_path() {
    return get_data_path() / "manifest";
}

/**
 * @brief Open a resource file
 */
inline std::optional<std::ifstream>
open_resource_file(std::filesystem::path path) {
    return open_file(get_resources_path() / path);
}

/**
 * @brief Get full path of path passed as command line argument.
 * 
 * @param std::filesystem::path path given as command line argument.
 * 
 * @return `std::filesystem::path` full path
 */
inline std::filesystem::path
get_argument_path(std::filesystem::path path) {
    if (path.is_absolute()){
        return path;
    }
    else {
        // yes I know this can be unsafe as current path can change
        // we could save it as a static variable in some function
        // but it probably won't change
        return std::filesystem::current_path() / path;
    }
}

} // namespace files
