#include "mesh.hpp"

#include "../terrain/material.hpp"
#include "types.hpp"

#include <map>
#include <unordered_map>

namespace entity {

void
Mesh::change_color_indexing(
    const std::map<MaterialId, const terrain::Material>& materials,
    const std::unordered_map<ColorInt, uint16_t>& mapping
) {
    for (auto& elem : indexed_color_ids_) {
        MaterialId material_id = (elem >> 8) & 0xff;
        ColorId color_id = elem & 0xff;

        terrain::Material mat = materials.at(material_id);
        ColorInt color = mat.color.at(color_id).second;

        elem = mapping.at(color);
    }
}

} // namespace entity
