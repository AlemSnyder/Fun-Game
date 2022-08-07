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

#ifndef __NODE_GROUP_HPP__
#define __NODE_GROUP_HPP__

#include <list>
#include <set>
#include <cstdint>
#include "unit_path.hpp"
#include "tile.hpp"

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
class NodeGroup{
public:
    /**
     * @brief Construct a new Node Group object
     */
    NodeGroup();
    /**
     * @brief Construct a new Node Group object
     * 
     * @param tile origianal tile in this NodeGroup
     * @param path_type most complex path type used to get from any tile to any other tile in this node group
     */
    NodeGroup(Tile* tile, UnitPath path_type);
    /**
     * @brief Merge two node groups together
     * 
     * @param other the node group that is added to this one
     * @return std::map<NodeGroup *, UnitPath> adjacent node groups of other
     */
    std::map<NodeGroup *, UnitPath> merge_groups(NodeGroup other);
    
    // !void update(); If I modify terrain path finding should not break
    // this should be in chunk though
    /**
     * @brief Get set of tiles in this group
     * 
     * @return The set of tiles
     */
    const std::set<const Tile*, TilePCompare> get_tiles() const;
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
    std::set<const NodeGroup *> get_adjacent_clear(int path_type) const;
    /**
     * @brief Remove node group from adjacency
     * 
     * @param NG node group to remove
     */
    void remove_adjacent(NodeGroup* NG);
    /**
     * @brief Get the adjacency map
     * 
     * @return std::map<NodeGroup *, UnitPath> 
     */
    std::map<NodeGroup *, UnitPath> get_adjacent_map();
    /**
     * @brief Get the adjacency map
     * 
     * @return std::map<const NodeGroup *, UnitPath> 
     */
    std::map<const NodeGroup *, UnitPath> get_adjacent_map() const;
    /**
     * @brief Test if two node groups are adjacent
     * 
     * @param other the other node group
     * @return true if the node groups are adjacent
     * @return false otherwise
     */
    bool adjacent_to(NodeGroup* other) const;
    /**
     * @brief Get the center in the x direction
     * 
     * @return float 
     */
    float get_center_x();
        /**
     * @brief Get the center in the y direction
     * 
     * @return float 
     */
    float get_center_y();
        /**
     * @brief Get the center in the z direction
     * 
     * @return float 
     */
    float get_center_z();
    /**
     * @brief vector to volumetric center
     * 
     * @return std::array<float, 3> 
     */
    std::array<float, 3> sop() const;

    bool operator==(const NodeGroup& other) const;
    bool operator>(const NodeGroup& other) const;
private:
    std::set<const Tile*, TilePCompare> tiles;
    std::map<NodeGroup *, UnitPath> adjacent;
    float center_x, center_y, center_z; // volumetric center, a weighted average
    UnitPath path_type_; // the path restraints to get form any tile in this Group to any other tile
};

#endif
