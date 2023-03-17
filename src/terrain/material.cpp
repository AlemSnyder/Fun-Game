
#include "material.hpp"

#include "../logging.hpp"

namespace terrain {

// color id to color for all materials
std::vector<uint32_t> TerrainColorMapping::color_ids_map;
// 8 bit color to color id
std::unordered_map<uint32_t, uint16_t> TerrainColorMapping::colors_inverse_map;

void
TerrainColorMapping::assign_color_mapping(const std::map<uint8_t, const Material>* materials
) {
    color_ids_map.clear();
    colors_inverse_map.clear();

    // TODO @AlemSnyder
    // GetIntegerv(GL_MAX_TEXTURE_SIZE, *mas_texture_size)
    // The above size is probably a better upper bound as it comes from the
    // texture size.
    // max_size = min((1U << 15), mas_texture_size);
    // but it if fix this then I should probably switch from a 1D texture
    // to a 2D texture, and that seems like a lot of work.

    if (materials->size() > (1U << 15)) {
        LOG_WARNING(
            logging::terrain_logger,
            "Too many colors in material file ({}). Only using first {}",
            materials->size(), (1U << 15)
        );
    }

    // empty should always have index 0;
    color_ids_map.push_back(0);
    colors_inverse_map[0] = 0;
    for (auto const& [id, material] : *materials) {
        for (auto color_data : material.color) {
            uint32_t color = color_data.second;
            if (std::find(color_ids_map.begin(), color_ids_map.end(), color)
                == color_ids_map.end()) {
                // voxel_colors[i] is not in colors
                // we should add it to colors
                // the index is the length of colors because it will be appended
                uint16_t j = color_ids_map.size();
                // add the color
                color_ids_map.push_back(color);
                // add to inverse colors
                colors_inverse_map[color] = j;
                if (color_ids_map.size() > (1U << 15) - 1)
                    return;
            }
        }
    }
}
} // namespace terrain
