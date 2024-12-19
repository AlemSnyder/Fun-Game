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

#include "logging.hpp"

#include <glaze/glaze.hpp>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

namespace files {

/**
 * @brief Get the path to the root loading dir.
 */
std::filesystem::path get_root_path();

/**
 * @brief Get the path to the data folder
 */
inline std::filesystem::path
get_data_path() {
    return get_root_path() / "data";
}

/**
 * @brief Get the path to the resource folder
 */
inline std::filesystem::path
get_resources_path() {
    return get_root_path() / "resources";
}

/**
 * @brief Get the path to the manifest folder
 */
inline std::filesystem::path
get_manifest_path() {
    return get_data_path() / "manifest";
}

/**
 * @brief Get the path to the logging directory
 */
inline std::filesystem::path
get_log_path() noexcept {
    return files::get_root_path() / "logs";
}

/**
 * @brief Get an ifstream of the contents of a given file
 */
std::optional<std::ifstream> open_file(std::filesystem::path path);

/**
 * @brief Get an ifstream of the contents of a file in the data folder
 */
inline std::optional<std::ifstream>
open_data_file(std::filesystem::path path) {
    return open_file(get_data_path() / path);
}

/**
 * @brief Get an ifstream of the contents of a file in the resources folder
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
    if (path.is_absolute()) {
        return path;
    } else {
        // yes I know this can be unsafe as current path can change
        // we could save it as a static variable in some function
        // but it probably won't change
        return std::filesystem::current_path() / path;
    }
}

template <class T>
std::optional<T>
read_json_from_file(std::filesystem::path path) {
    auto logger = logging::file_io_logger;

    if (!path.is_absolute()) {
        LOG_WARNING(
            logger, "Path {} is not absolute. Best to use absolute path.", path
        );
    }

    if (!std::filesystem::exists(path)) {
        LOG_ERROR(logger, "Path {} does not exist.", path);
        return {};
    }

    T data;
    auto ec = glz::read_file_json(data, path.c_str(), std::string{});

    if (ec) {
        LOG_ERROR(
            logger, "Failed to read {} from path {}. Error: {}", typeid(T).name(), path,
            glz::format_error(ec)
        );
        return {};
    }
    return data;
}

} // namespace files
