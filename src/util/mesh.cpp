#include "mesh.hpp"

#include "types.hpp"
#include "world/terrain/material.hpp"

#include <map>
#include <unordered_map>

namespace util {

void
Mesh::change_color_indexing(
    const std::unordered_map<MaterialId, const terrain::material_t>& materials,
    const std::unordered_map<ColorInt, uint16_t>& mapping
) {
    for (auto& elem : indexed_color_ids_) {
        MaterialId material_id = (elem >> 8) & 0xff;
        ColorId color_id = elem & 0xff;

        terrain::material_t mat = materials.at(material_id);
        ColorInt color = mat.color.at(color_id).hex_color;

        elem = mapping.at(color);
    }
}

} // namespace util
