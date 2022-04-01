#ifndef __NODE_GROUP_HPP__
#define __NODE_GROUP_HPP__

#include <list>
#include <set>
#include "tile.hpp"

class NodeGroup{
private:
    //static inline bool cmp(Tile* a, Tile* b) { return ((*a) > (*b)); };
    std::set<Tile*, TilePCompare> tiles;
    std::set<NodeGroup *> adjacent;
    float center_x, center_y, center_z;
public:
    NodeGroup();
    NodeGroup(Tile* tile);
    std::set<NodeGroup *> merge_groups(NodeGroup other);
    //~NodeGroup();
    void update();
    const std::set<Tile*, TilePCompare> get_tiles() const;
    std::set<Tile*, TilePCompare> get_tiles();
    void add_adjacent(NodeGroup* NG);
    void remove_adjacent(NodeGroup* NG);
    const std::set<NodeGroup *> get_adjacent() const;
    bool adjacent_to(const NodeGroup* other) const;

    float get_center_x();
    float get_center_y();
    float get_center_z();
    std::array<float, 3> sop() const;

    bool operator==(const NodeGroup& other) const;
    bool operator>(const NodeGroup& other) const;
};


#endif