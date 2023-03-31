#include "mesh.hpp"

#include "../constants.hpp"
#include "../terrain/material.hpp"

#include <map>
#include <unordered_map>

namespace entity {

void
Mesh::change_color_indexing(
    const std::map<Material_id_t, const terrain::Material>& materials,
    const std::unordered_map<Color_int_t, uint16_t>& mapping
) {
    for (auto& elem : indexed_color_ids_) {
        Material_id_t material_id = (elem >> 8) & 0xff;
        Color_id_t color_id = elem & 0xff;

        terrain::Material mat = materials.at(material_id);
        Color_int_t color = mat.color.at(color_id).second;

        elem = mapping.at(color);
    }
}

} // namespace entity
