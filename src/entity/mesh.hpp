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
#include "vertex.hpp"

#include <glm/glm.hpp>

#include <filesystem>
#include <map>
#include <optional>
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
        const std::vector<ColorInt>& color_map, glm::ivec3 size, glm::ivec3 center
    ) :
        size_(size),
        center_(center), indices_(indices), indexed_vertices_(indexed_vertices),
        indexed_color_ids_(indexed_color_ids), indexed_normals_(indexed_normals),
        color_map_(color_map) {}

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

/**
 * @brief Analyzes two voxels and returns a vertex representation of the face
 * between them.
 *
 * @details 80+% of this works
 *
 * @return std::optional<std::vector<Vertex>> an optional vector of four
 * vertices. Returned object has a value only if there is a visible interface
 * between the two given voxels.
 */
template <voxel_utility::VoxelLike T>
std::optional<std::vector<Vertex>>
analyze_voxel_interface(
    T voxel_object, VoxelOffset position, VoxelOffset major_direction,
    VoxelOffset minor_direction_1, VoxelOffset minor_direction_2
) {
    VoxelColorId voxel_a = voxel_object.get_voxel_color_id(position);

    VoxelColorId voxel_b = voxel_object.get_voxel_color_id(position + major_direction);

    // This tests if there is a visible face between the two voxels
    // returns a optional without a value when there is not face
    if ((voxel_a == AIR_MAT_COLOR_ID) && (voxel_b == AIR_MAT_COLOR_ID))
        return {};
    if ((voxel_a != AIR_MAT_COLOR_ID) && (voxel_b != AIR_MAT_COLOR_ID))
        return {};

    VoxelOffset normal = major_direction;

    bool should_reverse = false;

    std::vector<Vertex> out;
    // Set color as voxel that is not air
    // set the normal to point from solid to air
    // may need to reverse the direction of drawn vertices so that normal and
    // opengl normal are the same (look up face culling)
    VoxelColorId color;
    if (voxel_a == AIR_MAT_COLOR_ID) {
        color = voxel_b;
        normal = -normal;
        should_reverse = true;
    } else {
        color = voxel_a;
    }
    for (VoxelDim x = 0; x < 2; x++) {
        for (VoxelDim y = 0; y < 2; y++) {
            VoxelDim position_1;
            VoxelDim position_2;
            // reversing the order switches vertices 1 and 2 in figure 1 a
            if (should_reverse) {
                position_1 = y;
                position_2 = x;
            } else {
                position_1 = x;
                position_2 = y;
            }
            // ambient occlusion means less light when there are solid thing near the
            // vertex
            bool solid_1 = voxel_object.get_voxel_color_id(
                position + normal + (2 * position_1 - 1) * minor_direction_1
            );
            bool solid_2 = voxel_object.get_voxel_color_id(
                position + normal + (2 * position_2 - 1) * minor_direction_2
            );
            uint8_t ambient_occlusion = solid_1 + solid_2;

            //clang-format off
            VoxelOffset vertex_position = position
                                        + position_1 * minor_direction_1
                                        + position_2 * minor_direction_2
                                        + major_direction;
            //clang-format on
            out.emplace_back(
                vertex_position, glm::i8vec3(normal), color, ambient_occlusion
            );
        }
    }
    return out;
}

/*psudocode time

create outs
vec indexes
ie vect positions
vec normals
vec colors
vec ambient occlusion

size_t current vertex index = 0

for dimension (x,y,z direction)
    for major position in volume[major axis]
        unit vector_major = x, y, or z which ever we are on.
        unit minor_axis_1, 2 = the other two

        unordered_map(vertex -> voxel index)
        (hash map)

        for minor_position in volume[minor_axis1]:
        for minor_position in volume[minor_axis2]:

            analyze the (major position + minor_position + minor_position)
                and ((major position + 1) + minor_position + minor_position)
                interface

            this means
                1) not obscured
                    if it is obscured than continue (go to next interface)
                2) creating a vertex at each intersection
                    position (this is easy)
                    normal determined by which of the two voxels is air
                    color color of non-air voxel
                    ambient occlusion
                        0, 1, or 2
                        zero add one for each ADJACENT to air of analyzed this
                        is a solid voxel

                in the vertex four loop four corners:

                    crate vertex object

                    hash vertex into unordered_map
                    if it is already there use that index
                        ie add that index to indicies
                    if not
                        add vertex_object to to hash map
                            with index current vertex index
                        add vertex_object to Mesh
                        current_vertex_index +=1
*/

/**
 * @brief Generates a Mesh given a voxel object
 *
 * @details Given a Voxel Object iterates over all the voxels and adds
 * unobscured surfaces to the Mesh.This mesher will have pre-backed ambient
 * occlusion, and will use the same vertex when possible.
 */
template <voxel_utility::VoxelLike T>
Mesh
ambient_occlusion_mesher(T voxel_object) {
    std::vector<uint16_t> indicies;
    std::vector<VoxelOffset> indexed_vertices;
    std::vector<MatColorId> indexed_colors;
    std::vector<glm::i8vec3> indexed_normals;
    std::vector<uint8_t> indexed_occlusions;

    VoxelOffset size = voxel_object.get_size();
    VoxelOffset offset = voxel_object.get_offset();

    std::unordered_map<Vertex, uint16_t> vertex_ids;

    uint16_t current_vertex_index = 0;
    for (size_t dim_major_index = 0; dim_major_index < 3; dim_major_index++) {
        size_t dim_minor_index_1 = (dim_major_index + 1) % 3;
        size_t dim_minor_index_2 = (dim_major_index + 2) % 3;

        VoxelOffset major_direction({0, 0, 0});
        major_direction[dim_major_index] = 1;

        VoxelOffset minor_direction_1({0, 0, 0});
        minor_direction_1[dim_minor_index_1] = 1;

        VoxelOffset minor_direction_2({0, 0, 0});
        minor_direction_2[dim_minor_index_2] = 1;

        for (VoxelDim major_index = -1; major_index < size[dim_major_index];
             major_index++) {
            for (VoxelDim minor_index_1 = 0; minor_index_1 < size[dim_minor_index_1];
                 minor_index_1++) {
                for (VoxelDim minor_index_2 = 0;
                     minor_index_2 < size[dim_minor_index_2]; minor_index_2++) {
                    // is there a better way to write this?
                    VoxelOffset position;
                    position[dim_major_index] = major_index;
                    position[dim_minor_index_1] = minor_index_1;
                    position[dim_minor_index_2] = minor_index_2;

                    auto corners = analyze_voxel_interface(
                        voxel_object, position, major_direction, minor_direction_1,
                        minor_direction_2
                    );

                    if (!corners) {
                        continue;
                    }

                    uint16_t corner_indicies[4] = {0, 0, 0, 0};
                    // if the vertex already exists
                    for (size_t i = 0; i < 4; i++) {
                        const Vertex& vertex = corners.value()[i];
                        auto index_itr = vertex_ids.find(vertex);
                        // TODO The issue is here
                        if (index_itr != vertex_ids.end()) {
                            // The vertex exists add the index
                            corner_indicies[i] = vertex_ids.at(vertex);

                        } else {
                            // if the vertex does not already exist
                            // add the vertex to the map
                            vertex_ids.insert({vertex, current_vertex_index});

                            indexed_colors.push_back(vertex.mat_color_id);
                            indexed_normals.push_back(vertex.normal);
                            indexed_occlusions.push_back(vertex.ambient_occlusion);
                            indexed_vertices.push_back(vertex.position + offset);
                            // assign the corner to that index
                            corner_indicies[i] = current_vertex_index;

                            // increment the index by one
                            current_vertex_index++;
                        }
                    }
                    //clang-format off

                    /* Figure 1 a
                      0 ------- 1  Wow did you know a square a actually two
                      |       / |  triangles?! This is high level geometry
                      |  1  /   |  that many people don't know. Now add the
                      |   /  2  |  vertices in a counter-clock wise direction
                      | /       |  so that the opengl thinks the normal is
                      2 ------- 3  pointing toward the viewer.
                    */

                    //clang-format on

                    // triangle 1
                    indicies.push_back(corner_indicies[2]);
                    indicies.push_back(corner_indicies[1]);
                    indicies.push_back(corner_indicies[0]);
                    // triangle 2
                    indicies.push_back(corner_indicies[2]);
                    indicies.push_back(corner_indicies[3]);
                    indicies.push_back(corner_indicies[1]);
                }
            }
        }
    }
    return Mesh(
        indicies, indexed_vertices, indexed_colors, indexed_normals,
        voxel_object.get_color_ids(), size, offset
    );
}

} // namespace entity
