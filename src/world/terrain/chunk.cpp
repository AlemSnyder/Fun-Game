#include "chunk.hpp"

#include "material.hpp"
#include "path/tile_iterators.hpp"
#include "terrain.hpp"
#include "tile.hpp"

namespace terrain {

Chunk::Chunk(TerrainDim3 chunk_position, Terrain* ter) :
    ter_(ter), chunk_position_(chunk_position),
    tiles_(SIZE * SIZE * SIZE, Tile(ter_->get_material(0), 0)) {}

// initialize all tiles. this is done above. currently it is wrong.

// TODO this is an incredibly cursed function
void
Chunk::stamp_tile_region(
    MaterialId mat, ColorId color_id, std::optional<MaterialGroup> elements_can_stamp,
    LocalPosition xyz_start, LocalPosition xyz_end
) {
    for (TerrainOffset x = xyz_start.x; x < xyz_end.x; x++) {
        for (TerrainOffset y = xyz_start.y; y < xyz_end.y; y++) {
            for (TerrainOffset z = xyz_start.z; z < xyz_end.z; z++) {
                if (in_range(TerrainOffset3(x, y, z))) {
                    Tile* tile = get_tile(x, y, z);
                    if (elements_can_stamp.has_value()) {
                        if (elements_can_stamp.value().material_in(
                                tile->get_material_id(), tile->get_color_id()
                            )) {
                            tile->set_material(
                                ter_->get_material(mat),
                                ter_->natural_color(
                                    TerrainOffset3(x, y, z) + get_offset(),
                                    ter_->get_material(mat), color_id
                                )
                            );
                        }
                    } else {
                        tile->set_material(
                            ter_->get_material(mat),
                            ter_->natural_color(
                                TerrainOffset3(x, y, z) + get_offset(),
                                ter_->get_material(mat), color_id
                            )
                        );
                    }
                }
            }
        }
    }
}

void
Chunk::init_nodegroups() {
    std::unordered_map<LocalPosition, NodeGroup&> map____({});

    // for all tiles:
    // x, y, z
    //      check 1) tile is not solid 2) tile below is 3) tile is not in map
    //          if any of the above are false continue
    //      create a nodegroup for the tile
    //      add tile to map____
    //      recursively add adjacent tiles to the nodegroup if they are open and in this
    //      chunk

    // initializing a node group on all walkable tiles
    for (uint8_t x = 0; x < SIZE; x++)
        for (uint8_t y = 0; y < SIZE; y++)
            for (uint8_t z = 0; z < SIZE; z++) {
                LocalPosition local_position(x, y, z);
                if (ter_->can_stand_1(get_offset() + TerrainOffset3(local_position))) {
                    if (map____.find(local_position) != map____.end()) {
                        continue;
                    }
                    // the int determines which paths between two tiles are
                    // compliant 31 means anything that is not opposite corner.
                    // look at onePath for more information
                    NodeGroup group(chunk_position_, local_position, 31);
                    node_groups_.push_back(std::move(group));
                    // ter_->add_node_group(&node_groups_.back());
                    map____.emplace(local_position, node_groups_.back());

                    std::unordered_set<TerrainOffset3> adjacent_tiles;
                    adjacent_tiles.emplace(
                        get_offset() + TerrainOffset3(local_position)
                    );

                    while (adjacent_tiles.size() > 0) {
                        auto iter = adjacent_tiles.begin();
                        TerrainOffset3 next_position = *iter;
                        adjacent_tiles.erase(iter);
                        path::AdjacentIterator it(
                            *ter_, next_position,
                            DirectionFlags::HORIZONTAL1 | DirectionFlags::HORIZONTAL2
                                | DirectionFlags::VERTICAL | DirectionFlags::UP_AND_OVER
                                | DirectionFlags::UP_AND_DIAGONAL | DirectionFlags::OPEN
                        );
                        for (; !it.end(); it++) {
                            TerrainOffset3 current_position = it.get_pos();
                            if (ter_->get_chunk_from_tile(current_position)
                                != chunk_position_) {
                                continue;
                            }
                            if (!ter_->can_stand_1(current_position)) {
                                continue;
                            }
                            LocalPosition iterator_local_position =
                                current_position - get_offset();
                            if (map____.find(iterator_local_position)
                                != map____.end()) {
                                continue;
                            }

                            node_groups_.back().add_tile(iterator_local_position);

                            map____.emplace(
                                iterator_local_position, node_groups_.back()
                            );

                            adjacent_tiles.insert(current_position);
                        }
                    }
                }
            }

    // need to lock here
    // modifying the length of nodegroups
    std::unique_lock terrain_lock(ter_->get_nodegroup_mutex());
    for (auto& [position, node_group] : map____) {
        ter_->add_node_group(&node_group);
    }
}

void
Chunk::add_nodegroup_adjacent_mp() {
    // add all adjacent nodegroup
    for (NodeGroup& NG : node_groups_)
        for (LocalPosition position : NG.get_local_positions()) {
            if (position.x != SIZE - 1 && position.y != SIZE - 1
                && position.z != SIZE - 1) {
                continue;
            }

            auto it = ter_->get_tile_adjacent_iterator(
                TerrainOffset3(position) + get_offset(), 31
            );
            for (; !it.end(); it++) {
                TerrainOffset3 iterator_position = it.get_pos();
                ChunkPos adjacent_chunk = ter_->get_chunk_from_tile(iterator_position);

                ChunkPos relative_position = adjacent_chunk - chunk_position_;

                if (relative_position.x * 4 + relative_position.y * 2
                        + relative_position.z
                    < 0) {
                    continue;
                }

                NodeGroup* to_add = ter_->get_node_group(it.get_pos());

                if (!to_add) {
                    continue;
                }
                NG.add_adjacent(to_add, 31);
            }
        }
}

void
Chunk::add_nodegroup_adjacent_all() {
    // add all adjacent nodegroup
    for (NodeGroup& NG : node_groups_)
        for (LocalPosition position : NG.get_tiles()) {
            if (position.x != SIZE - 1 || position.y != SIZE - 1
                || position.z != SIZE - 1 || position.x != 0 || position.y != 0
                || position.z != 0) {
                continue;
            }

            auto it = ter_->get_tile_adjacent_iterator(
                TerrainOffset3(position) + get_offset(), 31
            );
            for (; !it.end(); it++) {
                TerrainOffset3 iterator_position = it.get_pos();
                ChunkPos adjacent_chunk = ter_->get_chunk_from_tile(iterator_position);

                ChunkPos relative_position = adjacent_chunk - chunk_position_;

                if (relative_position == ChunkPos(0)) {
                    continue;
                }

                NodeGroup* to_add = ter_->get_node_group(it.get_pos());

                if (!to_add) {
                    continue;
                }
                NG.add_adjacent(to_add, 31);
            }
        }
}

void
Chunk::merge_(NodeGroup& G1, std::unordered_set<NodeGroup*> to_merge) {
    if (to_merge.size() == 0) {
        return;
    }
    while (to_merge.size() > 0) {
        auto G2 = to_merge.begin();
        std::unordered_map<NodeGroup*, UnitPath> to_add = G1.merge_groups(**(G2));
        delete_node_group_(**G2);
        for (std::pair<NodeGroup* const, UnitPath> NG : to_add) {
            if (contains_node_group_(NG.first) && &G1 != NG.first) {
                to_merge.insert(NG.first);
            }
        }
        to_merge.erase(G2);
    }
}

void
Chunk::add_nodes_to(std::unordered_set<const NodeGroup*>& out) const {
    for (auto it = node_groups_.begin(); it != node_groups_.end(); it++) {
        auto& elem = *it;
        out.insert(&elem); // Ptr to element
    }
}

void
Chunk::delete_node_group_(NodeGroup& NG) {
    // remove form ter. tile to group map
    ter_->remove_node_group(&NG);
    for (std::pair<NodeGroup* const, UnitPath>& adjacent : NG.get_adjacent_map()) {
        adjacent.first->remove_adjacent(&NG);
    }
    node_groups_.remove(NG);
}

void
Chunk::merge_node_group_(NodeGroup& G1, NodeGroup& G2) {
    G1.merge_groups(G2);
    delete_node_group_(G2);
    ter_->add_node_group(&G1);
}

inline bool
Chunk::contains_node_group_(NodeGroup* NG) {
    return (NG->get_chunk_position() == chunk_position_);
}

ColorInt
terrain::Chunk::get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const {
    return ter_->get_voxel(get_offset() + VoxelOffset(x, y, z));
}

MatColorId
terrain::Chunk::get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const {
    if (x >= Chunk::SIZE || x < 0 || y >= Chunk::SIZE || y < 0 || z >= Chunk::SIZE
        || z < 0) [[unlikely]] {
        return ter_->get_voxel_color_id(get_offset() + VoxelOffset(x, y, z));
    }
    return get_tile(x, y, z)->get_mat_color_id();
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
    // +1 because (-1,-1,-1) is in the zero vector position
    size_t position = ((x + 1) * (SIZE * SIZE)) + ((y + 1) * SIZE) + z + 1;
    return data_[position];
}

} // namespace terrain
