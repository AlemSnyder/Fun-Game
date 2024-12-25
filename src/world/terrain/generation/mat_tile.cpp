#include "map_tile.hpp"
#include "util/color.hpp"

namespace terrain {

namespace generation {

TileType::TileType(
    const std::unordered_set<const LandGenerator*> land_generators, MapTile_t tile_type,
    const std::vector<AddToTop>& layer_effect_generators,
    const std::unordered_map<MaterialId, const terrain::material_t> materials
) :
    land_generators_(land_generators),
    tile_type_(tile_type) {
    Dim max_height = 0;

    ColorId stamp_color_id = 0;
    MaterialId stamp_material_id = 0;

    for (auto land_generator : land_generators_) {
        for (auto stamp_generator : *land_generator) {
            Dim stamp_generator_height = stamp_generator->height();

            if (max_height < stamp_generator_height) {
                max_height = stamp_generator_height;

                stamp_color_id = stamp_generator->color();
                stamp_material_id = stamp_generator->material();
            }
        }
    }

    Dim max_layer_effect_height = max_height;

    ColorId layer_effect_color_id = stamp_color_id;
    MaterialId layer_effect_material_id = stamp_material_id;

    for (const auto& layer_effect : layer_effect_generators) {
        Dim layer_effect_height = layer_effect.get_final_height(max_height);
        if (max_layer_effect_height < layer_effect_height) {
            layer_effect_color_id = layer_effect.get_color_id();
            layer_effect_material_id = layer_effect.get_material_id();
        }
    }

    if (layer_effect_material_id == DIRT_ID) {
        layer_effect_color_id = 0; // TODO this is the wrong glass color
    }

    top_color_ = materials.at(stamp_material_id).color[stamp_color_id].hex_color;

    secondary_color_ =
        materials.at(layer_effect_material_id).color[layer_effect_color_id].hex_color;

    if (max_height == max_layer_effect_height) {
        color_t real_color = color::uint32_to_color(secondary_color_);
        constexpr uint8_t reduce_by = 25;
        real_color[0] = real_color[0] > reduce_by ? real_color[0] - reduce_by : 0;
        real_color[1] = real_color[1] > reduce_by ? real_color[1] - reduce_by : 0;
        real_color[2] = real_color[2] > reduce_by ? real_color[2] - reduce_by : 0;

        secondary_color_ = color::color_to_uint32(real_color);
    }

    height_ = max_layer_effect_height;
}

} // namespace generation

} // namespace terrain
