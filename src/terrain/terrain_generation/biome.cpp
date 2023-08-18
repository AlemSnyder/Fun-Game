#include "biome.hpp"

#include "../../logging.hpp"

#include <json/json.h>

#include <quill/Quill.h>
#include <sol/sol.hpp>

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace terrain {

namespace terrain_generation {

Biome::Biome(std::string biome_name) {
    // quill::Logger* lua_logger;

    std::filesystem::path biome_json_path = files::get_data_path() / biome_name;

    Json::Value biome_data;
    std::ifstream biome_file =
        files::open_data_file(biome_json_path / "biome_data.json");
    biome_file >> biome_data;

    std::filesystem::path lua_map_generator_file =
        biome_json_path / biome_data["map_generator"].asString();
    sol::state lua;

    auto result = lua.script_file(lua_map_generator_file, sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        sol::call_status status = result.status();
        LOG_CRITICAL(logging::lua_logger, "{}: {}", sol::to_string(status), err.what());
    }

    sol::table map = lua["map"](16);

    auto tile_map_map = map["map"];
    int x_map_tiles = map["x"]; // should be 16
    int y_map_tiles = map["y"];
    tile_map_vector_.resize(x_map_tiles * y_map_tiles);
    for (size_t i = 0; i < tile_map_vector_.size(); i++) {
        tile_map_vector_[i] = tile_map_map[i];
    }
}

} // namespace terrain_generation

} // namespace terrain
