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
 * @file node_group.hpp
 *
 * @brief Defines NodeGroup class.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "../tile.hpp"
#include "types.hpp"
#include "unit_path.hpp"

#include <cstdint>
#include <list>
#include <unordered_map>

namespace terrain {

/**
 * @brief Group of tiles; used for path finding
 *
 * @details When path finding over large spaces the time complexity increases
 * with distance ^ 3, NodeGroups allows for path finding through chunks.
 * Because chunks are much larger than tiles the time required to find a path
 * is greatly reduced. Like a tile, NodeGroup contains a center, and
 * adjacency.
 *
 * All node groups are initialized from tiles, but then merge together when
 * they are bordering, and in the same chunk.
 *
 */
class NodeGroup {
    const ChunkPos chunk_position_;
    UnitPath path_type_; // the path restraints to get form any tile in this Group to
    std::unordered_set<LocalPosition> tile_positions_;
    std::unordered_map<NodeGroup*, UnitPath> adjacent;
    float center_x, center_y, center_z; // volumetric center, a weighted average
                                        // any other tile
 public:
    /**
     * @brief Construct a new Node Group object
     */
    NodeGroup();
    /**
     * @brief Construct a new Node Group object
     *
     * @param tile origianal tile in this NodeGroup
     * @param path_type most complex path type used to get from any tile to any other
     * tile in this node group
     */
    NodeGroup(ChunkPos chunk_position, LocalPosition tile_position, UnitPath path_type);

    // NodeGroup(std::unordered_set<LocalPosition>, UnitPath path_type);

    /**
     * @brief Merge two node groups together
     *
     * @param other the node group that is added to this one
     * @return std::unordered_map<NodeGroup *, UnitPath> adjacent node groups of other
     */
    std::unordered_map<NodeGroup*, UnitPath> merge_groups(NodeGroup other);

    [[nodiscard]] inline const ChunkPos
    get_chunk_position() {
        return chunk_position_;
    }

    // !void update(); If I modify terrain path finding should not break
    // this should be in chunk though
    /**
     * @brief Get a set of tile positions for each tile in the group.
     *
     * @return A set of global tile positions.
     */
    [[nodiscard]] std::unordered_set<TerrainOffset3> get_tiles() const;

    [[nodiscard]] inline const std::unordered_set<LocalPosition>& get_local_positions() const {
        return tile_positions_;
    }

    /**
     * @brief Add adjacent node group where
     *
     * @param NG The adjacent node group
     * @param path_type The path type between the two node groups
     */
    void add_adjacent(NodeGroup* NG, UnitPath path_type);

    /**
     * @brief Get adjacent node groups that are compatible with path type
     *
     * @param path_type type of paths that are allowed
     * @return set of adjacent node groups
     */
    [[nodiscard]] std::unordered_set<const NodeGroup*>
    get_adjacent_clear(UnitPath path_type) const;

    /**
     * @brief Remove node group from adjacency
     *
     * @param NG node group to remove
     */
    void remove_adjacent(NodeGroup* NG);

    /**
     * @brief Get the adjacency map
     *
     * @return std::unordered_map<NodeGroup *, UnitPath>
     */
    [[nodiscard]] std::unordered_map<NodeGroup*, UnitPath> get_adjacent_map();

    /**
     * @brief Get the adjacency map
     *
     * @return std::unordered_map<const NodeGroup *, UnitPath>
     */
    [[nodiscard]] std::unordered_map<const NodeGroup*, UnitPath>
    get_adjacent_map() const;

    /**
     * @brief Add a tile to the node_group
     * 
     * @param LocalPosition tile_position offset from chunk origen of tile
     */
    void add_tile(LocalPosition tile_position);

    /**
     * @brief Test if two node groups are adjacent
     *
     * @param other the other node group
     * @return true if the node groups are adjacent
     * @return false otherwise
     */
    [[nodiscard]] bool adjacent_to(NodeGroup* other) const;

    /**
     * @brief vector to volumetric center
     *
     * @return std::array<float, 3>
     */
    [[nodiscard]] glm::vec3 sop() const;

    [[nodiscard]] TerrainOffset3 unique_position() const;

    bool operator==(const NodeGroup& other) const;
    bool operator>(const NodeGroup& other) const;
};

} // namespace terrain
