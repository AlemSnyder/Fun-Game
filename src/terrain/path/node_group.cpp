#include "node_group.hpp"

#include <cstdint>

namespace terrain {

NodeGroup::NodeGroup(Tile* tile, UnitPath path_type) {
    tiles.insert(tile);
    center_x = tile->get_x();
    center_y = tile->get_y();
    center_z = tile->get_z();
    path_type_ = path_type;
}

void
NodeGroup::add_adjacent(NodeGroup* NG, UnitPath type) {
    if (NG != this) {
        adjacent.insert(std::make_pair(NG, type));
        NG->adjacent.insert(std::make_pair(this, type));
    }
}

void
NodeGroup::remove_adjacent(NodeGroup* NG) {
    if (NG != this) {
        adjacent.erase(NG);
        NG->adjacent.erase(this);
    }
}

std::map<const NodeGroup*, UnitPath>
NodeGroup::get_adjacent_map() const {
    return std::map<const NodeGroup*, UnitPath>(adjacent.begin(), adjacent.end());
}

std::map<NodeGroup*, UnitPath>
NodeGroup::get_adjacent_map() {
    return adjacent;
}

std::set<const NodeGroup*>
NodeGroup::get_adjacent_clear(int path_type) const {
    std::set<const NodeGroup*> out;

    for (const std::pair<NodeGroup*, UnitPath> t : adjacent) {
        if (t.second.compatible(path_type) && t.second.is_open()) {
            out.insert(t.first);
        }
    }
    return out;
}

std::map<NodeGroup*, UnitPath>
NodeGroup::merge_groups(NodeGroup other) {
    auto size = tiles.size();
    auto other_size = other.tiles.size();
    auto total_size = size + other_size;

    center_x = (center_x * size + other.center_x * other_size) / total_size;
    center_y = (center_y * size + other.center_y * other_size) / total_size;
    center_z = (center_z * size + other.center_z * other_size) / total_size;

    for (const Tile* t : other.get_tiles()) {
        tiles.insert(t);
    }

    for (std::pair<NodeGroup* const, UnitPath> adj : other.get_adjacent_map()) {
        add_adjacent(adj.first, adj.second);
    }

    // intersection of the ways to get between tiles
    path_type_ = path_type_ & other.path_type_;

    return other.get_adjacent_map();
}

float
NodeGroup::get_center_x() const {
    return center_x;
}

float
NodeGroup::get_center_y() const {
    return center_y;
}

float
NodeGroup::get_center_z() const {
    return center_z;
}

std::array<float, 3>
NodeGroup::sop() const {
    return {center_x, center_y, center_z};
}

const std::set<const Tile*, TilePCompare>
NodeGroup::get_tiles() const {
    return std::set<const Tile*, TilePCompare>(tiles.begin(), tiles.end());
}

bool
NodeGroup::adjacent_to(NodeGroup* other) const {
    return (adjacent.find(other)) == adjacent.end();
}

bool
NodeGroup::operator==(const NodeGroup& other) const {
    return (this == &other);
}

bool
NodeGroup::operator>(const NodeGroup& other) const {
    return ((**tiles.begin()) > **(other.tiles.begin()));
}

} // namespace terrain
