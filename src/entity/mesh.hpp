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

#include "../terrain/material.hpp"
#include "../types.hpp"
#include "../util/color.hpp"
#include "../util/voxel.hpp"

#include <glm/glm.hpp>

#include <filesystem>
#include <map>
#include <unordered_map>
#include <vector>

namespace entity {

/**
 * @brief Holds data that describes now an object is loaded by the shader
 *
 */
class Mesh {
 public:
    Mesh(
        const std::vector<uint16_t>& indices,
        const std::vector<glm::ivec3>& indexed_vertices,
        const std::vector<uint16_t>& indexed_color_ids,
        const std::vector<glm::i8vec3>& indexed_normals,
        const std::vector<ColorInt>& color_map,
        glm::ivec3 size,
        glm::ivec3 center
    ) :
        size_(size), center_(center),
        indices_(indices),
        indexed_vertices_(indexed_vertices), indexed_color_ids_(indexed_color_ids),
        indexed_normals_(indexed_normals), color_map_(color_map) {}

    // friend World;

 protected:
    // x, y, z length of the mesh
    glm::ivec3 size_;
    // defines center of mesh for rotating
    glm::ivec3 center_;
    // indices of each vertex that is drawn
    std::vector<std::uint16_t> indices_;
    // position of vertices in mesh space
    std::vector<glm::ivec3> indexed_vertices_;
    // color of vertex
    std::vector<uint16_t> indexed_color_ids_;
    // normal direction
    std::vector<glm::i8vec3> indexed_normals_;
    // color map
    std::vector<ColorInt> color_map_;

 public:
    // x, y, z length of the mesh
    [[nodiscard]] inline const glm::ivec3&
    get_size() const noexcept {
        return size_;
    }

    // center of mesh
    [[nodiscard]] inline const glm::ivec3&
    get_center() const noexcept {
        return center_;
    }

    // indices of each vertex that is drawn
    [[nodiscard]] inline const std::vector<std::uint16_t>&
    get_indices() const noexcept {
        return indices_;
    }

    // position of vertices in mesh space
    [[nodiscard]] inline const std::vector<glm::ivec3>&
    get_indexed_vertices() const noexcept {
        return indexed_vertices_;
    }

    // color of vertex
    [[nodiscard]] inline const std::vector<std::uint16_t>&
    get_indexed_color_ids() const noexcept {
        return indexed_color_ids_;
    }

    // normal direction
    [[nodiscard]] inline const std::vector<glm::i8vec3>&
    get_indexed_normals() const noexcept {
        return indexed_normals_;
    }

    // color mapping from color id (vector index) to 8 bit color
    [[nodiscard]] inline const std::vector<ColorInt>&
    get_color_map() const noexcept {
        return color_map_;
    }

    // private:
    /**
     * @brief Set the color indexing to what the GPU uses
     *
     * @details When meshing a chunk the data of tiles is compared. The tile
     * does not store the color id that corresponds to the color texture on the
     * gpu. Instead of converting each tile's material and color to color id in
     * generate_mesh_greedy, it is done later when there are less values to
     * iterate over.
     *
     * That is what this function does. Converts the material and color to the
     * color id used on the GPU.
     *
     * @param map
     */
    void change_color_indexing(
        const std::map<MaterialId, const terrain::Material>& materials,
        const std::unordered_map<ColorInt, uint16_t>& mapping
    );

}; // class Mesh

/**
 * @brief Generates a mesh from the given 3D voxel structure
 *
 * @tparam T is_base_of voxel_utility::VoxelLike, T
 * @param voxel_object
 */
template <voxel_utility::VoxelLike T>
Mesh
generate_mesh(T voxel_object) {
    std::vector<uint16_t> indices;
    std::vector<glm::ivec3> indexed_vertices;
    std::vector<uint16_t> indexed_colors;

    std::vector<glm::i8vec3> indexed_normals;
    // mesh off set
    VoxelOffset center = voxel_object.get_offset();
    VoxelSize object_geometry = voxel_object.get_size();
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // in which directions is the mesh being drawn
        const std::size_t dims_index_1 = (axis + 1) % 3;
        const std::size_t dims_index_2 = (axis + 2) % 3;

        int voxel_position[3] = {0}; // position of a voxel in world space
        int mesh_normal[3] = {0};    // direction that the mesh is not changing in

        // the color of all tiles at the same "level".
        std::vector<std::pair<bool, VoxelColorId>> color_info(
            object_geometry[dims_index_1] * object_geometry[dims_index_2]
        );

        // Compute color_info
        mesh_normal[axis] = 1;
        // for each layer going in the direction of axis
        voxel_position[axis] = -1;
        while (voxel_position[axis] < static_cast<int>(object_geometry[axis])) {
            std::size_t counter = 0;
            // for each voxel in this level
            for (voxel_position[dims_index_2] = 0;
                 voxel_position[dims_index_2]
                 < static_cast<int>(object_geometry[dims_index_2]);
                 ++voxel_position[dims_index_2])
                for (voxel_position[dims_index_1] = 0;
                     voxel_position[dims_index_1]
                     < static_cast<int>(object_geometry[dims_index_1]);
                     ++voxel_position[dims_index_1], ++counter) {
                    // tile in the level above
                    const uint16_t voxel_above = voxel_object.get_voxel_color_id(
                        voxel_position[0], voxel_position[1], voxel_position[2]
                    );
                    // tiles in the level below
                    const uint16_t voxel_below = voxel_object.get_voxel_color_id(
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
            glm::uint width = 0, height = 0;

            counter = 0;
            for (glm::uint j = 0; j < object_geometry[dims_index_2]; ++j) {
                glm::uint i = 0;
                while (i < object_geometry[dims_index_1]) {
                    // color and direction of the face between two voxels
                    std::pair<bool, VoxelColorId> color = color_info[counter];
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
                               && i + width < object_geometry[dims_index_1]) {
                            ++width;
                        }
                        // Compute height
                        bool done = false;
                        for (height = 1; j + height < object_geometry[dims_index_2];
                             ++height) {
                            // expand until one of the tiles in the next row is
                            // not the same color/facing direction
                            for (glm::uint k = 0; k < width; ++k) {
                                // if the direction and color of the original
                                // face is different from the direction and
                                // color of the face currently being tested
                                std::pair<bool, VoxelColorId> test_against = color_info
                                    [counter + k
                                     + height * object_geometry[dims_index_1]];
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
                            + center
                        );
                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0] + offset_1[0],
                                voxel_position[1] + offset_1[1],
                                voxel_position[2] + offset_1[2]
                            )
                            + center
                        );
                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0] + offset_1[0] + offset_2[0],
                                voxel_position[1] + offset_1[1] + offset_2[1],
                                voxel_position[2] + offset_1[2] + offset_2[2]
                            )
                            + center
                        );
                        indexed_vertices.push_back(
                            glm::ivec3(
                                voxel_position[0] + offset_2[0],
                                voxel_position[1] + offset_2[1],
                                voxel_position[2] + offset_2[2]
                            )
                            + center
                        );

                        // how many corners on a square are there?
                        for (size_t voxel_position = 0; voxel_position < 4;
                             voxel_position++) {
                            indexed_colors.push_back(color.second);
                        }

                        glm::i8vec3 triangle_normal = glm::i8vec3(
                            glm::normalize(glm::cross(
                                glm::vec3(
                                    indexed_vertices[vertex_size]
                                    - indexed_vertices[vertex_size + 1]
                                ),
                                glm::vec3(
                                    indexed_vertices[vertex_size]
                                    - indexed_vertices[vertex_size + 2]
                                )
                            ))
                            + glm::vec3(.5, .5, .5)
                        );
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

                        for (glm::uint w = 0; w < width; ++w)
                            for (glm::uint h = 0; h < height; ++h)
                                color_info
                                    [counter + w + h * object_geometry[dims_index_1]] =
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
    }
    return Mesh(
        indices, indexed_vertices, indexed_colors, indexed_normals,
        voxel_object.get_color_ids(), object_geometry, center
    );
}

} // namespace entity
