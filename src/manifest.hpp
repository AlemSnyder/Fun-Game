#pragma once

#include <glaze/glaze.hpp>

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace manifest {

struct descriptor_t {
    std::string identification;
    std::filesystem::path path;
};

struct manifest_t {
    std::string name;
    std::optional<std::vector<descriptor_t>> biomes;
    std::optional<std::vector<descriptor_t>> entities;
    // what other types are there?
    // events
    // items may be different from entities
    // crafters
    // etc
};

} // namespace manifest

template <>
inline glz::detail::any_t::operator std::vector<manifest::descriptor_t>() const {
    assert(false && "Not Implemented");
    return {};
}
