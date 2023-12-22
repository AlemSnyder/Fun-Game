#include "terrain_base.hpp"

#include "../logging.hpp"
#include "../types.hpp"
#include "generation/biome.hpp"
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

TerrainBase::TerrainBase(
    Dim x_map_tiles, Dim y_map_tiles, Dim area_size_, Dim z,
    const generation::Biome& biome, const generation::TerrainMacroMap& macro_map
) :
    area_size_(area_size_),
    biome_(biome), X_MAX(x_map_tiles * area_size_), Y_MAX(y_map_tiles * area_size_),
    Z_MAX(z) {
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);
    for (size_t xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        tiles_.push_back(Tile(sop(xyz), biome_.get_material(0), 0));
    }

    // srand(seed);
    LOG_INFO(logging::terrain_logger, "Start of land generator.");

    // TODO make this faster 4
    for (size_t i = 0; i < x_map_tiles; i++)
        for (size_t j = 0; j < y_map_tiles; j++) {
            generation::MapTile map_tile = macro_map.get_tile(i, j);
            auto macro_types = biome_.get_macro_ids(map_tile.get_tile_type());
            for (auto generator_macro : macro_types) {
                init_area(map_tile, biome_.get_generator(generator_macro));
            }
        }

    LOG_INFO(logging::terrain_logger, "End of land generator: place tiles.");

    // TODO make this faster 3
    for (const generation::AddToTop& top_data : biome.get_top_generators()) {
        add_to_top(top_data);
    }

    LOG_INFO(logging::terrain_logger, "End of land generator: top layer placement.");
}

TerrainBase::TerrainBase(
    const generation::Biome& biome, voxel_utility::qb_data_t data
) :
    area_size_(32),
    biome_(biome), X_MAX(data.size.x), Y_MAX(data.size.y), Z_MAX(data.size.z) {
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    auto materials_inverse = biome.get_colors_inverse_map();

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

Dim
TerrainBase::get_first_not(const MaterialGroup& materials, Dim x, Dim y, Dim guess)
    const {
    if (guess < 1) {
        guess = 1;
    } else if (static_cast<Dim>(guess) >= Z_MAX) {
        guess = Z_MAX - 1;
    }
    if (has_tile_material(materials, x, y, guess - 1)) {
        if (!has_tile_material(materials, x, y, guess)) {
            return guess;
        } else {
            // go up
            for (Dim z = guess + 1; z < Z_MAX; z++) {
                if (!has_tile_material(materials, x, y, z)) {
                    return z;
                }
            }
            return Z_MAX; // -1? should not be minus one, but one should
                          // consider that this is a possible return value
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
TerrainBase::add_to_top(const generation::AddToTop& top_data) {
    Dim guess = Z_MAX / 2;
    // for loop
    for (size_t x = 0; x < X_MAX; x++)
        for (size_t y = 0; y < Y_MAX; y++) {
            // get first (not) z of material
            guess = get_first_not(top_data.get_elements_above(), x, y, guess);
            // if z is between some bounds
            // stop_h = get stop height (guess, top_data["how_to_add"])
            Dim max_height = top_data.get_final_height(guess);
            for (size_t z = guess; z < max_height; z++) {
                const Tile& tile = *get_tile(x, y, z);
                if (top_data.can_overwrite_material(
                        tile.get_material_id(), tile.get_color_id()
                    )) {
                    get_tile(x, y, z)->set_material(
                        biome_.get_material(top_data.get_material_id()),
                        top_data.get_color_id()
                    );
                }
            }
        }
}

void
TerrainBase::stamp_tile_region(
    const generation::TileStamp& stamp, TerrainOffset x_offset = 0,
    TerrainOffset y_offset = 0
) {
    // set tiles in region to mat and color_id if the current material is in
    // elements_can_stamp.
    TerrainOffset x_start = stamp.x_start + x_offset * area_size_ + area_size_ / 2;
    TerrainOffset y_start = stamp.y_start + y_offset * area_size_ + area_size_ / 2;
    TerrainOffset x_end = stamp.x_end + x_offset * area_size_ + area_size_ / 2;
    TerrainOffset y_end = stamp.y_end + y_offset * area_size_ + area_size_ / 2;

    for (TerrainOffset x = x_start; x < x_end; x++) {
        for (TerrainOffset y = y_start; y < y_end; y++) {
            for (TerrainOffset z = stamp.z_start; z < stamp.z_end; z++) {
                if (in_range(x, y, z)) {
                    Tile* tile = get_tile(x, y, z);
                    if (stamp.elements_can_stamp.has_value()) {
                        if (stamp.elements_can_stamp.value().material_in(
                                tile->get_material_id(), tile->get_color_id()
                            ))
                            tile->set_material(
                                biome_.get_material(stamp.mat), stamp.color_id
                            );
                    } else {
                        tile->set_material(
                            biome_.get_material(stamp.mat), stamp.color_id
                        );
                    }
                }
            }
        }
    }
}

void
TerrainBase::init_area(generation::MapTile& map_tile, generation::LandGenerator gen) {
    while (!gen.empty()) {
        stamp_tile_region(
            gen.get_stamp(map_tile.get_rand_engine()), map_tile.get_x(),
            map_tile.get_y()
        );
        gen.next();
    }
    gen.reset();
}

Dim
TerrainBase::get_stop_height(Dim height, const Json::Value& how_to_add) {
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
