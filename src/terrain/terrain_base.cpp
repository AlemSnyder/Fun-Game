#include "terrain_base.hpp"

#include "../logging.hpp"
#include "terrain_generation/noise.hpp"

// move add_to_top

namespace terrain {

void
TerrainBase::qb_read(
    std::vector<uint32_t> data,
    const std::map<uint32_t, std::pair<const Material*, uint8_t>>& materials_inverse
) {
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    std::set<uint32_t> unknown_materials;

    for (uint32_t xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        auto [x, y, z] = sop(xyz);
        uint32_t color = data[xyz];
        if (color == 0) {                             // if the qb voxel is transparent.
            auto mat_color = materials_inverse.at(0); // set the materials to air
            tiles_.push_back(Tile({x, y, z}, mat_color.first, mat_color.second));
        } else if (materials_inverse.count(color)) { // if the color is known
            auto mat_color = materials_inverse.at(color);
            tiles_.push_back(Tile({x, y, z}, mat_color.first, mat_color.second));
        } else { // the color is unknown
            unknown_materials.insert(color);
            auto mat_color = materials_inverse.at(0); // else set to air.
            tiles_.push_back(Tile({x, y, z}, mat_color.first, mat_color.second));
        }
    }

    for (uint32_t color : unknown_materials) {
        LOG_WARNING(logging::terrain_logger, "Cannot find color: {:x}", color);
    }
}

int
TerrainBase::get_first_not(
    const std::set<std::pair<int, int>> materials, int x, int y, int guess
) const {
    if (guess < 1) {
        guess = 1;
    } else if ((uint32_t)guess >= Z_MAX) {
        guess = Z_MAX - 1;
    }
    if (materials.find(std::make_pair(
            get_tile(x, y, guess - 1)->get_material_id(),
            get_tile(x, y, guess - 1)->get_color_id()
        ))
        != materials.end()) {
        if (materials.find(std::make_pair(
                get_tile(x, y, guess)->get_material_id(),
                get_tile(x, y, guess)->get_color_id()
            ))
            == materials.end()) {
            return guess;
        } else {
            // go up
            for (uint16_t z = guess + 1; z < Z_MAX; z++) {
                if (materials.find(std::make_pair(
                        get_tile(x, y, z)->get_material_id(),
                        get_tile(x, y, z)->get_color_id()
                    ))
                    == materials.end()) {
                    return z;
                }
            }
            return Z_MAX; // -1?
        }
    } else {
        // go down
        for (uint16_t z = guess - 2; z > 0; z--) {
            if (materials.find(std::make_pair(
                    get_tile(x, y, z)->get_material_id(),
                    get_tile(x, y, z)->get_color_id()
                ))
                != materials.end()) {
                return z + 1;
            }
        }
        return 0;
    }
}

void
TerrainBase::add_to_top(
    const Json::Value& top_data, const std::map<uint8_t, const Material>& materials
) {
    std::set<std::pair<int, int>> material_type;

    for (auto color_data : top_data["above_colors"]) {
        int E = color_data["E"].asInt();
        if (color_data["C"].isInt()) {
            int C = color_data["C"].asInt();
            material_type.insert(std::make_pair(E, C));
        } else if (color_data["C"].asBool()) {
            for (unsigned int C = 0; C < materials.at(E).color.size(); C++) {
                material_type.insert(std::make_pair(E, C));
            }
        }
    }

    uint16_t guess = 0;
    // for loop
    for (uint16_t x = 0; x < X_MAX; x++)
        for (uint16_t y = 0; y < Y_MAX; y++) {
            // get first (not) z of material
            guess = get_first_not(material_type, x, y, guess);
            // if z is between some bounds
            // stop_h = get stop height (guess, top_data["how_to_add"])
            int max_height = get_stop_height(guess, top_data["how_to_add"]);
            for (int z = guess; z < max_height; z++) {
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
    const Material* mat, std::set<std::pair<int, int>> elements_can_stamp,
    uint8_t color_id
) {
    // set tiles in region to mat and color_id if the current material is in
    // elements_can_stamp.
    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            for (int z = z_start; z < z_end; z++) {
                if (in_range(x, y, z)) {
                    Tile* tile = get_tile(x, y, z);
                    if (elements_can_stamp.find(std::make_pair(
                            static_cast<int>(tile->get_material_id()),
                            static_cast<int>(tile->get_color_id())
                        ))
                        != elements_can_stamp.end()) {
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
    const Material* mat, uint8_t color_id
) {
    // set tiles in region to mat and color_id
    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            for (int z = z_start; z < z_end; z++) {
                if (in_range(x, y, z)) {
                    get_tile(x, y, z)->set_material(mat, color_id);
                }
            }
        }
    }
}

void
TerrainBase::init_area(int area_x, int area_y, terrain_generation::LandGenerator gen) {
    while (!gen.empty()) {
        stamp_tile_region(gen.get_this_stamp(), area_x, area_y);
        ++gen;
    }
    gen.reset();
}

// generates a size_x by size_y vector of macro tile types.
std::vector<int>
TerrainBase::generate_macro_map(
    unsigned int size_x, unsigned int size_y, Json::Value& terrain_data
) {
    std::vector<int> out;
    int background = terrain_data["BackGround"].asInt(); // default terrain type.
    int numOctaves = terrain_data["NumOctaves"].asInt(); // number of octaves
    double persistance = terrain_data["Persistance"].asDouble();
    int range = terrain_data["Range"].asInt();
    int spacing = terrain_data["Spacing"].asInt();
    out.resize(size_x * size_y, background);
    terrain_generation::NoiseGenerator ng =
        terrain_generation::NoiseGenerator(numOctaves, persistance, 3);

    for (size_t i = 0; i < out.size(); i++) {
        auto [x, y, z] = sop(i, size_x, size_y, 1);
        auto p = ng.getValueNoise(
            static_cast<double>(x) * spacing, static_cast<double>(y) * spacing
        );
        out[i] = static_cast<int>((p + 1) * (p + 1) * range);
    }

    // There should be some formatting for map.
    // it is supposed to be size_x by size_y
    LOG_INFO(logging::terrain_logger, "Map: {}", out);

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
