#include "node_group.hpp"

NodeGroup::NodeGroup(Tile* tile){
    tiles.insert(tile);
    center_x = tile->get_x();
    center_y = tile->get_y();
    center_z = tile->get_z();
}

void NodeGroup::add_adjacent(NodeGroup* NG){
    if (NG != this){
        adjacent.insert(NG);
        NG->adjacent.insert(this);
    }
}
void NodeGroup::remove_adjacent(NodeGroup* NG){
    if (NG != this){
        adjacent.erase(NG);
        NG->adjacent.erase(this);
    }
}

const std::set<NodeGroup *> NodeGroup::get_adjacent() const{
    return adjacent;
}

std::set<NodeGroup *> NodeGroup::merge_groups(NodeGroup other){
    center_x = (center_x*tiles.size() + other.center_x*other.tiles.size())/(tiles.size()+other.tiles.size());
    center_y = (center_y*tiles.size() + other.center_y*other.tiles.size())/(tiles.size()+other.tiles.size());
    center_z = (center_z*tiles.size() + other.center_z*other.tiles.size())/(tiles.size()+other.tiles.size());

    for (Tile* t : other.get_tiles()){
        tiles.insert(t);
    }
    for (NodeGroup* adj: other.get_adjacent()){
        add_adjacent(adj);
    }
    return other.adjacent;
}

float NodeGroup::get_center_x(){ return center_x; }
float NodeGroup::get_center_y(){ return center_y; }
float NodeGroup::get_center_z(){ return center_z; }

const std::set<Tile*> NodeGroup::get_tiles() const{
    return tiles;
}
std::set<Tile*> NodeGroup::get_tiles(){
    return tiles;
}

inline bool NodeGroup::adjacent_to(const NodeGroup* other) const{
    return (std::find(adjacent.begin(), adjacent.end(), other) == adjacent.end());
}

bool NodeGroup::operator==(const NodeGroup & other) const{
    return (this == &other);
}