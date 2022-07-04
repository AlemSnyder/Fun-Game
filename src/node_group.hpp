#ifndef __NODE_GROUP_HPP__
#define __NODE_GROUP_HPP__

#include <list>
#include <set>
#include <stdint.h>
#include "onepath.hpp"
#include "tile.hpp"

class NodeGroup{
private:
    //static inline bool cmp(Tile* a, Tile* b) { return ((*a) > (*b)); };
    std::set<const Tile*, TilePCompare> tiles;
    std::map<NodeGroup *, OnePath> adjacent;
    float center_x, center_y, center_z; // volumetric center, a weighted average
    OnePath path_type_; // the path restraints to get form any tile in this Group to any other tile
public:
    NodeGroup();
    NodeGroup(Tile* tile, OnePath path_type);
    std::map<NodeGroup *, OnePath> merge_groups(NodeGroup other);
    //~NodeGroup();
    void update();
    const std::set<const Tile*, TilePCompare> get_tiles() const;
    void add_adjacent(NodeGroup* NG, OnePath path_type);
    std::set<const NodeGroup *> get_adjacent_clear(int path_type) const;
    void remove_adjacent(NodeGroup* NG);
    std::map<NodeGroup *, OnePath> get_adjacent();
    std::map<const NodeGroup *, OnePath> get_adjacent() const;
    bool adjacent_to(NodeGroup* other) const;

    float get_center_x();
    float get_center_y();
    float get_center_z();
    std::array<float, 3> sop() const;

    bool operator==(const NodeGroup& other) const;
    bool operator>(const NodeGroup& other) const;
};

#endif
