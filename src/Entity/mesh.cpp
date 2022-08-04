// -*- lsst-c++ -*-
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

int Mesh::get_position_(int x, int y, int z) const {
    return ((x * size_[1] + y ) * size_[2] + z);
}

void Mesh::generate_mesh_(std::vector<uint32_t> data,
                       std::vector<int32_t> center,
                       std::vector<int> dims){
    glm::vec3 glm_vector(center[0], center[1], center[2]);
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // printf("axis: %ld\n", axis);

        const std::size_t u = (axis + 1) % 3;
        const std::size_t v = (axis + 2) % 3;

        // printf("u: %ld, v: %ld\n", u, v);

        int x[3] = {0};
        int q[3] = {0};
        std::vector<std::pair<bool, uint32_t>> mask(dims[u] * dims[v]);
        // printf("dims[u]: %d, dims[v]: %d\n", dims[u], dims[v]);

        // printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
        // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

        // Compute mask
        q[axis] = 1;
        for (x[axis] = -1; x[axis] < dims[axis];) {
            // printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
            // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

            std::size_t counter = 0;
            for (x[v] = 0; x[v] < dims[v]; ++x[v])
                for (x[u] = 0; x[u] < dims[u]; ++x[u], ++counter) {
                    const uint32_t a = x[axis] >= 0 
                                ? data[get_position_(x[0], x[1], x[2])]
                                : 0;
                    const uint32_t b = x[axis] < dims[axis] - 1
                                ? data[get_position_(x[0] + q[0], x[1] + q[1], x[2] + q[2])]
                                : 0;
                    const bool ba = a;
                    if (b == ba) // if both solid or both not solid
                        mask[counter] = std::make_pair(false, 0);
                    else if (ba)
                        mask[counter] = std::make_pair(true, a);
                    else
                        mask[counter] = std::make_pair(false, b);
                }

            ++x[axis];

            // Generate mesh for mask using lexicographic ordering
            int width = 0, height = 0;

            counter = 0;
            for (int j = 0; j < dims[v]; ++j)
            for (int i = 0; i < dims[u];) {
                std::pair<bool, uint32_t> c = mask[counter]; //color
                if (c.second) {
                    // Compute width
                    width = 1;
                    while (c == mask[counter + width] && i + width < dims[u]) {
                        ++width;
                    }

                    // Compute height
                    bool done = false;
                    for (height = 1; j + height < dims[v]; ++height) {
                        for (int k = 0; k < width; ++k)
                            if (c != mask[counter + k + height * dims[u]]) {
                                done = true;
                                break;
                            }

                        if (done) break;
                    }

                    // Add quad
                    x[u] = i;
                    x[v] = j;

                    int du[3] = {0}, dv[3] = {0};

                    if (c.first) {
                        dv[v] = height;
                        du[u] = width;
                    } else {
                        du[v] = height;
                        dv[u] = width;
                    }

                    const std::size_t vertex_size = indexed_vertices_.size();

                    indexed_vertices_.push_back(
                        glm::vec3(x[0], x[1], x[2])+glm_vector);
                    indexed_vertices_.push_back(glm::vec3(
                        x[0] + du[0], x[1] + du[1], x[2] + du[2])
                        + glm_vector);
                    indexed_vertices_.push_back(glm::vec3(
                        x[0] + du[0] + dv[0], x[1] + du[1] + dv[1], x[2] + du[2] + dv[2])
                        + glm_vector);
                    indexed_vertices_.push_back(glm::vec3(
                        x[0] + dv[0], x[1] + dv[1], x[2] + dv[2])
                        + glm_vector);

                    uint32_t int_color = c.second;
                    uint32_t red = ( int_color >> 24 ) & 0xFF;
                    uint32_t green = ( int_color >> 16 ) & 0xFF;
                    uint32_t blue = ( int_color >> 8 ) & 0xFF;
                    // the last one >> 0 is A
                    glm::vec3 vector_color( red / 255.0,
                                          green / 255.0,
                                           blue / 255.0);
                    for (size_t x = 0; x < 4; x++){ // how many corners on a square are there?
                        indexed_colors_.push_back(vector_color);
                    }

                    glm::vec3 triangle_normal = glm::normalize(
                                           glm::cross(indexed_vertices_[vertex_size]
                                                        - indexed_vertices_[vertex_size+1],
                                                      indexed_vertices_[vertex_size]
                                                        - indexed_vertices_[vertex_size+2]));
                    for (size_t x = 0; x < 4; x++){ // how many corners on a square are there?
                        indexed_normals_.push_back(triangle_normal);
                    }

                    indices_.push_back(vertex_size);
                    indices_.push_back(vertex_size+1);
                    indices_.push_back(vertex_size+2);
                    indices_.push_back(vertex_size+2);
                    indices_.push_back(vertex_size+3);
                    indices_.push_back(vertex_size);

                    for (int b = 0; b < width; ++b)
                        for (int a = 0; a < height; ++a)
                            mask[counter + b + a * dims[u]] = std::make_pair(false, 0);

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

