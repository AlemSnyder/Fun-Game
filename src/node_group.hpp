#ifndef __NODE_GROUP_HPP__
#define __NODE_GROUP_HPP__

#include <list>
#include <set>
#include "tile.hpp"

class NodeGroup{
public:
    NodeGroup();
    NodeGroup(Tile* tile);
    std::set<NodeGroup *> merge_groups(NodeGroup other);
    //~NodeGroup();
    void update();
    const std::set<Tile*> get_tiles() const;
    std::set<Tile*> get_tiles();
    void add_adjacent(NodeGroup* NG);
    void remove_adjacent(NodeGroup* NG);
    const std::set<NodeGroup *> get_adjacent() const;
    bool adjacent_to(const NodeGroup* other) const;

    float get_center_x();
    float get_center_y();
    float get_center_z();

    bool operator==(const NodeGroup& other) const;

private:
    /* data */
    std::set<Tile*> tiles;
    std::set<NodeGroup *> adjacent;
    float center_x, center_y, center_z;
    
};


#endif