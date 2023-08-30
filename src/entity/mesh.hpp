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
        const std::vector<MatColorId>& indexed_color_ids,
        const std::vector<glm::i8vec3>& indexed_normals,
        const std::vector<ColorInt>& color_map
    ) :
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
    std::vector<MatColorId> indexed_color_ids_;
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
    [[nodiscard]] inline const std::vector<uint16_t>&
    get_indices() const noexcept {
        return indices_;
    }

    // position of vertices in mesh space
    [[nodiscard]] inline const std::vector<glm::ivec3>&
    get_indexed_vertices() const noexcept {
        return indexed_vertices_;
    }

    // color of vertex
    [[nodiscard]] inline const std::vector<MatColorId>&
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

inline void
greedy_mesh(
    std::vector<uint16_t>& indices, std::vector<VoxelOffset>& indexed_vertices,
    std::vector<MatColorId>& indexed_colors, std::vector<glm::i8vec3>& indexed_normals,

    VoxelSize object_geometry,
    const std::vector<std::pair<bool, VoxelColorId>>& color_info,
    VoxelOffset axis_offset, std::size_t dims_index_1, std::size_t dims_index_2
) {
    // takes object_geometry color_info and offset
    // modifies indicies, indexed_vertices, indexed_colors, indexed_normals

    // Generate mesh for color_info using lexicographic ordering
    VoxelOffset voxel_position(0, 0, 0);

    size_t counter = 0;
    glm::uint face_width = object_geometry[dims_index_2];
    for (glm::uint j = 0; j < face_width; ++j) {
        glm::uint i = 0;
        glm::uint face_height = object_geometry[dims_index_1];
        while (i < face_height) {
            // color and direction of the face between two voxels
            std::pair<bool, VoxelColorId> color = color_info[counter];
            // if there is a face between two voxels
            if (!color.second) {
                ++i;
                ++counter;
                continue;
            }
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

            // Compute width and height of greedy mesh
            glm::uint width = 1;
            glm::uint height = 1;
            while (color == color_info[counter + width]
                   && i + width < object_geometry[dims_index_1]) {
                ++width;
            }
            // Compute height
            bool done = false;
            while (j + height < object_geometry[dims_index_2]) {
                // expand until one of the tiles in the next row is
                // not the same color/facing direction
                for (glm::uint k = 0; k < width; ++k) {
                    // if the direction and color of the original
                    // face is different from the direction and
                    // color of the face currently being tested
                    size_t face_index =
                        counter + k + height * object_geometry[dims_index_1];
                    std::pair<bool, VoxelColorId> test_against = color_info[face_index];
                    if (color != test_against) {
                        done = true;
                        break;
                    }
                }
                if (done)
                    break;
                ++height;
            }

            // Add quad
            voxel_position[dims_index_1] = i;
            voxel_position[dims_index_2] = j;

            // a width or height depending on normal direction
            VoxelOffset offset_1(0, 0, 0);
            VoxelOffset offset_2(0, 0, 0);

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

            indexed_vertices.push_back(voxel_position + axis_offset);
            indexed_vertices.push_back(voxel_position + offset_1 + axis_offset);
            indexed_vertices.push_back(
                voxel_position + offset_1 + offset_2 + axis_offset
            );
            indexed_vertices.push_back(voxel_position + offset_2 + axis_offset);

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
                // adding .5 because average is plus .5 then floor
                + glm::vec3(.5, .5, .5)
            );

            // how many corners on a square are there?
            for (size_t voxel_position = 0; voxel_position < 4; voxel_position++) {
                // same color for each corner
                indexed_colors.push_back(color.second);
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

            // Increment counters
            i += width;
            counter += width;
        }
    }
}

/**
 * @brief Generates a mesh from the given 3D voxel structure
 *
 * @tparam T is_base_of voxel_utility::VoxelLike, T
 * @param voxel_object
 */
template <voxel_utility::VoxelLike T>
Mesh
generate_mesh(T voxel_object) {
    // data that will be passed to mesh constructor
    std::vector<uint16_t> indices;
    std::vector<VoxelOffset> indexed_vertices;
    std::vector<MatColorId> indexed_colors;
    std::vector<glm::i8vec3> indexed_normals;

    // mesh offset
    VoxelOffset center = voxel_object.get_offset();
    // mesh size
    VoxelSize object_geometry = voxel_object.get_size();
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // in which directions is the mesh being drawn
        const std::size_t dims_index_1 = (axis + 1) % 3;
        const std::size_t dims_index_2 = (axis + 2) % 3;

        // position of a voxel in world space
        VoxelOffset voxel_position(0, 0, 0);
        // direction that the mesh is not changing in
        VoxelOffset mesh_normal(0, 0, 0);

        // the color of all tiles at the same "level".
        std::vector<std::pair<bool, VoxelColorId>> color_info(
            object_geometry[dims_index_1] * object_geometry[dims_index_2]
        );

        // Compute color_info
        // for each layer going in the direction of axis

        // mesh normal is in direction of axis, all other values are 0
        // this could be reversed later
        mesh_normal[axis] = 1;
        // start the vertex position scan one less in the direction of axis
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
                    const uint16_t voxel_above =
                        voxel_object.get_voxel_color_id(voxel_position);
                    // tiles in the level below
                    const uint16_t voxel_below =
                        voxel_object.get_voxel_color_id(voxel_position + mesh_normal);
                    const bool air_above = voxel_above;
                    // if both solid or both not solid then don't make
                    // a face between the voxels
                    if ((voxel_below != 0) == air_above) {
                        color_info[counter] = std::make_pair(false, 0);
                    }
                    // if voxel_above is solid make face with voxel_above's
                    // color facing up
                    else if (air_above) {
                        color_info[counter] = std::make_pair(true, voxel_above);
                    }
                    // voxel_below is solid make face with voxel_below's color
                    // facing down
                    else {
                        color_info[counter] = std::make_pair(false, voxel_below);
                    }
                }

            ++voxel_position[axis]; // go to the next level

            // After taking a slice use the greedy algorithm on that slice

            VoxelOffset axis_offset(0, 0, 0);
            axis_offset[axis] = voxel_position[axis];

            counter = 0;
            greedy_mesh(
                indices, indexed_vertices, indexed_colors, indexed_normals,
                object_geometry, color_info, center + axis_offset, dims_index_1,
                dims_index_2
            );
        }
    }
    return Mesh(
        indices, indexed_vertices, indexed_colors, indexed_normals,
        voxel_object.get_color_ids()
    );
}

} // namespace entity
