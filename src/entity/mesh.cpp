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

} // namespace entity
