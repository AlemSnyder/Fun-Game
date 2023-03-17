#include "mesh.hpp"

#include "../terrain/material.hpp"

#include <map>
#include <unordered_map>

namespace entity {

void
Mesh::change_color_indexing(
    std::map<uint8_t, const terrain::Material> materials,
    std::unordered_map<uint32_t, uint16_t> mapping
) {
    for (auto& elem: indexed_color_ids_) {
        uint8_t material_id = elem >> 8;
        uint8_t color_id = elem;

        elem =
            mapping.at(materials.at(material_id).color.at(color_id).second);
    }
}

std::vector<std::array<float, 4>>
convert_color_data(const std::vector<uint32_t> color_map){

    std::vector<std::array<float, 4>> float_colors;
    for (uint32_t int_color : color_map) {
        uint32_t red = (int_color >> 24) & 0xFF;
        uint32_t green = (int_color >> 16) & 0xFF;
        uint32_t blue = (int_color >> 8) & 0xFF;
        uint32_t alpha = (int_color)&0xFF;
        // the last one >> 0 is A
        std::array<float, 4> vector_color(
            {red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f}
        );
        float_colors.push_back(vector_color);
    }
    return float_colors;
}

} // namespace entity
