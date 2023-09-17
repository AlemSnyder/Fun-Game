#include "biome.hpp"

#include "../../logging.hpp"
#include "noise.hpp"

#include <json/json.h>

#include <quill/Quill.h>
#include <sol/sol.hpp>

#include <filesystem>
#include <map>
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

Biome::Biome(const biome_json_data& biome_data) :
    materials_(init_materials(biome_data.materials_data)),
    grass_data_(biome_data.materials_data["Dirt"]["Gradient"]) {
    std::filesystem::path biome_json_path =
        files::get_data_path() / biome_data.biome_name;

    std::filesystem::path lua_map_generator_file =
        biome_json_path / biome_data.biome_data["map_generator"].asString();
    if (!std::filesystem::exists(lua_map_generator_file)) {
        LOG_ERROR(
            logging::lua_logger, "File, {}, not found", lua_map_generator_file.string()
        );
        return;
    }

    read_tile_macro_data(biome_data.biome_data["Biome"]);

    read_map_tile_data(biome_data.biome_data["Biome"]);

    read_add_to_top_data(biome_data.biome_data["Biome"]["After_Effects"]);

    sol::state lua;

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
    MacroDim map_tiles = 16;
    sol::table map = map_function(map_tiles);

    auto tile_map_map = map["map"];
    MacroDim x_map_tiles = map["x"]; // should be 16
    MacroDim y_map_tiles = map["y"];
    assert(
        ((y_map_tiles == x_map_tiles) && (x_map_tiles == map_tiles))
        && "Map tile input and out put must all match"
    );
    tile_map_vector_.resize(x_map_tiles * y_map_tiles);
    for (size_t i = 0; i < tile_map_vector_.size(); i++) {
        tile_map_vector_[i] = tile_map_map[i];
    }
}

void
Biome::read_tile_macro_data(const Json::Value& biome_data) {
    // for tile macro in data biome
    for (const Json::Value& tile_macro : biome_data["Tile_Macros"]) {
        // create a land generator for each tile macro
        generation::LandGenerator gen(tile_macro["Land_Data"]);
        land_generators_.push_back(gen);
    }
}

void
Biome::read_map_tile_data(const Json::Value& biome_data) {
    // add tile macro to tiles
    for (const Json::Value& tile_type : biome_data["Tile_Data"]) {
        std::vector<TileMacro_t> tile_macros;
        for (const Json::Value& tile_macro_id : tile_type["Land_From"]) {
            TileMacro_t tile_macro = tile_macro_id.asInt();
            if (tile_macro >= land_generators_.size()) [[unlikely]] {
                LOG_WARNING(
                    logging::terrain_logger,
                    "Tile Macro {} is not valid. There are only {} land generators.",
                    tile_macro, land_generators_.size()
                );
                continue;
            }
            tile_macros.push_back(tile_macro_id.asInt());
        }
        macro_tile_types_.push_back(std::move(tile_macros));
    }
}

void
Biome::read_add_to_top_data(const Json::Value& after_effects_data) {
    for (const Json::Value& add_data : after_effects_data["Add_To_Top"]) {
        add_to_top_generators_.emplace_back(add_data);
    }
}

std::map<MaterialId, const terrain::Material>
Biome::init_materials(const Json::Value& material_data) {
    std::map<MaterialId, const terrain::Material> out;
    for (auto element_it = material_data.begin(); element_it != material_data.end();
         element_it++) {
        std::vector<std::pair<const std::string, ColorInt>> color_vector;

        const Json::Value material = *element_it;
        std::string name = element_it.key().asString();
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
    // I think this should be delayed
    // terrain::TerrainColorMapping::assign_color_mapping(out);
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
Biome::get_json_data(const std::string& biome_name) {
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
