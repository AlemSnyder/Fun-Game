#pragma once

#include "util/hash_combine.hpp"

#include <compare>
#include <string>

namespace terrain {

namespace generation {

/**
 * @brief Plant identification for biome generation.
 */
// read from json
struct plant_t {
    std::string name; // shortened name
    // identification should look like path eg biome/trees/tree_type_1
    std::string identification;
    // The map that generates these plants eg Trees_1
    std::string map_name;

    [[nodiscard]] inline std::strong_ordering operator<=>(const plant_t& other
    ) const = default;
};

} // namespace generation

} // namespace terrain

template <>
struct std::hash<terrain::generation::plant_t> {
    size_t
    operator()(const terrain::generation::plant_t& plant) const {
        size_t start = 0;
        utils::hash_combine(start, plant.name);
        utils::hash_combine(start, plant.identification);
        utils::hash_combine(start, plant.map_name);
        return start;
    }
};
