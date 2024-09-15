#include "biome.hpp"

#include "entity/object_handler.hpp"
#include "logging.hpp"
#include "terrain/generation/noise.hpp"
#include "terrain/generation/worley_noise.hpp"

#include <json/json.h>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace terrain {

namespace generation {

GrassData::GrassData(const Json::Value& json_grass_data) {
    ColorId grass_mid_color_id = json_grass_data["midpoint"].asInt();

    // Should be much lower than a Dim
    std::vector<Dim> grass_grad_data;
    for (const Json::Value& grass_level : json_grass_data["levels"]) {
        Dim level_length = grass_level.asInt();
        if (level_length > 16) {
            LOG_WARNING(
                logging::terrain_logger, "Quite long grass length {}", level_length
            );
        }
        grass_grad_data.push_back(level_length);
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

Biome::Biome(const biome_json_data& biome_data, size_t seed) :
    materials_(init_materials_(biome_data.materials_data)),
    grass_data_(biome_data.materials_data["Dirt"]["Gradient"]), seed_(seed) {
    std::filesystem::path biome_json_path =
        files::get_data_path() / biome_data.biome_name;

    std::filesystem::path lua_map_generator_file =
        biome_json_path / biome_data.biome_data["map_generator"].asString();

    read_tile_macro_data_(biome_data.biome_data["Biome"]);

    read_map_tile_data_(biome_data.biome_data["Biome"]);

    read_add_to_top_data_(biome_data.biome_data["Biome"]["After_Effects"]);

    read_plants_data_(biome_data.biome_data["Biome"]["Generate_Plants"]);

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
Biome::get_map(MacroDim size) const {
    std::vector<MapTile> out;

    sol::protected_function map_function = lua_["map"];

    if (!map_function.valid()) [[unlikely]] {
        LOG_ERROR(logging::lua_logger, "Function map not defined.");
        return {};
    }

    auto function_output = map_function(size);

    if (!function_output.valid()) {
        LOG_ERROR(logging::lua_logger, "Function result not valid.");
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
            out.emplace_back(tile_type, seed_, x, y);
        }
    }

    return TerrainMacroMap(out, x_map_tiles, y_map_tiles);
}

const std::map<std::string, PlantMap>
Biome::get_plant_map(Dim length) const {
    std::map<std::string, PlantMap> out;

    sol::protected_function plant_map = lua_["plants_map"];

    sol::protected_function map_function = lua_["map"];

    sol::table macor_map = map_function(length);

    sol::table map = plant_map(length, macor_map);

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
Biome::read_tile_macro_data_(const Json::Value& biome_data) {
    // for tile macro in data biome
    for (const Json::Value& tile_macro : biome_data["Tile_Macros"]) {
        // create a land generator for each tile macro
        generation::LandGenerator gen(tile_macro["Land_Data"]);
        land_generators_.push_back(gen);
    }
}

void
Biome::read_map_tile_data_(const Json::Value& biome_data) {
    // add tile macro to tiles
    int type_id = 0; // TODO change from int
    for (const Json::Value& tile_type_data : biome_data["Tile_Data"]) {
        std::unordered_set<const LandGenerator*> tile_macros;
        for (const Json::Value& tile_macro_id : tile_type_data["Land_From"]) {
            TileMacro_t tile_macro = tile_macro_id.asInt();
            if (tile_macro >= land_generators_.size()) [[unlikely]] {
                LOG_WARNING(
                    logging::terrain_logger,
                    "Tile Macro {} is not valid. There are only {} land generators.",
                    tile_macro, land_generators_.size()
                );
                continue;
            }
//            const LandGenerator& land_generator = land_generators_[tile_macro];
            tile_macros.insert(&land_generators_[tile_macro]);
        }
//        TileType tile_type(tile_macros, type_id);

        macro_tile_types_.emplace_back(tile_macros, type_id, add_to_top_generators_, materials_);
    }
}

void
Biome::read_add_to_top_data_(const Json::Value& after_effects_data) {
    for (const Json::Value& add_data : after_effects_data["Add_To_Top"]) {
        add_to_top_generators_.emplace_back(add_data);
    }
}

void
Biome::read_plants_data_(const Json::Value& plants_data) {
    for (const Json::Value& plant : plants_data) {
        generate_plants_.emplace(
            plant["name"].asString(), plant["identification"].asString(),
            plant["map_name"].asString()
        );
    }
}

std::map<MaterialId, const terrain::Material>
Biome::init_materials_(const Json::Value& material_data) {
    std::map<MaterialId, const terrain::Material> out;
    for (const auto& key : material_data.getMemberNames()) {
        std::vector<std::pair<const std::string, ColorInt>> color_vector;

        const Json::Value material = material_data[key];
        std::string name = key;
        for (const Json::Value& json_color : material["colors"]) {
            const std::string color_name = json_color["name"].asString();
            ColorInt color_value = std::stoll(json_color["hex"].asString(), 0, 16);
            color_vector.push_back(std::make_pair(std::move(color_name), color_value));
        }

        terrain::Material mat{
            color_vector,                                    // color
            static_cast<float>(material["speed"].asFloat()), // speed_multiplier
            material["solid"].asBool(),                      // solid
            static_cast<MaterialId>(material["id"].asInt()), // element_id
            name                                             // name
        };
        out.insert(std::make_pair(mat.element_id, std::move(mat)));
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

    Json::Value biome_data;
    auto biome_file = files::open_data_file(biome_json_path / "biome_data.json");
    if (biome_file.has_value())
        biome_file.value() >> biome_data;

    Json::Value materials_data;
    auto materials_file = files::open_data_file(biome_json_path / "materials.json");
    if (materials_file.has_value())
        materials_file.value() >> materials_data;

    return {biome_name, std::move(biome_data), std::move(materials_data)};
}

} // namespace generation

} // namespace terrain
