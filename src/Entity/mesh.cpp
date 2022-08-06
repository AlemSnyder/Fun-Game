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

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <glm/glm.hpp>

#include "../Util/voxelutility.hpp"
#include "mesh.hpp"

Mesh::Mesh(std::string path){
    load_from_qb_(path);
}

void Mesh::load_from_qb_(std::string path){
    std::vector<uint32_t> data;
    std::vector<int32_t>center;
    //std::vector<int> size;
    int test = VoxelUtility::from_qb(&path[0], data, center, size_);

    if (!test){
        std::string message("Could not load voxel from ");
        throw std::invalid_argument(message + path);
    }

    generate_mesh_(data, center, size_);

}

int Mesh::get_position_(int mesh_corner, int y, int z) const {
    return ((mesh_corner * size_[1] + y ) * size_[2] + z);
}

void Mesh::generate_mesh_(std::vector<uint32_t> data,
                       std::vector<int32_t> center,
                       std::vector<int> dims){
    glm::vec3 glm_vector(center[0], center[1], center[2]);
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // printf("axis: %ld\n", axis);

        const std::size_t dims_index_1 = (axis + 1) % 3;
        const std::size_t dims_index_2 = (axis + 2) % 3;

        // printf("dims_index_1: %ld, dims_index_2: %ld\n", dims_index_1, dims_index_2);

        int mesh_corner[3] = {0}; // position of a corner in world space
        int mesh_size[3] = {0}; // size of mesh in each direction
        // the color of all tiles at the same "level".
        std::vector<std::pair<bool, uint32_t>> color_info(dims[dims_index_1] * dims[dims_index_2]);
        // printf("dims[dims_index_1]: %d, dims[dims_index_2]: %d\n", dims[dims_index_1], dims[dims_index_2]);

        // printf("mesh_corner: %d, %d, %d\n", mesh_corner[0], mesh_corner[1], mesh_corner[2]);
        // printf("mesh_size: %d, %d, %d\n", mesh_size[0], mesh_size[1], mesh_size[2]);

        // Compute color_info
        mesh_size[axis] = 1;
        for (mesh_corner[axis] = -1; mesh_corner[axis] < dims[axis];) {
            // printf("mesh_corner: %d, %d, %d\n", mesh_corner[0], mesh_corner[1], mesh_corner[2]);
            // printf("mesh_size: %d, %d, %d\n", mesh_size[0], mesh_size[1], mesh_size[2]);

            std::size_t counter = 0;
            for (mesh_corner[dims_index_2] = 0; mesh_corner[dims_index_2] < dims[dims_index_2]; ++mesh_corner[dims_index_2])
                for (mesh_corner[dims_index_1] = 0; mesh_corner[dims_index_1] < dims[dims_index_1]; ++mesh_corner[dims_index_1], ++counter) {
                    const uint32_t a = mesh_corner[axis] >= 0 
                                ? data[get_position_(mesh_corner[0], mesh_corner[1], mesh_corner[2])]
                                : 0;
                    const uint32_t b = mesh_corner[axis] < dims[axis] - 1
                                ? data[get_position_(mesh_corner[0] + mesh_size[0], mesh_corner[1] + mesh_size[1], mesh_corner[2] + mesh_size[2])]
                                : 0;
                    const bool ba = a;
                    if (b == ba) // if both solid or both not solid
                        color_info[counter] = std::make_pair(false, 0);
                    else if (ba)
                        color_info[counter] = std::make_pair(true, a);
                    else
                        color_info[counter] = std::make_pair(false, b);
                }

            ++mesh_corner[axis];

            // Generate mesh for color_info using lexicographic ordering
            int width = 0, height = 0;

            counter = 0;
            for (int i = 0; i < dims[dims_index_1];)
            for (int j = 0; j < dims[dims_index_2]; ++j) {
                std::pair<bool, uint32_t> color = color_info[counter]; //color
                if (color.second) {
                    // Compute width
                    width = 1;
                    while (color == color_info[counter + width] && i + width < dims[dims_index_1]) {
                        ++width;
                    }

                    // Compute height
                    bool done = false;
                    for (height = 1; j + height < dims[dims_index_2]; ++height) {
                        // expand until one of the tiles in the next row is not the same color/facing direction
                        for (int k = 0; k < width; ++k)
                            // if the direction and color of this face is different from the direction and color of the final square
                            if (color != color_info[counter + k + height * dims[dims_index_1]]) {
                                done = true;
                                break;
                            }

                        if (done) break;
                    }

                    // Add quad
                    mesh_corner[dims_index_1] = i;
                    mesh_corner[dims_index_2] = j;

                    int off_set_1[3] = {0};
                    int off_set_2[3] = {0};

                    if (color.first) { // depending on the normal direction
                        off_set_2[dims_index_2] = height;
                        off_set_1[dims_index_1] = width;
                    } else {
                        off_set_1[dims_index_2] = height;
                        off_set_2[dims_index_1] = width;
                    }

                    const std::size_t vertex_size = indexed_vertices_.size();

                    indexed_vertices_.push_back(
                        glm::vec3(mesh_corner[0], mesh_corner[1], mesh_corner[2])+glm_vector);
                    indexed_vertices_.push_back(glm::vec3(
                        mesh_corner[0] + off_set_1[0], mesh_corner[1] + off_set_1[1], mesh_corner[2] + off_set_1[2])
                        + glm_vector);
                    indexed_vertices_.push_back(glm::vec3(
                        mesh_corner[0] + off_set_1[0] + off_set_2[0], mesh_corner[1] + off_set_1[1] + off_set_2[1], mesh_corner[2] + off_set_1[2] + off_set_2[2])
                        + glm_vector);
                    indexed_vertices_.push_back(glm::vec3(
                        mesh_corner[0] + off_set_2[0], mesh_corner[1] + off_set_2[1], mesh_corner[2] + off_set_2[2])
                        + glm_vector);

                    uint32_t int_color = color.second;
                    uint32_t red = ( int_color >> 24 ) & 0xFF;
                    uint32_t green = ( int_color >> 16 ) & 0xFF;
                    uint32_t blue = ( int_color >> 8 ) & 0xFF;
                    // the last one >> 0 is A
                    glm::vec3 vector_color( red / 255.0,
                                          green / 255.0,
                                           blue / 255.0);
                    for (size_t mesh_corner = 0; mesh_corner < 4; mesh_corner++){ // how many corners on a square are there?
                        indexed_colors_.push_back(vector_color);
                    }

                    glm::vec3 triangle_normal = glm::normalize(
                                           glm::cross(indexed_vertices_[vertex_size]
                                                        - indexed_vertices_[vertex_size+1],
                                                      indexed_vertices_[vertex_size]
                                                        - indexed_vertices_[vertex_size+2]));
                    for (size_t mesh_corner = 0; mesh_corner < 4; mesh_corner++){ // how many corners on a square are there?
                        indexed_normals_.push_back(triangle_normal);
                    }

                    indices_.push_back(vertex_size);
                    indices_.push_back(vertex_size+1);
                    indices_.push_back(vertex_size+2);
                    indices_.push_back(vertex_size+2);
                    indices_.push_back(vertex_size+3);
                    indices_.push_back(vertex_size);

                    for (int w = 0; w < width; ++w)
                        for (int h = 0; h < height; ++h)
                            color_info[counter + w + h * dims[dims_index_1]] = std::make_pair(false, 0);

                    // Increment counters
                    i += width;
                    counter += width;
                } else {
                    ++i;
                    ++counter;
                }
            }
        }
    }
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

