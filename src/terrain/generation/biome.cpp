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

Biome::Biome(const std::string& biome_name) {
    // quill::Logger* lua_logger;

    std::filesystem::path biome_json_path = files::get_data_path() / biome_name;

    Json::Value biome_data;
    std::ifstream biome_file =
        files::open_data_file(biome_json_path / "biome_data.json");
    biome_file >> biome_data;

    std::filesystem::path lua_map_generator_file =
        biome_json_path / biome_data["map_generator"].asString();
    if (!std::filesystem::exists(lua_map_generator_file)) {
        LOG_ERROR(
            logging::lua_logger, "File, {}, not found", lua_map_generator_file.string()
        );
        return;
    }

    read_tile_macro_data(biome_data);

    read_map_tile_data(biome_data);

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

} // namespace generation

} // namespace terrain
