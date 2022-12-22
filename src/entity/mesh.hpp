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
/**
 * @file mesh.hpp
 *
 * @brief Defines Mesh Struct
 *
 * @ingroup entity
 *
 */
#pragma once

#include "../util/voxel_io.hpp"

#include <glm/glm.hpp>

#include <filesystem>
#include <vector>

namespace entity {

/**
 * @brief Holds data that describes now an object is loaded by the shader
 *
 */
struct Mesh {
    Mesh(
        std::vector<uint16_t>& indices, std::vector<glm::ivec3>& indexed_vertices,
        std::vector<glm::vec3>& indexed_colors,
        std::vector<glm::i8vec3>& indexed_normals
    ) :
        indices_(indices),
        indexed_vertices_(indexed_vertices), indexed_colors_(indexed_colors),
        indexed_normals_(indexed_normals)
    {}

    // defines a bounding box of the mesh
    const std::vector<int> size_;
    // defines center of mesh for rotating
    const std::vector<int> center_;

    // indices of vertices drawn (vertices used twice can be ignored)
    const std::vector<std::uint16_t> indices_;
    // position of vertices in mesh space
    const std::vector<glm::ivec3> indexed_vertices_;
    // color of vertex
    const std::vector<glm::vec3> indexed_colors_;
    // normal direction
    const std::vector<glm::i8vec3> indexed_normals_;

}; // struct Mesh

/**
 * @brief Generates a mesh from the given 3D voxel structure
 *
 * @tparam T
 * @param voxel_object
 */
template <class T>
Mesh
generate_mesh(T voxel_object)
{
    std::vector<uint16_t> indices;
    std::vector<glm::ivec3> indexed_vertices;
    std::vector<glm::vec3> indexed_colors;

    std::vector<glm::i8vec3> indexed_normals;
    // mesh off set
    std::vector<int> center = voxel_object.get_offset();
    std::vector<uint32_t> dims = voxel_object.get_size();
    glm::ivec3 offset(center[0], center[1], center[2]);
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // in which directions is the mesh being drawn
        const std::size_t dims_index_1 = (axis + 1) % 3;
        const std::size_t dims_index_2 = (axis + 2) % 3;

        int voxel_position[3] = {0}; // position of a voxel in world space
        int mesh_normal[3] = {0};    // direction that the mesh is not changing in

        // the color of all tiles at the same "level".
        std::vector<std::pair<bool, uint32_t>> color_info(
            dims[dims_index_1] * dims[dims_index_2]
        );

        // Compute color_info
        mesh_normal[axis] = 1;
        // for each layer going in the direction of axis
        for (voxel_position[axis] = -1; voxel_position[axis] < int(dims[axis]);) {
            std::size_t counter = 0;
            // for each voxel in this level
            for (voxel_position[dims_index_2] = 0;
                 voxel_position[dims_index_2] < int(dims[dims_index_2]);
                 ++voxel_position[dims_index_2])
                for (voxel_position[dims_index_1] = 0;
                     voxel_position[dims_index_1] < int(dims[dims_index_1]);
                     ++voxel_position[dims_index_1], ++counter) {
                    // tile in the level above
                    const uint32_t voxel_above = voxel_object.get_voxel(
                        voxel_position[0], voxel_position[1], voxel_position[2]
                    );
                    // tiles in the level below
                    const uint32_t voxel_below = voxel_object.get_voxel(
                        voxel_position[0] + mesh_normal[0],
                        voxel_position[1] + mesh_normal[1],
                        voxel_position[2] + mesh_normal[2]
                    );
                    const bool bool_a = voxel_above;
                    // if both solid or both not solid then don't make
                    // voxel_above face between the voxels
                    if ((voxel_below != 0) == bool_a) {
                        color_info[counter] = std::make_pair(false, 0);
                    }
                    // if voxel_above is solid make face with voxel_above's
                    // color facing up
                    else if (bool_a) {
                        color_info[counter] = std::make_pair(true, voxel_above);
                    }
                    // voxel_below is solid make face with voxel_below's color
                    // facing down
                    else {
                        color_info[counter] = std::make_pair(false, voxel_below);
                    }
                }

            ++voxel_position[axis]; // go to the next level

            // Generate mesh for color_info using lexicographic ordering
            unsigned int width = 0, height = 0;

            counter = 0;
            for (unsigned int j = 0; j < dims[dims_index_2]; ++j)
                for (unsigned int i = 0; i < dims[dims_index_1];) {
                    // color and direction of the face between two voxels
                    std::pair<bool, uint32_t> color = color_info[counter];
                    // if there is a face between two voxels
                    if (color.second) {
                        // find the size of all faces that have the same color
                        // and direction like this: first expands over the
                        // width, then expanding over the height

                        // clang-format off
// c c c c c n n n     # c c c c n n n     # # # # #[n]n n     # # # # # n n n
// c c c c c c n n     c c c c c c n n     c c c c c c n n     # # # # # c n n
// c c c c c c c n - > c c c c c c c n - > c c c c c c c n - > # # # # # c c n
// c c c c c n n n     c c c c c n n n     c c c c c n n n     # # # # # n n n
// c c n c c n c c     c c n c c n c c     c c n c c n c c     c c[n]c c n c c
                        // clang-format on

                        // Compute width
                        width = 1;
                        while (color == color_info[counter + width]
                               && i + width < dims[dims_index_1]) {
                            ++width;
                        }
                        // Compute height
                        bool done = false;
                        for (height = 1; j + height < dims[dims_index_2]; ++height) {
                            // expand until one of the tiles in the next row is
                            // not the same color/facing direction
                            for (unsigned int k = 0; k < width; ++k) {
                                // if the direction and color of the original
                                // face is different from the direction and
                                // color of the face currently being tested
                                std::pair<bool, uint32_t> test_against = color_info
                                    [counter + k + height * dims[dims_index_1]];
                                if (color != test_against) {
                                    done = true;
                                    break;
                                }
                            }
                            if (done)
                                break;
                        }

                        // Add quad
                        voxel_position[dims_index_1] = i;
                        voxel_position[dims_index_2] = j;

                        int offset_1[3] = {0};
                        int offset_2[3] = {0};

                        // depending on the normal direction set the width, and
                        // height
                        if (color.first) {
                            offset_2[dims_index_2] = height;
                            offset_1[dims_index_1] = width;
                        } else {
                            offset_1[dims_index_2] = height;
                            offset_2[dims_index_1] = width;
                        }

                        const std::size_t vertex_size = indexed_vertices.size();

                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0], voxel_position[1], voxel_position[2]
                            )
                            + offset
                        );
                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0] + offset_1[0],
                                voxel_position[1] + offset_1[1],
                                voxel_position[2] + offset_1[2]
                            )
                            + offset
                        );
                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0] + offset_1[0] + offset_2[0],
                                voxel_position[1] + offset_1[1] + offset_2[1],
                                voxel_position[2] + offset_1[2] + offset_2[2]
                            )
                            + offset
                        );
                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0] + offset_2[0],
                                voxel_position[1] + offset_2[1],
                                voxel_position[2] + offset_2[2]
                            )
                            + offset
                        );

                        uint32_t int_color = color.second;
                        uint32_t red = (int_color >> 24) & 0xFF;
                        uint32_t green = (int_color >> 16) & 0xFF;
                        uint32_t blue = (int_color >> 8) & 0xFF;
                        // the last one >> 0 is A
                        glm::vec3 vector_color(
                            red / 255.0, green / 255.0, blue / 255.0
                        );
                        // how many corners on a square are there?
                        for (size_t voxel_position = 0; voxel_position < 4;
                             voxel_position++) {
                            indexed_colors.push_back(vector_color);
                        }

                        glm::i8vec3 triangle_normal =
                            glm::ivec3(glm::normalize(glm::cross(glm::vec3(
                                indexed_vertices[vertex_size]
                                    - indexed_vertices[vertex_size + 1]),
                                glm::vec3(indexed_vertices[vertex_size]
                                    - indexed_vertices[vertex_size + 2])
                            )));
                        // how many corners on a square are there?
                        for (size_t voxel_position = 0; voxel_position < 4;
                             voxel_position++) {
                            indexed_normals.push_back(triangle_normal);
                        }
                        // the first half of the rectangle
                        indices.push_back(vertex_size);
                        indices.push_back(vertex_size + 1);
                        indices.push_back(vertex_size + 2);
                        // the second half of the rectangle
                        indices.push_back(vertex_size + 2);
                        indices.push_back(vertex_size + 3);
                        indices.push_back(vertex_size);

                        for (unsigned int w = 0; w < width; ++w)
                            for (unsigned int h = 0; h < height; ++h)
                                color_info[counter + w + h * dims[dims_index_1]] =
                                    std::make_pair(false, 0);

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
    return Mesh(indices, indexed_vertices, indexed_colors, indexed_normals);
}

} // namespace entity
