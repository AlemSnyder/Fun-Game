#include "terrain_map.hpp"

#include "util/color.hpp"

namespace terrain {

namespace generation {

std::array<png_byte, 3>
TerrainMapRepresentation::get_color(size_t i, size_t j) const {
    size_t index_i = i / tile_size_in_pixels_;
    size_t index_j = j / tile_size_in_pixels_;

    size_t mod_i = i % tile_size_in_pixels_;
    size_t mod_j = j % tile_size_in_pixels_;

    const TileType& tile = terrain_map_.get_tile(index_i, index_j).get_type();

    ColorInt color;

    if (index_i == 0 || index_j == 0) {
        color = tile.get_top_color();
    } else if (mod_i < tile_border_size_ || mod_j < tile_border_size_) {
        const TileType* adjacent_tile;
        if (mod_i < tile_border_size_ && mod_j < tile_border_size_) {
            adjacent_tile = &terrain_map_.get_tile(index_i - 1, index_j - 1).get_type();

        } else if (mod_i < tile_border_size_ && !(mod_j < tile_border_size_)) {
            adjacent_tile = &terrain_map_.get_tile(index_i - 1, index_j).get_type();
        } else {
            adjacent_tile = &terrain_map_.get_tile(index_i, index_j - 1).get_type();
        }

        if (adjacent_tile->get_height() <= tile.get_height()) {
            color = tile.get_top_color();

        } else {
            color = tile.get_secondary_color();
        }

    } else {
        color = tile.get_top_color();
    }

    color_t color_vec = color::uint32_to_color(color);

    return {color_vec[0], color_vec[1], color_vec[2]};
}

} // namespace generation

} // namespace terrain
