#pragma once

#include "types.hpp"
#include "world/plant.hpp"

#include <glaze/glaze.hpp>

#include <filesystem>
#include <map>
#include <unordered_set>
#include <variant>
#include <vector>

namespace terrain {

namespace generation {

enum class generation_stamp_type {
    GRID,
    RADIUS,
    POSITION,
};

struct material_designation_t {
    std::variant<bool, MaterialId, std::vector<MaterialId>>
        material;                                            // bool or MaterialId
    std::variant<bool, ColorId, std::vector<ColorId>> color; // bool or ColorId
};

struct stamp_generation_grid_data_t {
    int radius;
    int number;
};

struct stamp_generation_radius_data_t {
    int radius;
    int number;
};

struct stamp_generation_position_data_t {
    std::vector<std::pair<int, int>> positions;
};

struct generation_stamp_t {
    generation_stamp_type type;
    MaterialId material_id;
    ColorId color_id;
    std::vector<material_designation_t> can_override;
    int size;
    int size_range;
    int height;
    int height_range;
    int center_range;
    std::optional<stamp_generation_grid_data_t> grid;
    std::optional<stamp_generation_position_data_t> position;
    std::optional<stamp_generation_radius_data_t> radius;
};

struct tile_macros_t {
    std::vector<generation_stamp_t> generation_data;
};

struct tile_data_t {
    std::vector<int> used_tile_macros;
};

enum class layer_effect_add : uint8_t {
    NONE = 0,
    TO = 1,
    ADD = 2,
};

struct layer_effect_data_t {
    // enum class about add n vs to n

    Dim start;
    Dim stop;
    Dim data;

    layer_effect_add add_directions;

    [[nodiscard]] inline std::strong_ordering
    operator<=>(const layer_effect_data_t& other) const {
        // c for compare
        auto stop_c = stop <=> other.stop;
        if (stop_c != std::strong_ordering::equivalent) {
            return stop_c;
        }
        auto start_c = start <=> other.start;
        if (start_c != std::strong_ordering::equivalent) {
            return start_c;
        }

        // we want to compare directions like this because going to a position is
        // bigger than adding some value assuming the stop is the same
        auto directions_c =
            static_cast<int>(add_directions) <=> static_cast<int>(other.add_directions);
        if (directions_c != std::strong_ordering::equivalent) {
            return directions_c;
        }

        return data <=> other.data;
    }

    [[nodiscard]] bool operator==(const layer_effect_data_t&) const = default;
};

struct layer_effects_t {
    MaterialId material_id;
    ColorId color_id;

    std::vector<material_designation_t> above_colors;
    std::vector<material_designation_t> can_override;
    std::vector<layer_effect_data_t> how_to_add;
};

struct biome_data_t {
    std::filesystem::path map_generator_path;
    std::filesystem::path image_path;
    std::string description;
    std::string name;

    std::vector<tile_macros_t> tile_macros;
    std::vector<tile_data_t> tile_data;
    std::vector<layer_effects_t> layer_effects;

    std::unordered_set<plant_t> generate_plants;
};

} // namespace generation

} // namespace terrain

template <>
struct glz::meta<terrain::generation::generation_stamp_type> {
    using enum terrain::generation::generation_stamp_type;
    static constexpr auto value = enumerate(GRID, RADIUS, POSITION);
};

template <>
struct glz::meta<terrain::generation::layer_effect_add> {
    using enum terrain::generation::layer_effect_add;
    static constexpr auto value = enumerate(NONE, TO, ADD);
};

// error with glaze
// when using optional glaze causes compiler errors and this suppresses those errors
template <>
inline glz::detail::any_t::operator terrain::generation::stamp_generation_grid_data_t(
) const {
    assert(false && "Not Implemented");
    return {};
}

template <>
inline glz::detail::any_t::operator terrain::generation::
    stamp_generation_position_data_t() const {
    assert(false && "Not Implemented");
    return {};
}

template <>
inline glz::detail::any_t::operator terrain::generation::stamp_generation_radius_data_t(
) const {
    assert(false && "Not Implemented");
    return {};
}
