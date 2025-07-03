#include "biome.hpp"

#include "entity/object_handler.hpp"
#include "local_context.hpp"
#include "logging.hpp"
#include "terrain/generation/lua_interface.hpp"
#include "terrain/generation/noise.hpp"
#include "terrain/generation/worley_noise.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#include <glaze/glaze.hpp>
#pragma clang diagnostic pop

#include <sol/sol.hpp>

#include <filesystem>
#include <map>
#include <optional>
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
    if (!grass_data) {
        LOG_WARNING(logging::terrain_logger, "Grass Data Empty");
    }
}

Biome::Biome(const std::string& biome_name, size_t seed) :
    Biome(get_json_data(files::get_data_path() / biome_name), seed) {}

Biome::Biome(biome_json_data biome_data, size_t seed) :
    materials_(init_materials_(biome_data.materials_data)),
    generate_plants_(biome_data.biome_data.generate_plants),
    grass_data_(biome_data.materials_data.at("Dirt").gradient),
    lua_map_generator_file_(
        files::get_data_path() / biome_data.biome_name
        / biome_data.biome_data.map_generator_path
    ),
    name_(biome_data.biome_name), id_name_(biome_data.biome_name), seed(seed) {
    // TODO make id_name_ add id_name_ to the json, and test if it is a good id (no
    // spaces)
    read_tile_macro_data_(biome_data.biome_data.tile_macros);

    read_map_tile_data_(biome_data.biome_data.tile_data);

    read_add_to_top_data_(biome_data.biome_data.layer_effects);

    if (!std::filesystem::exists(lua_map_generator_file_)) [[unlikely]] {
        LOG_ERROR(
            logging::lua_logger, "File, {}, not found", lua_map_generator_file_.string()
        );
    }
}

TerrainMacroMap
Biome::get_map(MacroDim size) const {
    std::vector<MapTile> out;

    sol::state& lua = LocalContext::get_lua_state();
    if (!std::filesystem::exists(lua_map_generator_file_)) [[unlikely]] {
        return {};
    }

    init_lua_interface(lua);

    sol::table biome_library =
        lua.require_file("Base_Biome", lua_map_generator_file_.string(), false);

    sol::protected_function map_function = biome_library[id_name_]["biome_map"]["map"];

    if (!map_function.valid()) [[unlikely]] {
        LOG_ERROR(logging::lua_logger, "Function map not defined.");
        return {};
    }

    auto function_output = map_function(size);

    if (!function_output.valid()) {
        LOG_ERROR(logging::lua_logger, "Function result not valid.");
        sol::error err = function_output;
        sol::call_status status = function_output.status();
        LOG_ERROR(logging::lua_logger, "{}: {}", sol::to_string(status), err.what());
        return {};
    }

    if (!(function_output.get_type() == sol::type::table)) {
        LOG_ERROR(logging::lua_logger, "Function output must be a table.");
        return {};
    }

    sol::table map = function_output;

    auto tile_map_map = map["map"];
    if (!(tile_map_map.get_type() == sol::type::table)) {
        LOG_ERROR(logging::lua_logger, "Invalid map");
        return {};
    }

    auto table_x_value = map["x"];
    if (!(table_x_value.get_type() == sol::type::number)) {
        LOG_ERROR(logging::lua_logger, "Invalid x value");
        return {};
    }
    MacroDim x_map_tiles = table_x_value;

    auto table_y_value = map["y"];
    if (!(table_y_value.get_type() == sol::type::number)) {
        LOG_ERROR(logging::lua_logger, "Invalid y value");
        return {};
    }
    MacroDim y_map_tiles = table_y_value;

    assert(
        ((y_map_tiles == x_map_tiles) && (x_map_tiles == size))
        && "Map tile input and out put must all match"
    );
    out.reserve(x_map_tiles * y_map_tiles);
    for (MacroDim x = 0; x < x_map_tiles; x++) {
        for (MacroDim y = 0; y < y_map_tiles; y++) {
            size_t map_index = x * y_map_tiles + y;
            int tile_id = tile_map_map[map_index].get_or<int, int>(0);
            const TileType& tile_type = macro_tile_types_[tile_id];
            out.emplace_back(tile_type, seed, x, y);
        }
    }

    return TerrainMacroMap(out, x_map_tiles, y_map_tiles);
}

const std::unordered_map<std::string, PlantMap>
Biome::get_plant_map(Dim length) const {
    std::unordered_map<std::string, PlantMap> out;

    sol::state& lua = LocalContext::get_lua_state();

    if (!std::filesystem::exists(lua_map_generator_file_)) [[unlikely]] {
        return {};
    }

    sol::table biome_library =
        lua.require_file("Base_Biome", lua_map_generator_file_.string(), false);

    sol::protected_function plant_map =
        biome_library[id_name_]["biome_map"]["plants_map"];

    if (!plant_map.valid()) {
        LOG_ERROR(
            logging::lua_logger, "Error with plant_map in {}.", lua_map_generator_file_
        );
        return {};
    }

    sol::protected_function map_function = lua["Base"]["biome_map"]["map"];

    if (!map_function.valid()) {
        LOG_ERROR(
            logging::lua_logger, "Error with map_function in {}.",
            lua_map_generator_file_
        );
        return {};
    }

    sol::protected_function_result macor_map = map_function(length);

    if (!macor_map.valid()) {
        sol::error err = macor_map;
        sol::call_status status = macor_map.status();
        LOG_ERROR(logging::lua_logger, "{}: {}", sol::to_string(status), err.what());
        return {};
    }

    sol::table macro_map_table = macor_map;

    sol::table map = plant_map(length, macro_map_table);

    MacroDim x_map_tiles = map["x"];
    MacroDim y_map_tiles = map["y"];

    LOG_DEBUG(
        logging::lua_logger, "Input length: {}, (x, y) = ({}, {})", length, x_map_tiles,
        y_map_tiles
    );

    assert(
        ((y_map_tiles == x_map_tiles) && (x_map_tiles == length))
        && "Map tile input and out put must all match"
    );

    auto maps = sol::table(map["map"]);
    for (const auto& plant_sub_map : maps) {
        std::vector<float> type_map;

        sol::object flora_type = plant_sub_map.first;

        type_map.reserve(x_map_tiles * y_map_tiles);

        LOG_DEBUG(logging::lua_logger, "Key is {}", flora_type.as<std::string>());

        auto tile_map_map = maps[flora_type];
        for (MacroDim x = 0; x < x_map_tiles; x++) {
            for (MacroDim y = 0; y < y_map_tiles; y++) {
                size_t map_index = x * y_map_tiles + y;
                auto value = tile_map_map[map_index];

                if (value.is<float>()) {
                    type_map.emplace_back(value.get<float>());
                } else {
                    LOG_ERROR(logging::lua_logger, "Value is not a float.");
                }
            }
        }

        std::string type_as_string = flora_type.as<std::string>();

        out.emplace(std::make_pair<std::string, PlantMap>(
            std::move(type_as_string), {type_map, x_map_tiles, y_map_tiles}
        ));
    }

    return out;
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
    MapTile_t type_id = 0;
    for (const tile_data_t& tile_type : biome_data) {
        std::unordered_set<const LandGenerator*> tile_macros;
        for (TileMacro_t tile_macro : tile_type.used_tile_macros) {
            if (tile_macro >= land_generators_.size()) [[unlikely]] {
                LOG_WARNING(
                    logging::terrain_logger,
                    "Tile Macro {} is not valid. There are only {} land generators.",
                    tile_macro, land_generators_.size()
                );
                continue;
            }
            //            const LandGenerator& land_generator =
            //            land_generators_[tile_macro];
            tile_macros.insert(&land_generators_[tile_macro]);
        }
        //        TileType tile_type(tile_macros, type_id);

        macro_tile_types_.emplace_back(
            tile_macros, type_id, add_to_top_generators_, materials_
        );
        type_id++;
    }
}

void
Biome::read_add_to_top_data_(const std::vector<layer_effects_t>& after_effects_data) {
    for (const layer_effects_t& add_data : after_effects_data) {
        add_to_top_generators_.emplace_back(add_data);
    }
}

std::unordered_map<MaterialId, const terrain::material_t>
Biome::init_materials_(const all_materials_t& material_data) {
    std::unordered_map<MaterialId, const terrain::material_t> out;
    for (const auto& key : material_data) {
        out.insert(std::make_pair(key.second.material_id, key.second));
    }
    return out;
}

std::unordered_map<ColorInt, MaterialColor>
Biome::get_colors_inverse_map() const {
    std::unordered_map<ColorInt, MaterialColor> materials_inverse;
    for (const auto& element : materials_) {
        for (ColorId color_id = 0; color_id < element.second.color.size(); color_id++) {
            materials_inverse.emplace(
                element.second.color.at(color_id).hex_color,
                MaterialColor{element.second, color_id}
            );
        }
    }
    return materials_inverse;
}

biome_json_data
Biome::get_json_data(const std::filesystem::path& biome_folder_path) {
    auto biome_data = files::read_json_from_file<terrain::generation::biome_data_t>(
        biome_folder_path / "biome_data.json"
    );

    if (!biome_data) {
        return {};
    }

    auto materials = files::read_json_from_file<all_materials_t>(
        biome_folder_path / "materials.json"
    );

    if (!materials) {
        return {};
    }

    terrain::generation::biome_json_data data(
        biome_data->name, *biome_data, *materials
    );
    return data;
}

} // namespace generation

} // namespace terrain
