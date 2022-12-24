#pragma once

#include <exception>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace exc {

class file_not_found_error : public std::runtime_error {
    const std::string ERROR_PREFIX_ = "Could not open ";

    const std::filesystem::path path_;

 public:
    file_not_found_error(const std::filesystem::path path) :
        runtime_error(ERROR_PREFIX_ + path.string()), path_(path) {}

    file_not_found_error(const file_not_found_error& other) noexcept = default;

    file_not_found_error& operator=(const file_not_found_error& other
    ) noexcept = default;

    [[nodiscard]] virtual inline const std::filesystem::path
    path() const noexcept {
        return path_;
    }
};

class not_implemented_error : public std::logic_error {
 public:
    not_implemented_error(const std::string what) : logic_error(what) {}

    not_implemented_error(const char* what) : logic_error(what) {}

    not_implemented_error(const not_implemented_error& other) noexcept = default;

    not_implemented_error& operator=(const not_implemented_error& other
    ) noexcept = default;
};

} // namespace exc
