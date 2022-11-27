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
 * @file chunk.hpp
 *
 * @brief Defines Chunk class.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "path/node_group.hpp"
#include "terrain.hpp"
#include "tile.hpp"

#include <list>
#include <set>

namespace terrain {

class Terrain;

/**
 * @brief It's a chunk you've played minecraft.
 *
 * @details The chunk is used to define node groups that are used for
 * path finding. Chunk will also be used to generate meshes so that small
 * modifications can be made.
 *
 */
class Chunk {
    std::list<NodeGroup> node_groups_;
    Terrain* ter_;
    uint16_t Cx_, Cy_, Cz_; // Chunk position. Incremented by 1 so multiply by
                            // Chunk::SIZE to get tile position.
 public:
    static const int8_t SIZE = 16; // number of tiles in each direction
    /**
     * @brief Construct a new Chunk object
     *
     * @param bx chunk x position
     * @param by chunk y position
     * @param bz chunk z position
     * @param ter the terrain this chunk is in
     */
    Chunk(int bx, int by, int bz, Terrain* ter);
    /**
     * @brief adds node groups in this chunk to out
     *
     * @param out a set that gets the node groups in this chunk added to it
     */
    void add_nodes_to(std::set<const NodeGroup*>& out) const;

    /**
     * @brief Used for getting mesh
     *
     * @return std::vector<int> offset of chunk in world space
     */
    inline std::vector<int> get_offset() const {
        return {Cx_ * Chunk::SIZE, Cy_ * Chunk::SIZE, Cz_ * Chunk::SIZE};
    }

    /**
     * @brief Get the size of a chunk
     *
     * @return std::vector<unsigned int> vector of Chunk::SIZE
     */
    inline static std::vector<unsigned int> get_size() {
        return {Chunk::SIZE, Chunk::SIZE, Chunk::SIZE};
    }

    /**
     * @brief Get the color of a tile
     *
     * @param x x position in chunk
     * @param y y position in chunk
     * @param z z position in chunk
     * @return uint32_t color or tile color id
     */
    uint32_t get_voxel(int x, int y, int z) const;

 private:

    void delNodeGroup(NodeGroup& NG);
    void mergeNodeGroup(NodeGroup& g1, NodeGroup& g2);
    bool contains_nodeGroup(NodeGroup*);
    void R_merge(NodeGroup& g1, std::set<NodeGroup*>& to_merge);
};

} // namespace terrain
