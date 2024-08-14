#include "biome.hpp"

#include "../../logging.hpp"
#include "noise.hpp"
#include "worley_noise.hpp"

#include <glaze/glaze.hpp>
#include <sol/sol.hpp>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace terrain {

namespace generation {

GrassData::GrassData(const grass_data_t& grass_data) {
    ColorId grass_mid_color_id = grass_data.midpoint;

    // Should be much lower than a Dim
    std::vector<Dim> grass_grad_data;
    for (Dim layer_length : grass_data.levels) {
        if (layer_length > 16) {
            LOG_WARNING(
                logging::terrain_logger, "Quite long grass length {}", layer_length
            );
        }
        grass_grad_data.push_back(layer_length);
    }

    if (grass_mid_color_id >= grass_grad_data.size()) {
        grass_mid_color_id = grass_grad_data.size() - 1;
        LOG_WARNING(
            logging::terrain_logger, "Grass Mid (from biome_data.json) too large"
        );
    }

    for (ColorId i = 0; i < grass_grad_data.size(); i++) {
        if (i == grass_mid_color_id)
            grass_mid_ = grass_colors_.size();
        for (size_t j = 0; j < grass_grad_data[i]; j++)
            grass_colors_.push_back(i);
    }
    grass_grad_length_ = grass_colors_.size();
}

GrassData::GrassData(const std::optional<grass_data_t>& grass_data) :
    GrassData(grass_data.value_or(grass_data_t())) {
    if (!grass_data.has_value()) {
        LOG_WARNING(logging::terrain_logger, "Grass Data Empty");
    }
}

Biome::Biome(const biome_json_data& biome_data, size_t seed) :
    materials_(init_materials_(biome_data.materials_data)),
    grass_data_(biome_data.materials_data.data.at("Dirt").gradient),
    seed_(seed) { // TODO
    std::filesystem::path biome_json_path =
        files::get_data_path() / biome_data.biome_name;

    std::filesystem::path lua_map_generator_file =
        biome_json_path / biome_data.biome_data.map_generator_path;

    read_tile_macro_data_(biome_data.biome_data.tile_macros);

    read_map_tile_data_(biome_data.biome_data.tile_data);

    read_add_to_top_data_(biome_data.biome_data.layer_effects);

    init_lua_state_(lua_map_generator_file);
}

void
Biome::init_lua_state(
    sol::state& lua, const std::filesystem::path& lua_map_generator_file
) {
    if (!std::filesystem::exists(lua_map_generator_file)) [[unlikely]] {
        LOG_ERROR(
            logging::lua_logger, "File, {}, not found", lua_map_generator_file.string()
        );
        return;
    }

    // add functions/libraries etc
    lua.open_libraries(sol::lib::base);
    lua.open_libraries(sol::lib::math);
    lua.new_usertype<FractalNoise>(
        "FractalNoise", sol::meta_function::construct,
        sol::factories(
            // FractalNoise.new(...) -- dot syntax, no "self" value
            // passed in
            [](int num_octaves, double persistence, int prime_index) {
                return std::make_shared<FractalNoise>(
                    num_octaves, persistence, prime_index
                );
            },
            // FractalNoise:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, int num_octaves, double persistence, int prime_index) {
                return std::make_shared<FractalNoise>(
                    num_octaves, persistence, prime_index
                );
            }
        ),
        // FractalNoise(...) syntax, only
        sol::call_constructor,
        sol::factories([](int num_octaves, double persistence, int prime_index) {
            return std::make_shared<FractalNoise>(
                num_octaves, persistence, prime_index
            );
        }),
        "sample", &FractalNoise::get_noise
    );

    lua.new_usertype<WorleyNoise>(
        "WorleyNoise", sol::meta_function::construct,
        sol::factories(
            // WorleyNoise.new(...) -- dot syntax, no "self" value
            // passed in
            [](int tile_size, double radius) {
                return std::make_shared<WorleyNoise>(tile_size, radius);
            },
            // WorleyNoise:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, int tile_size, double radius) {
                return std::make_shared<WorleyNoise>(tile_size, radius);
            }
        ),
        // WorleyNoise(...) syntax, only
        sol::call_constructor, sol::factories([](int tile_size, double radius) {
            return std::make_shared<WorleyNoise>(tile_size, radius);
        }),
        "sample", &WorleyNoise::get_noise
    );

    lua.new_usertype<AlternativeWorleyNoise>(
        "AlternativeWorleyNoise", sol::meta_function::construct,
        sol::factories(
            // AlternativeWorleyNoise.new(...) -- dot syntax, no "self" value
            // passed in
            [](int tile_size, double positive_chance, double radius) {
                return std::make_shared<AlternativeWorleyNoise>(
                    tile_size, positive_chance, radius
                );
            },
            // AlternativeWorleyNoise:new(...) -- colon syntax, passes in the
            // "self" value as first argument implicitly
            [](sol::object, int tile_size, double positive_chance, double radius) {
                return std::make_shared<AlternativeWorleyNoise>(
                    tile_size, positive_chance, radius
                );
            }
        ),
        // AlternativeWorleyNoise(...) syntax, only
        sol::call_constructor,
        sol::factories([](int tile_size, double positive_chance, double radius) {
            return std::make_shared<AlternativeWorleyNoise>(
                tile_size, positive_chance, radius
            );
        }),
        "sample", &AlternativeWorleyNoise::get_noise
    );

    auto result = lua.safe_script_file(
        lua_map_generator_file.string(), sol::script_pass_on_error
    );
    if (!result.valid()) {
        sol::error err = result;
        sol::call_status status = result.status();
        LOG_ERROR(logging::lua_logger, "{}: {}", sol::to_string(status), err.what());
        return;
    }

    sol::protected_function map_function = lua["map"];
    if (!map_function.valid()) {
        LOG_ERROR(logging::lua_logger, "Function map not defined.");
        return;
    }
}

TerrainMacroMap
Biome::map_generation_test(
    const std::filesystem::path& lua_map_generator_file, size_t size
) {
    sol::state lua;

    Biome::init_lua_state(lua, lua_map_generator_file);

    std::vector<MapTile> out;

    sol::protected_function map_function = lua["map"];

    if (!map_function.valid()) [[unlikely]] {
        LOG_ERROR(logging::lua_logger, "Function map not defined.");
        return {};
    }

    sol::table map = map_function(size);

    auto tile_map_map = map["map"];
    MacroDim x_map_tiles = map["x"]; // should be 16
    MacroDim y_map_tiles = map["y"];
    assert(
        ((y_map_tiles == x_map_tiles) && (x_map_tiles == size))
        && "Map tile input and out put must all match"
    );
    out.reserve(x_map_tiles * y_map_tiles);
    for (MacroDim x = 0; x < x_map_tiles; x++) {
        for (MacroDim y = 0; y < y_map_tiles; y++) {
            size_t map_index = x * y_map_tiles + y;
            out.emplace_back(tile_map_map[map_index], 0, x, y);
        }
    }

    return TerrainMacroMap(out, x_map_tiles, y_map_tiles);
}

const TerrainMacroMap
Biome::get_map(MacroDim length) const {
    std::vector<MapTile> out;

    sol::protected_function map_function = lua_["map"];

    sol::table map = map_function(length);

    auto tile_map_map = map["map"];
    MacroDim x_map_tiles = map["x"]; // should be 16
    MacroDim y_map_tiles = map["y"];
    assert(
        ((y_map_tiles == x_map_tiles) && (x_map_tiles == length))
        && "Map tile input and out put must all match"
    );
    out.reserve(x_map_tiles * y_map_tiles);
    for (MacroDim x = 0; x < x_map_tiles; x++) {
        for (MacroDim y = 0; y < y_map_tiles; y++) {
            size_t map_index = x * y_map_tiles + y;
            out.emplace_back(tile_map_map[map_index], seed_, x, y);
        }
    }

    return TerrainMacroMap(out, x_map_tiles, y_map_tiles);
}

void
Biome::read_tile_macro_data_(const std::vector<tile_macros_t>& tile_macros) {
    // for tile macro in data biome
    for (const tile_macros_t& tile_macro : tile_macros) {
        // create a land generator for each tile macro
        generation::LandGenerator gen(tile_macro.generation_data);
        land_generators_.push_back(gen);
    }
}

void
Biome::read_map_tile_data_(const std::vector<tile_data_t>& biome_data) {
    // add tile macro to tiles
    for (const tile_data_t& tile_type : biome_data) {
        std::vector<TileMacro_t> tile_macros;
        for (TileMacro_t tile_macro : tile_type.used_tile_macros) {
            if (tile_macro >= land_generators_.size()) [[unlikely]] {
                LOG_WARNING(
                    logging::terrain_logger,
                    "Tile Macro {} is not valid. There are only {} land generators.",
                    tile_macro, land_generators_.size()
                );
                continue;
            }
            tile_macros.push_back(tile_macro);
        }
        macro_tile_types_.push_back(std::move(tile_macros));
    }
}

void
Biome::read_add_to_top_data_(const std::vector<layer_effects_t>& after_effects_data) {
    for (const layer_effects_t& add_data : after_effects_data) {
        add_to_top_generators_.emplace_back(add_data);
    }
}

std::map<MaterialId, const terrain::Material>
Biome::init_materials_(const all_materials_t& material_data) {
    std::map<MaterialId, const terrain::Material> out;
    for (const auto& key : material_data.data) {
        std::vector<std::pair<const std::string, ColorInt>> color_vector;

        const auto& material = key.second;
        std::string material_name = key.first.data();
        for (const auto& color : material.color) {
            const std::string color_name = color.color_name;
            ColorInt color_value = color.hex_color;
            color_vector.push_back(std::make_pair(std::move(color_name), color_value));
        }

        terrain::Material mat{
            std::move(color_vector),   // color
            material.speed_multiplier, // speed_multiplier
            material.solid,            // solid
            material.material_id,      // element_id
            std::move(material_name)   // material_name
        };
        out.insert(std::make_pair(mat.material_id, std::move(mat)));
    }
    return out;
}

std::map<ColorInt, std::pair<const Material*, ColorId>>
Biome::get_colors_inverse_map() const {
    std::map<ColorInt, std::pair<const Material*, ColorId>> materials_inverse;
    for (const auto& element : materials_) {
        for (ColorId color_id = 0; color_id < element.second.color.size(); color_id++) {
            materials_inverse.insert(
                std::map<ColorInt, std::pair<const Material*, ColorId>>::value_type(
                    element.second.color.at(color_id).second,
                    std::make_pair(&element.second, color_id)
                )
            );
        }
    }
    return materials_inverse;
}

biome_json_data
Biome::get_json_data_(const std::string& biome_name) {
    std::filesystem::path biome_json_path = files::get_data_path() / biome_name;
    auto biome_file = files::open_data_file(biome_json_path / "biome_data.json");

    glz::context ctx{};

    biome_data_t biome_data{};
    if (biome_file.has_value()) {
        std::string content(
            (std::istreambuf_iterator<char>(biome_file.value())),
            std::istreambuf_iterator<char>()
        );
        auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(
            biome_data, content, ctx
        );
        if (ec) {
            LOG_ERROR(logging::file_io_logger, "{}", glz::format_error(ec, content));
            return {};
        }
    } else {
        return {};
    }
    auto materials_file = files::open_data_file(biome_json_path / "materials.json");

    all_materials_t materials_data;
    if (materials_file.has_value()) {
        std::string content(
            (std::istreambuf_iterator<char>(materials_file.value())),
            std::istreambuf_iterator<char>()
        );
        auto ec = glz::read<glz::opts{.error_on_unknown_keys = false}>(
            materials_data, content, ctx
        );
        if (ec) {
            LOG_ERROR(logging::file_io_logger, "{}", glz::format_error(ec, content));
            return {};
        }

        return {biome_name, std::move(biome_data), std::move(materials_data)};
    } else {
        return {};
    }
}

} // namespace generation

} // namespace terrain
