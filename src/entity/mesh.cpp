#include "mesh.hpp"
#include "../terrain/material.hpp"
#include <map>
#include <unordered_map>

namespace entity {

void Mesh::change_color_indexing(std::map<int, const terrain::Material> materials, std::unordered_map<uint32_t, uint16_t> mapping){
    for (size_t i = 0; i < indexed_color_ids_.size(); i++){
        uint8_t material_id = indexed_color_ids_[i] >> 8;
        uint8_t color_id = indexed_color_ids_[i];

        indexed_color_ids_[i] = mapping.at(materials.at(material_id).color.at(color_id).second);
    }
}

}
