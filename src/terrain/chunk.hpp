// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under

 * * the terms of the GNU General Public License as published by
 * the Free Software
 *
 * Foundation, version 2 of the License, or
 * (at your option) any later version.
 *

 * *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT
 * ANY
 * WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS
 * FOR A
 * PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.

 */

/**
 * @file chunk.hpp
 *
 * @brief Defines Chunk class.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "../util/voxel.hpp"
#include "path/node_group.hpp"

#include <list>
#include <set>

namespace terrain {

class Terrain;

/**
 * @brief It's a chunk you've played minecraft.
 *
 * @details The chunk is used to

 * * define node groups that are used for
 * path finding. Chunk will also be used to
 *
 * generate meshes so that small
 * modifications can be made.
 *
 */
class Chunk : public voxel_utility::VoxelBase {
    std::list<NodeGroup> node_groups_;
    Terrain* ter_;
    // Chunk position. Incremented by 1 so multiply by Chunk::SIZE to get tile position.
    Dim Cx_, Cy_, Cz_;

 public:
    static const Dim SIZE = 16; // number of tiles in each direction

    /**
     * @brief Construct a new Chunk object
     *
     * @param bx chunk x

     * * position
     * @param by chunk y position
     * @param bz chunk z position


     * * * @param ter the terrain this chunk is in
     */
    Chunk(Dim bx, Dim by, Dim bz, Terrain* ter) : Chunk({bx, by, bz}, ter){};

    Chunk(TerrainDim3 chunk_position, Terrain* ter);

    /**
     * @brief adds node groups in this chunk to out
     *
     * @param out a

     * * set that gets the node groups in this chunk added to it
     */
    void add_nodes_to(std::set<const NodeGroup*>& out) const;

    /**
     * @brief Used for getting mesh
     *
     * @return std::vector<int>

     * * offset of chunk in world space
     */
    [[nodiscard]] inline VoxelOffset
    get_offset() const {
        return {Cx_ * Chunk::SIZE, Cy_ * Chunk::SIZE, Cz_ * Chunk::SIZE};
    }

    /**
     * @brief Get the size of a chunk
     *
     * @return
     * std::vector<unsigned
     * int> vector of Chunk::SIZE
     */
    [[nodiscard]] inline VoxelSize
    get_size() {
        return {Chunk::SIZE, Chunk::SIZE, Chunk::SIZE};
    }

    /**
     * @brief Get the color of a tile
     *
     * @param x x position in

     * * chunk
     * @param y y position in chunk
     * @param z z position in chunk


     * * * @return ColorInt tile color id
     */
    [[nodiscard]] ColorInt get_voxel(int x, int y, int z) const;

    /**
     * @brief Get the voxel color id
     *
     * @param x x position in
     * chunk

     * * @param y y position in chunk
     * @param z z position in chunk

     * * @return
     * MatColorId material and color id
     */
    [[nodiscard]] MatColorId get_voxel_color_id(int x, int y, int z) const;

    /**
     * @brief Get the colors used in terrain.
     *
     * @return const
     *
     * std::vector<ColorInt>&
     */
    [[nodiscard]] const inline std::vector<ColorInt>&
    get_color_ids() const {
        return TerrainColorMapping::get_color_ids_map();
    }

 private:
    void delNodeGroup(NodeGroup& NG);
    void mergeNodeGroup(NodeGroup& g1, NodeGroup& g2);
    bool contains_nodeGroup(NodeGroup*);
    void R_merge(NodeGroup& g1, std::set<NodeGroup*>& to_merge);
};

} // namespace terrain
