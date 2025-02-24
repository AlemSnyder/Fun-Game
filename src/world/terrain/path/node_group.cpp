#include "node_group.hpp"

#include "world/terrain/chunk.hpp"

#include <cstdint>

namespace terrain {

NodeGroup::NodeGroup(
    ChunkPos chunk_position, LocalPosition tile_position, UnitPath path_type
) :
    chunk_position_(chunk_position),
    path_type_(path_type), tile_positions_({tile_position}), center_x(tile_position.x),
    center_y(tile_position.y), center_z(tile_position.z) {}

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

std::unordered_map<const NodeGroup*, UnitPath>
NodeGroup::get_adjacent_map() const {
    return std::unordered_map<const NodeGroup*, UnitPath>(
        adjacent.begin(), adjacent.end()
    );
}

std::unordered_map<NodeGroup*, UnitPath>
NodeGroup::get_adjacent_map() {
    return adjacent;
}

std::unordered_set<const NodeGroup*>
NodeGroup::get_adjacent_clear(UnitPath path_type) const {
    std::unordered_set<const NodeGroup*> out;

    for (const std::pair<NodeGroup*, UnitPath> t : adjacent) {
        if (t.second.compatible(path_type) && t.second.is_open()) {
            out.insert(t.first);
        }
    }
    return out;
}

std::unordered_map<NodeGroup*, UnitPath>
NodeGroup::merge_groups(NodeGroup other) {
    auto size = tile_positions_.size();
    auto other_size = other.tile_positions_.size();
    auto total_size = size + other_size;

    center_x = (center_x * size + other.center_x * other_size) / total_size;
    center_y = (center_y * size + other.center_y * other_size) / total_size;
    center_z = (center_z * size + other.center_z * other_size) / total_size;

    for (const auto t : other.get_tiles()) {
        tile_positions_.insert(t);
    }

    for (std::pair<NodeGroup* const, UnitPath> adj : other.get_adjacent_map()) {
        add_adjacent(adj.first, adj.second);
    }

    // intersection of the ways to get between tiles
    path_type_ = path_type_ & other.path_type_;

    return other.get_adjacent_map();
}

void
NodeGroup::add_tile(LocalPosition position){

    auto size = tile_positions_.size();


    tile_positions_.insert(position);

    center_x = (center_x * size + position.x) / (size + 1);
    center_y = (center_y * size + position.y) / (size + 1);
    center_z = (center_z * size + position.z) / (size + 1);

}

glm::vec3
NodeGroup::sop() const {
    return glm::vec3(center_x, center_y, center_z)
           + glm::vec3(chunk_position_) * float(Chunk::SIZE);
}

std::unordered_set<TerrainOffset3>
NodeGroup::get_tiles() const {
    std::unordered_set<TerrainOffset3> out({});
    for (auto position : tile_positions_) {
        out.insert(TerrainOffset3(position) + TerrainOffset3(chunk_position_));
    }
    return out;
}

bool
NodeGroup::adjacent_to(NodeGroup* other) const {
    return (adjacent.find(other)) == adjacent.end();
}

bool
NodeGroup::operator==(const NodeGroup& other) const {
    return (this == &other);
}

TerrainOffset3
NodeGroup::unique_position() const {
    return TerrainOffset3(*tile_positions_.begin())
           + TerrainOffset3(chunk_position_) * TerrainOffset(Chunk::SIZE);
}

// bool
// NodeGroup::operator>(const NodeGroup& other) const {
//     return ((**tile_positions_.begin()) > **(other.tile_positions_.begin()));
// }

} // namespace terrain
