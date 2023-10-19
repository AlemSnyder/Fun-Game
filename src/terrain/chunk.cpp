#include "chunk.hpp"

#include "material.hpp"
#include "path/tile_iterators.hpp"
#include "terrain.hpp"
#include "terrain_base.hpp"
#include "tile.hpp"

namespace terrain {

Chunk::Chunk(TerrainDim3 chunk_position, Terrain* ter) :
    ter_(ter), Cx_(chunk_position.x), Cy_(chunk_position.y), Cz_(chunk_position.z) {
    for (Dim x = SIZE * Cx_; x < SIZE * (1 + Cx_); x++)
        for (Dim y = SIZE * Cy_; y < SIZE * (1 + Cy_); y++)
            for (Dim z = SIZE * Cz_; z < SIZE * (1 + Cz_); z++) {
                if (ter->can_stand_1(x, y, z)) {
                    // the int determines which paths between two tiles are
                    // compliant 31 means anything that is not opposite corner.
                    // look at onePath for more information
                    NodeGroup group = NodeGroup(ter->get_tile(x, y, z), 31);
                    node_groups_.push_back(group);
                    ter->add_node_group(&node_groups_.back());
                }
            }

    for (NodeGroup& NG : node_groups_)
        for (const Tile* tile_main : NG.get_tiles()) {
            auto it = ter_->get_tile_adjacent_iterator(ter->pos(tile_main), 31);
            for (; !it.end(); it++) {
                if (NodeGroup* to_add = ter->get_node_group(it.get_pos())) {
                    // possible to go in both directions
                    // like add adjacent from above, and below
                    NG.add_adjacent(to_add, 31);
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
        NG->get_center_x() >= SIZE * Cx_ && NG->get_center_x() < SIZE * (1 + Cx_)
        && NG->get_center_y() >= SIZE * Cy_ && NG->get_center_y() < SIZE * (1 + Cy_)
        && NG->get_center_z() >= SIZE * Cz_ && NG->get_center_z() < SIZE * (1 + Cz_)
    );
}

ColorInt
terrain::Chunk::get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const {
    return ter_->get_voxel(
        x + Cx_ * Chunk::SIZE, y + Cy_ * Chunk::SIZE, z + Cz_ * Chunk::SIZE
    );
}

MatColorId
terrain::Chunk::get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const {
    return ter_->get_voxel_color_id(
        x + Cx_ * Chunk::SIZE, y + Cy_ * Chunk::SIZE, z + Cz_ * Chunk::SIZE
    );
}

std::vector<MatColorId>
ChunkData::get_mat_color_from_chunk(const Chunk& chunk) {
    std::vector<MatColorId> out;
    out.reserve(
        (Chunk::SIZE + 2) * (terrain::Chunk::SIZE + 2) * (terrain::Chunk::SIZE + 2)
    );
    for (VoxelDim x = -1; x < SIZE - 1; x++) {
        for (VoxelDim y = -1; y < SIZE - 1; y++) {
            for (VoxelDim z = -1; z < SIZE - 1; z++) {
                out.push_back(chunk.get_voxel_color_id(x, y, z));
            }
        }
    }
    return out;
}

MatColorId
ChunkData::get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const {
    // out side of data -> return 0
    // In reality this should never be called.
    // It might be beneficial to call Chunk get_voxel_..., but that would negate
    // the benefit of copping the data.
    // Not changing the code means it "fails" loudly
    if (x < -1 || y < -1 || z < -1) {
        return AIR_MAT_COLOR_ID; // 0
    }
    if (x > SIZE - 1 || y > SIZE - 1 || z > SIZE - 1) {
        return AIR_MAT_COLOR_ID; // 0
    }

    // stand vector position from 3D space
    // That's not true. +1 because (-1,-1,-1) is in the zero vector position
    size_t position = ((x + 1) * (SIZE * SIZE)) + ((y + 1) * SIZE) + z + 1;
    return data_[position];
}

} // namespace terrain
