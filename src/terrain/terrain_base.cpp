#include "terrain_base.hpp"

#include "../logging.hpp"
#include "../types.hpp"
#include "generation/noise.hpp"

// move add_to_top

namespace terrain {

void
TerrainBase::qb_read(
    std::vector<ColorInt> data,
    const std::map<ColorInt, std::pair<const Material*, ColorId>>& materials_inverse
) {
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    std::set<ColorInt> unknown_colors;

    for (size_t xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        TerrainDim3 tile_position = sop(xyz);
        ColorInt color = data[xyz];
        if (color == 0) {                             // if the qb voxel is transparent.
            auto mat_color = materials_inverse.at(0); // set the materials to air
            tiles_.push_back(Tile(tile_position, mat_color.first, mat_color.second));
        } else if (materials_inverse.count(color)) { // if the color is known
            auto mat_color = materials_inverse.at(color);
            tiles_.push_back(Tile(tile_position, mat_color.first, mat_color.second));
        } else { // the color is unknown
            unknown_colors.insert(color);
            auto mat_color = materials_inverse.at(0); // else set to air.
            tiles_.push_back(Tile(tile_position, mat_color.first, mat_color.second));
        }
    }

    for (ColorInt color : unknown_colors) {
        LOG_WARNING(logging::terrain_logger, "Cannot find color: {:x}", color);
    }
}

// when data is given use different Y max
TerrainBase::TerrainBase(
    const std::map<MaterialId, const terrain::Material>& materials,
    const std::vector<int>& grass_grad_data, unsigned int grass_mid, Dim x_map_tiles,
    Dim y_map_tiles, Dim area_size, Dim z_tiles
) :
    area_size_(area_size),
    materials_(materials), X_MAX(x_map_tiles * area_size),
    Y_MAX(y_map_tiles * area_size), Z_MAX(z_tiles) {
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    if (grass_mid >= grass_grad_data.size()) {
        grass_mid_ = grass_grad_data.size() - 1;
        std::cerr << "Grass Mid (from biome_data.json) not valid";
    }

    for (size_t i = 0; i < grass_grad_data.size(); i++) {
        if (i == static_cast<size_t>(grass_mid)) {
            grass_mid_ = grass_colors_.size();
        }
        for (int j = 0; j < grass_grad_data[i]; j++) {
            grass_colors_.push_back(i);
        }
    }
    grass_grad_length_ = grass_colors_.size();
}

TerrainBase::TerrainBase(
    Dim x_map_tiles, Dim y_mat_tiles, Dim area_size, Dim z_tiles,
    const std::map<MaterialId, const Material>& materials,
    const Json::Value& biome_data, std::vector<int> grass_grad_data,
    unsigned int grass_mid, std::vector<int> Terrain_Maps
) :
    TerrainBase(
        materials, grass_grad_data, grass_mid, x_map_tiles, y_mat_tiles, area_size,
        z_tiles
    ) {
    for (size_t xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        tiles_.push_back(Tile(sop(xyz), &materials_.at(0), 0));
    }

    // srand(seed);
    LOG_INFO(logging::terrain_logger, "Start of land generator.");

    // create a map of int -> LandGenerator
    std::vector<generation::LandGenerator> land_generators;

    // for tile macro in data biome
    for (const Json::Value& tile_macro : biome_data["Tile_Macros"]) {
        // create a land generator for each tile macro
        generation::LandGenerator gen(materials, tile_macro["Land_Data"]);
        land_generators.push_back(gen);
    }

    LOG_INFO(
        logging::terrain_logger, "End of land generator: create macro tile generator."
    );

    // TODO make this faster 4
    for (size_t i = 0; i < x_map_tiles; i++)
        for (size_t j = 0; j < y_mat_tiles; j++) {
            int tile_type = Terrain_Maps[j + i * y_mat_tiles];
            Json::Value macro_types = biome_data["Tile_Data"][tile_type]["Land_From"];
            for (Json::Value generator_macro : macro_types) {
                init_area(i, j, land_generators.at(generator_macro.asInt()));
            }
        }

    LOG_INFO(logging::terrain_logger, "End of land generator: place tiles.");

    // TODO make this faster 3
    for (const Json::Value& after_affect : biome_data["After_Effects"]["Add_To_Top"]) {
        add_to_top(after_affect, materials);
    }

    LOG_INFO(logging::terrain_logger, "End of land generator: top layer placement.");
}

TerrainBase::TerrainBase(
    const std::map<MaterialId, const Material>& materials,
    std::vector<int> grass_grad_data, unsigned int grass_mid,
    voxel_utility::qb_data_t data
) :
    area_size_(32),
    materials_(materials), X_MAX(data.size.x), Y_MAX(data.size.y), Z_MAX(data.size.z) {
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    if (grass_mid >= grass_grad_data.size()) {
        grass_mid_ = grass_grad_data.size() - 1;
        std::cerr << "Grass Mid (from biome_data.json) not valid";
    }

    for (size_t i = 0; i < grass_grad_data.size(); i++) {
        if (i == static_cast<size_t>(grass_mid)) {
            grass_mid_ = grass_colors_.size();
        }
        for (int j = 0; j < grass_grad_data[i]; j++) {
            grass_colors_.push_back(i);
        }
    }
    grass_grad_length_ = grass_colors_.size();
    std::map<ColorInt, std::pair<const Material*, ColorId>> materials_inverse;
    for (auto it = materials_.begin(); it != materials_.end(); it++) {
        for (size_t color_id = 0; color_id < it->second.color.size(); color_id++) {
            materials_inverse.insert(
                std::map<ColorInt, std::pair<const Material*, ColorId>>::value_type(
                    it->second.color.at(color_id).second,
                    std::make_pair(&it->second, (ColorId)color_id)
                )
            );
        }
    }

    try {
        qb_read(data.data, materials_inverse);
    } catch (const std::exception& e) {
        LOG_ERROR(
            logging::terrain_logger, "Could not load terrain save file due to {}",
            e.what()
        );
        throw;
    }
}

int
TerrainBase::get_first_not(
    const std::set<std::pair<MaterialId, ColorId>>& materials, int x, int y, int guess
) const {
    if (guess < 1) {
        guess = 1;
    } else if (static_cast<Dim>(guess) >= Z_MAX) {
        guess = Z_MAX - 1;
    }
    if (has_tile_material(materials, x, y, guess - 1)) {
        if (has_tile_material(materials, x, y, guess)) {
            return guess;
        } else {
            // go up
            for (Dim z = guess + 1; z < Z_MAX; z++) {
                if (has_tile_material(materials, x, y, z)) {
                    return z;
                }
            }
            return Z_MAX; // -1? should not be minus one, but one should consider that
                          // this is a possible return value
        }
    } else if (guess == 1) {
        return 0;
    } else {
        // go down
        for (Dim z = guess - 2; z != 0; z--) {
            if (has_tile_material(materials, x, y, z)) {
                return z + 1;
            }
        }
        return 0;
    }
}

void
TerrainBase::add_to_top(
    const Json::Value& top_data, const std::map<MaterialId, const Material>& materials
) {
    std::set<std::pair<MaterialId, ColorId>> material_type;

    for (auto color_data : top_data["above_colors"]) {
        // element id
        MaterialId E = color_data["E"].asInt();
        if (color_data["C"].isInt()) {
            // color id
            ColorId C = color_data["C"].asInt();
            material_type.insert(std::make_pair(E, C));
        } else if (color_data["C"].asBool()) {
            // add all color ids
            for (ColorId C = 0; C < materials.at(E).color.size(); C++) {
                material_type.insert(std::make_pair(E, C));
            }
        }
    }

    Dim guess = 0;
    // for loop
    for (size_t x = 0; x < X_MAX; x++)
        for (size_t y = 0; y < Y_MAX; y++) {
            // get first (not) z of material
            guess = get_first_not(material_type, x, y, guess);
            // if z is between some bounds
            // stop_h = get stop height (guess, top_data["how_to_add"])
            unsigned int max_height = get_stop_height(guess, top_data["how_to_add"]);
            for (size_t z = guess; z < max_height; z++) {
                get_tile(x, y, z)->set_material(
                    &materials.at(top_data["Material_id"].asInt()),
                    top_data["Color_id"].asInt()
                );
            }
        }
}

void
TerrainBase::stamp_tile_region(
    int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
    const Material* mat,
    const std::set<std::pair<MaterialId, ColorId>>& elements_can_stamp, ColorId color_id
) {
    // set tiles in region to mat and color_id if the current material is in
    // elements_can_stamp.
    for (ssize_t x = x_start; x < x_end; x++) {
        for (ssize_t y = y_start; y < y_end; y++) {
            for (ssize_t z = z_start; z < z_end; z++) {
                if (in_range(x, y, z)) {
                    Tile* tile = get_tile(x, y, z);
                    if (has_tile_material(elements_can_stamp, tile)) {
                        tile->set_material(mat, color_id);
                    }
                }
            }
        }
    }
}

void
TerrainBase::stamp_tile_region(
    int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
    const Material* mat, ColorId color_id
) {
    // set tiles in region to mat and color_id
    for (ssize_t x = x_start; x < x_end; x++) {
        for (ssize_t y = y_start; y < y_end; y++) {
            for (ssize_t z = z_start; z < z_end; z++) {
                if (in_range(x, y, z)) {
                    get_tile(x, y, z)->set_material(mat, color_id);
                }
            }
        }
    }
}

void
TerrainBase::init_area(int area_x, int area_y, generation::LandGenerator gen) {
    while (!gen.empty()) {
        stamp_tile_region(gen.get_this_stamp(), area_x, area_y);
        gen.next();
    }
    gen.reset();
}

// generates a x_map_tiles by y_map_tiles vector of macro tile types.
std::vector<int>
TerrainBase::generate_macro_map(
    unsigned int x_map_tiles, unsigned int y_map_tiles, const Json::Value& terrain_data
) {
    std::vector<int> out;
    int background = terrain_data["BackGround"].asInt(); // default terrain type.
    int numOctaves = terrain_data["NumOctaves"].asInt(); // number of octaves
    double persistance = terrain_data["Persistance"].asDouble();
    int range = terrain_data["Range"].asInt();
    int spacing = terrain_data["Spacing"].asInt();
    out.resize(x_map_tiles * y_map_tiles, background);
    generation::NoiseGenerator ng =
        generation::NoiseGenerator(numOctaves, persistance, 3);

    for (size_t i = 0; i < out.size(); i++) {
        TerrainDim3 tile_position = sop(i, x_map_tiles, y_map_tiles, 1);
        auto p = ng.getValueNoise(
            static_cast<double>(tile_position.x) * spacing,
            static_cast<double>(tile_position.y) * spacing
        );
        out[i] = static_cast<int>((p + 1) * (p + 1) * range);
    }

    // There should be some formatting for map.
    // it is supposed to be size_x by size_y

    return out;
}

int
TerrainBase::get_stop_height(int height, const Json::Value& how_to_add) {
    for (auto& add_data : how_to_add) {
        if (height >= add_data["from"][0].asInt()
            && height < add_data["from"][1].asInt()) {
            if (add_data["to"].isInt()) {
                return add_data["to"].asInt();
            } else {
                return height + add_data["add"].asInt();
            }
        }
    }
    return 0;
}

} // namespace terrain
