#include "chunk.hpp"

#include "path/tile_iterators.hpp"
#include "terrain.hpp"
#include "tile.hpp"

namespace terrain {

Chunk::Chunk(int bx, int by, int bz, Terrain* ter) {
    ter_ = ter;
    Cx_ = bx;
    Cy_ = by;
    Cz_ = bz;
    for (int x = size * bx; x < size * (1 + bx); x++)
        for (int y = size * by; y < size * (1 + by); y++)
            for (int z = size * bz; z < size * (1 + bz); z++) {
                if (ter->can_stand_1(x, y, z)) {
                    // the int determines which paths between two tiles are
                    // compliant 31 means anything that is not opposite corner.
                    // look at onePath for more information
                    NodeGroup group = NodeGroup(ter->get_tile(x, y, z), 31);
                    node_groups_.push_back(group);
                    ter->add_node_group(&node_groups_.back());
                }
            }
    for (NodeGroup& NG : node_groups_) {
        for (const Tile* tile_main : NG.get_tiles()) {
            for (auto it =
                     ter_->get_tile_adjacent_iterator(ter->pos(tile_main), 31);
                 !it.end(); it++) {
                if (NodeGroup* to_add = ter->get_node_group(it.get_pos())) {
                    // possible to go in both directions
                    // like add adjacent from above, and below
                    NG.add_adjacent(to_add, 31);
                }
            }
        }
    }

    auto it = node_groups_.begin();
    while (it != node_groups_.end()) {
        // to merge = get_adjacent_map()
        std::set<NodeGroup*> to_merge;
        for (std::pair<NodeGroup* const, UnitPath> other : (it)->get_adjacent_map()) {
            if (contains_nodeGroup(other.first)) {
                to_merge.insert(other.first);
            }
        }
        R_merge((*it), to_merge);
        it++;
    }
}

void
Chunk::R_merge(NodeGroup& G1, std::set<NodeGroup*>& to_merge) {
    if (to_merge.size() == 0) {
        return;
    }
    std::set<NodeGroup*> new_merge;
    while (to_merge.size() > 0) {
        auto G2 = to_merge.begin();
        std::map<NodeGroup*, UnitPath> to_add = G1.merge_groups(**(G2));
        delNodeGroup(**G2);
        new_merge.erase(*G2);
        ter_->add_node_group(&G1);
        for (std::pair<NodeGroup* const, UnitPath> NG : to_add) {
            if (contains_nodeGroup(NG.first) && &G1 != NG.first) {
                new_merge.insert(NG.first);
            }
        }
        to_merge.erase(G2);
    }
    R_merge(G1, new_merge);
}

void
Chunk::add_nodes_to(std::set<const NodeGroup*>& out) const {
    for (auto it = node_groups_.begin(); it != node_groups_.end(); it++) {
        auto& elem = *it;
        out.insert(&elem); // Ptr to element
    }
}

void
Chunk::delNodeGroup(NodeGroup& NG) {
    // remove form ter. tile to group map
    ter_->remove_node_group(&NG);
    for (std::pair<NodeGroup* const, UnitPath>& adjacent : NG.get_adjacent_map()) {
        adjacent.first->remove_adjacent(&NG);
    }
    node_groups_.remove(NG);
}

void
Chunk::mergeNodeGroup(NodeGroup& G1, NodeGroup& G2) {
    G1.merge_groups(G2);
    delNodeGroup(G2);
    // return out;
    ter_->add_node_group(&G1);
}

inline bool
Chunk::contains_nodeGroup(NodeGroup* NG) {
    return (
        NG->get_center_x() >= size * Cx_ && NG->get_center_x() < size * (1 + Cx_)
        && NG->get_center_y() >= size * Cy_ && NG->get_center_y() < size * (1 + Cy_)
        && NG->get_center_z() >= size * Cz_ && NG->get_center_z() < size * (1 + Cz_)
    );
}

} // namespace terrain
