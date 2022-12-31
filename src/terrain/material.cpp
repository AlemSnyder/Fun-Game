#include "material.hpp"

namespace terrain {

std::vector<uint32_t> Terrain_Color_Mapping::color_ids_map;
std::unordered_map<uint32_t, uint16_t> Terrain_Color_Mapping::colors_inverse_map;

void
Terrain_Color_Mapping::assign_color_mapping(const std::map<int, const Material>* materials) {
    color_ids_map.clear();
    colors_inverse_map.clear();

    // empty should always have index 0;
    color_ids_map.push_back(0);
    colors_inverse_map[0] = 0;
    for (auto const& [a, b] : *materials) {
        for (auto color_data : b.color) {
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
            }
        }
    }
    if (color_ids_map.size() > (1U << 15)) {
        LOG_ERROR(
            logging::terrain_logger, "Too many colors in material file: {}",
            color_ids_map.size()
        );
    }
}
} // namespace terrain
