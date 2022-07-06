#include <cstdint>
#include "node_group.hpp"

NodeGroup::NodeGroup(Tile* tile, OnePath path_type){
    tiles.insert(tile);
    center_x = tile->get_x();
    center_y = tile->get_y();
    center_z = tile->get_z();
    path_type_ = path_type;
}

void NodeGroup::add_adjacent(NodeGroup* NG, OnePath type){
    if (NG != this){
        adjacent.insert(std::make_pair(NG, type));
        NG->adjacent.insert(std::make_pair(this, type));
    }
}
void NodeGroup::remove_adjacent(NodeGroup* NG){
    if (NG != this){
        adjacent.erase(NG);
        NG->adjacent.erase(this);
    }
}

std::map<const NodeGroup *, OnePath> NodeGroup::get_adjacent_map() const{
    return std::map<const NodeGroup *, OnePath>(adjacent.begin(), adjacent.end());
}

std::map<NodeGroup *, OnePath> NodeGroup::get_adjacent_map(){
    return adjacent;
}

std::set<const NodeGroup *> NodeGroup::get_adjacent_clear(int path_type)const{
    std::set<const NodeGroup *> out;

    for (const std::pair<NodeGroup *,OnePath> t : adjacent) {
        if (t.second.compatible(path_type) && t.second.is_open()) {
            out.insert(t.first);
        }
    }
    return out;
}

std::map<NodeGroup *, OnePath> NodeGroup::merge_groups(NodeGroup other){
    center_x = (center_x*tiles.size() + other.center_x*other.tiles.size())/(tiles.size()+other.tiles.size());
    center_y = (center_y*tiles.size() + other.center_y*other.tiles.size())/(tiles.size()+other.tiles.size());
    center_z = (center_z*tiles.size() + other.center_z*other.tiles.size())/(tiles.size()+other.tiles.size());

    for (const Tile* t : other.get_tiles()){
        tiles.insert(t);
    }
    for (std::pair<NodeGroup *const, OnePath> adj: other.get_adjacent_map()){
        add_adjacent(adj.first, adj.second);
    }
    path_type_ = path_type_ & other.path_type_; // restriction of the ways to get between tiles

    return other.get_adjacent_map();
}

float NodeGroup::get_center_x(){ return center_x; }
float NodeGroup::get_center_y(){ return center_y; }
float NodeGroup::get_center_z(){ return center_z; }
std::array<float, 3> NodeGroup::sop() const {
    return { center_x, center_y, center_z };
}

const std::set<const Tile*, TilePCompare> NodeGroup::get_tiles() const {
    return std::set<const Tile*, TilePCompare>(tiles.begin(), tiles.end());
}

bool NodeGroup::adjacent_to(NodeGroup* other) const{
    return (adjacent.find(other)) == adjacent.end();
}

bool NodeGroup::operator==(const NodeGroup & other) const{
    return (this == &other);
}
bool NodeGroup::operator>(const NodeGroup & other) const{
    return ((**tiles.begin()) > **(other.tiles.begin()));
}