#include "biome.hpp"

#include "global_context.hpp"
#include "local_context.hpp"
#include "manifest/object_handler.hpp"
#include "logging.hpp"
#include "util/files.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#include <glaze/glaze.hpp>
#pragma clang diagnostic pop

#include <filesystem>
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

Biome::Biome(biome_data_t biome_data, size_t seed) :
    materials_(init_materials_(
        files::read_json_from_file<all_materials_t>(biome_data.materials_path)
            .value_or(std::unordered_map<std::string, terrain::material_t>())
    )),
    generate_plants_(biome_data.generate_plants),
    grass_data_(materials_.at(DIRT_ID).gradient),
    map_generator_file_(biome_data.map_generator_path), name_(biome_data.name),
    id_name_(biome_data.id), seed(seed) {

    auto material_read =
        files::read_json_from_file<all_materials_t>(biome_data.materials_path);

    if (!material_read) {
        return;
    }

    read_tile_macro_data_(biome_data.tile_macros);
    read_map_tile_data_(biome_data.tile_data);
    read_add_to_top_data_(biome_data.layer_effects);

    if (!std::filesystem::exists(biome_data.materials_path)) [[unlikely]] {
        LOG_WARNING(
            logging::as_logger,
            "File, {}, not found. Materials may not have been loaded.",
            biome_data.materials_path.string()
        );
    }

    if (!std::filesystem::exists(map_generator_file_)) [[unlikely]] {
        LOG_ERROR(
            logging::as_logger,
            "File, {}, not found. Will not be able to generate biome map.",
            map_generator_file_.string()
        );
    }

    auto& global_context = GlobalContext::instance();

    // This is technically lazy loading
    global_context.load_file(id_name_, biome_data.map_generator_path);

}

TerrainMacroMap
Biome::get_map(MacroDim size) const {
    auto& global_context = GlobalContext::instance();
    auto& local_context = LocalContext::instance();

    auto type = global_context.get_type(id_name_, "biome_map");
    if (type == nullptr) {
        return {};
    }
    auto factory_function =
        type->GetFactoryByDecl("biome_map@ biome_map()");

    auto result = local_context.run_function(factory_function);
    if (!result) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript getting biome map");
        return {};
    }
    AngelScript::asIScriptObject* biome_map = local_context.get_return_object();
    if (biome_map == nullptr) {
        LOG_ERROR(logging::main_logger, "Failed to get object");
        return {};
    }
    biome_map->AddRef();

    AngelScript::asIScriptFunction* method =
        type->GetMethodByDecl("int sample(int, int)");
    if (method == nullptr) {
        LOG_WARNING(logging::main_logger, "Could not find biome map function.");
        return {};
    }
    result = local_context.run_method(biome_map, method, 5, 5);

    std::vector<MapTile> out;

    MacroDim x_map_tiles = size;
    MacroDim y_map_tiles = size;

    out.reserve(x_map_tiles * y_map_tiles);
    for (MacroDim x = 0; x < x_map_tiles; x++) {
        for (MacroDim y = 0; y < y_map_tiles; y++) {
            int x_copy = x;
            int y_copy = y;
            auto result_2 = local_context.run_method<int>(
                biome_map, method, std::move(x_copy), std::move(y_copy)
            );
            if (!result_2) {
                LOG_ERROR(
                    logging::main_logger, "Error code {}",
                    static_cast<int>(result_2.error())
                );
                return {};
            }

            int tile_id = result_2.value();
            const TileType& tile_type = macro_tile_types_[tile_id];
            out.emplace_back(tile_type, seed, x, y);
        }
    }

    biome_map->Release();

    return TerrainMacroMap(out, x_map_tiles, y_map_tiles);
}

const std::unordered_map<std::string, PlantMap>
Biome::get_plant_map(Dim length) const {
    std::unordered_map<std::string, PlantMap> out;

    auto& global_context = GlobalContext::instance();
    auto& local_context = LocalContext::instance();

    auto type = global_context.get_type(id_name_, "biome_map");
    auto factory_function =
        type->GetFactoryByDecl("biome_map@ biome_map()");

    auto result = local_context.run_function(factory_function);
    if (!result) {
        LOG_ERROR(logging::main_logger, "Failed AngelScript getting biome map");
        return {};
    }
    AngelScript::asIScriptObject* biome_map = local_context.get_return_object();
    if (biome_map == nullptr) {
        LOG_ERROR(logging::main_logger, "Failed to get object");
        return {};
    }
    biome_map->AddRef();

    AngelScript::asIScriptFunction* script_method =
        type->GetMethodByDecl("float sample_plants(const string &in, int, int)");
    //result = local_context.run_method(biome_map, script_method, 5, 5);
    if (script_method == nullptr) {
        LOG_WARNING(logging::main_logger, "Could not find biome map function.");
        return {};
    }

    for (auto& plant : generate_plants_) {
        std::string plant_map_name = plant.map_name;

        std::vector<float> plant_data;

        MacroDim x_map_tiles = length;
        MacroDim y_map_tiles = length;

        out.reserve(x_map_tiles * y_map_tiles);
        for (MacroDim x = 0; x < x_map_tiles; x++) {
            for (MacroDim y = 0; y < y_map_tiles; y++) {
                int x_copy = x;
                int y_copy = y;
                auto result_2 = local_context.run_method<float>(
                    biome_map, script_method, &plant_map_name, std::move(x_copy),
                    std::move(y_copy)
                );
                if (!result) {
                    LOG_ERROR(
                        logging::script_logger, "Error code {}",
                        static_cast<int>(result.error())
                    );
                    return {};
                }

                float plant_probability = result_2.value();
                plant_data.push_back(plant_probability);
            }
        }
        out.emplace(plant_map_name, PlantMap(plant_data, length, length));
    }

    biome_map->Release();

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

} // namespace generation

} // namespace terrain
