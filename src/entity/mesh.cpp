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

#include "../util/voxelutility.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

namespace entity {

Mesh::Mesh(std::filesystem::path path) {
    load_from_qb_(path.string());
}

void
Mesh::load_from_qb_(std::string path) {
    std::vector<uint32_t> data;
    std::vector<int32_t> center;
    // std::vector<int> size;
    voxel_utility::VoxelObject voxel_object(&path[0]);
    // int test = voxel_utility::from_qb(&path[0], data, center, size_);

    if (!voxel_object.ok()) {
        std::string message("Could not load voxel from ");
        throw std::invalid_argument(message + path);
    }

    // generate_mesh_(data, center, size_);
    generate_mesh_(voxel_object);
}

} // namespace entity
