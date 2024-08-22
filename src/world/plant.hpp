#pragma once

#include "util/hash_combine.hpp"

#include <compare>
#include <filesystem>
#include <optional>
#include <string>

namespace terrain {

namespace generation {

/**
 * @brief Plant identification for biome generation.
 */
// read from json
struct plant_t {
    //    std::string name; // shortened name
    // ^ whi? who does this help?
    // identification should look like path eg biome/trees/tree_type_1
    std::string identification;
    // The map that generates these plants eg Trees_1
    std::string map_name;
    // path to lua file that contains function map_name
    std::optional<std::filesystem::path> map_generator_path;

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const plant_t& other) const {
        return identification <=> other.identification;
    }

    [[nodiscard]] inline bool
    operator==(const plant_t& other) const {
        return identification == other.identification;
    }

    // only compare the identification
    // can only insert entities from one map (fight me)
};

} // namespace generation

} // namespace terrain

template <>
struct std::hash<terrain::generation::plant_t> {
    size_t
    operator()(const terrain::generation::plant_t& plant) const {
        std::hash<std::string> hasher;
        return hasher(plant.identification);
    }
};
