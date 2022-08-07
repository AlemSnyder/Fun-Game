// -*- lsst-color++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "mesh.hpp"

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <utility>
#include <vector>

#include "../Util/voxelutility.hpp"

namespace Entity{

Mesh::Mesh(std::string path) { load_from_qb_(path); }

void Mesh::load_from_qb_(std::string path) {
    std::vector<uint32_t> data;
    std::vector<int32_t> center;
    // std::vector<int> size;
    VoxelUtility::VoxelObject voxel_object(&path[0]);
    //int test = VoxelUtility::from_qb(&path[0], data, center, size_);

    if (!voxel_object.ok()) {
        std::string message("Could not load voxel from ");
        throw std::invalid_argument(message + path);
    }

    //generate_mesh_(data, center, size_);
    generate_mesh_(voxel_object);
}

void Mesh::generate_mesh_(VoxelUtility::VoxelObject voxel_object){
    generate_mesh(voxel_object,
                  indices_,
                  indexed_vertices_,
                  indexed_colors_,
                  indexed_normals_);
}

int Mesh::get_position_(int x, int y, int z) const {
    return ((x * size_[1] + y) * size_[2] + z);
}

void Mesh::get_mesh(std::vector<unsigned short> &indices,
                    std::vector<glm::vec3> &indexed_vertices,
                    std::vector<glm::vec3> &indexed_colors,
                    std::vector<glm::vec3> &indexed_normals) const {
    indices = indices_;
    indexed_vertices = indexed_vertices_;
    indexed_colors = indexed_colors_;
    indexed_normals = indexed_normals_;
}

} // namespace Entity

