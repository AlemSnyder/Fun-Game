// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */
/**
 * @file chunk.hpp
 *
 * @brief Defines Chunk class.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "path/node_group.hpp"
#include "types.hpp"
#include "util/voxel.hpp"

#include <list>
#include <mutex>
#include <unordered_set>

namespace terrain {

class Terrain;

/**
 * @brief It's a chunk you've played minecraft.
 *
 * @details The chunk is used to define node groups that are used for
 * path finding. Chunk will also be used to generate meshes so that small
 * modifications can be made.
 */
class Chunk : public voxel_utility::VoxelBase {
    mutable std::mutex mut_;

    Terrain* ter_;

    // Chunk position. Incremented by 1 so multiply by
    // Chunk::SIZE to get tile position.
    const ChunkPos chunk_position_;

    // vector of voxels in terrain
    std::vector<Tile> tiles_;

    std::list<NodeGroup> node_groups_;

 public:
    static const Dim SIZE = 16; // number of tiles in each direction

    /**
     * @brief Construct a new Chunk object
     *
     * @param bx chunk x position
     * @param by chunk y position
     * @param bz chunk z position
     * @param ter the terrain this chunk is in
     */
    Chunk(Dim bx, Dim by, Dim bz, Terrain* ter) : Chunk({bx, by, bz}, ter){};

    Chunk(TerrainDim3 chunk_position, Terrain* ter);

    [[nodiscard]] inline std::mutex&
    get_mutex() const {
        return mut_;
    }

    [[nodiscard]] inline bool static in_range(TerrainOffset3 position) {
        return (
            position.x < SIZE && position.y < SIZE && position.z < SIZE
            && position.x >= 0 && position.y >= 0 && position.z >= 0
        );
    }

    void init_nodegroups();

    void add_nodegroup_adjacent_mp();

    void add_nodegroup_adjacent_all();

    /**
     * @brief adds node groups in this chunk to out
     *
     * @param out a set that gets the node groups in this chunk added to it
     */
    void add_nodes_to(std::unordered_set<const NodeGroup*>& out) const;

    /**
     * @brief Chunk position relative to other chunks
     *
     * @return ChunkPos
     */
    [[nodiscard]] inline ChunkPos
    get_chunk_position() const {
        return chunk_position_;
    }

    /**
     * @brief Used for getting mesh
     *
     * @return VoxelOffset offset of chunk in world space
     */
    [[nodiscard]] inline VoxelOffset
    get_offset() const {
        return VoxelOffset(chunk_position_) * TerrainOffset(Chunk::SIZE);
    }

    /**
     * @brief Get the size of a chunk
     *
     * @return std::vector<unsigned int> vector of Chunk::SIZE
     */
    [[nodiscard]] inline VoxelSize
    get_size() {
        return {Chunk::SIZE, Chunk::SIZE, Chunk::SIZE};
    }

    [[nodiscard]] inline Tile*
    get_tile(Dim x, Dim y, Dim z) {
        return const_cast<Tile*>(std::as_const(*this).get_tile(x, y, z));
    }

    [[nodiscard]] inline Tile*
    get_tile(TerrainDim3 tile_relative_position) {
        return get_tile(
            tile_relative_position.x, tile_relative_position.y, tile_relative_position.z
        );
    }

    [[nodiscard]] inline const Tile*
    get_tile(Dim x, Dim y, Dim z) const {
        size_t index = x * SIZE * SIZE + y * SIZE + z;
        return &tiles_[index];
    }

    [[nodiscard]] inline const Tile*
    get_tile(TerrainDim3 tile_relative_position) const {
        return get_tile(
            tile_relative_position.x, tile_relative_position.y, tile_relative_position.z
        );
    }

    void stamp_tile_region(
        MaterialId mat, ColorId color_id,
        std::optional<MaterialGroup> elements_can_stamp, LocalPosition xyz_start,
        LocalPosition xyz_end
    );

    // VoxelBase Specialization

    /**
     * @brief Get the color of a tile
     *
     * @param x x position in chunk
     * @param y y position in chunk
     * @param z z position in chunk
     * @return ColorInt tile color id
     */
    [[nodiscard]] ColorInt get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const;

    /**
     * @brief Get the voxel color
     *
     * @param position VoxelOffset position in chunk
     * @return ColorInt color as an integer
     */
    [[nodiscard]] ColorInt
    get_voxel(VoxelOffset position) const {
        return get_voxel(position.x, position.y, position.z);
    }

    /**
     * @brief Get the voxel color id
     *
     * @param x x position in chunk
     * @param y y position in chunk
     * @param z z position in chunk
     * @return MatColorId material and color id
     */
    [[nodiscard]] MatColorId
    get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const;

    /**
     * @brief Get the voxel color id
     *
     * @param position VoxelOffset position in chunk
     * @return MatColorId material and color id
     */
    [[nodiscard]] inline MatColorId
    get_voxel_color_id(VoxelOffset position) const {
        return get_voxel_color_id(position.x, position.y, position.z);
    }

    /**
     * @brief Get the colors used in terrain.
     *
     * @return const std::vector<ColorInt>&
     */
    [[nodiscard]] const inline std::vector<ColorInt>&
    get_color_ids() const {
        return TerrainColorMapping::get_color_ids_map();
    }

 private:
    void delete_node_group_(NodeGroup& NG);
    void merge_node_group_(NodeGroup& g1, NodeGroup& g2);
    bool contains_node_group_(NodeGroup*);
    void merge_(NodeGroup& g1, std::unordered_set<NodeGroup*> to_merge);
};

/**
 * @brief ChunkData save the voxel color id data such that is can be rapidly accessed.
 *
 * @details ChunkData is used in the mesher. Not only is the data in the chunk
 * important, but also the data that borders this chunk. The SIZE for ChunkData
 * is the SIZE for Chunk plus two, one for each edge.
 *
 * The total data saved in ChunkData is 4 * 18 * 18 * 18 ~ 23 kb, small enough
 * for a L1 cache. (I think I'm not an EE)
 */
class ChunkData : public voxel_utility::VoxelBase {
 public:
    static const VoxelDim SIZE = Chunk::SIZE + 2;
    const std::vector<MatColorId> data_;
    const VoxelOffset offset_;
    const std::vector<ColorInt>& color_ids_;

    [[nodiscard]] std::vector<MatColorId> get_mat_color_from_chunk(const Chunk& chunk);

    inline ChunkData(const Chunk& chunk) :
        data_(get_mat_color_from_chunk(chunk)), offset_(chunk.get_offset()),
        color_ids_(chunk.get_color_ids()){};

    /**
     * @brief Used for getting mesh
     *
     * @return VoxelOffset offset of chunk in world space
     */
    [[nodiscard]] inline VoxelOffset
    get_offset() const {
        return offset_;
    }

    /**
     * @brief Get the size of a chunk
     *
     * @return VoxelSize vector of Chunk::SIZE
     */
    [[nodiscard]] inline VoxelSize
    get_size() const {
        return {Chunk::SIZE, Chunk::SIZE, Chunk::SIZE};
    }

    /**
     * @deprecated This only exists for inheritance. Should not be used
     */
    [[deprecated]] [[nodiscard]] inline ColorInt
    get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const {
        return get_voxel_color_id(x, y, z);
    }

    // This is questionable, but no narrowing conversion
    /**
     * @deprecated This only exists for inheritance. Should not be used
     */
    [[deprecated]] [[nodiscard]] inline ColorInt
    get_voxel(VoxelOffset position) const {
        return get_voxel_color_id(position.x, position.y, position.z);
    }

    /**
     * @brief Get the voxel color id
     *
     * @param x x position in chunk
     * @param y y position in chunk
     * @param z z position in chunk
     * @return MatColorId material and color id
     */
    [[nodiscard]] MatColorId
    get_voxel_color_id(VoxelDim x, VoxelDim y, VoxelDim z) const;

    /**
     * @brief Get the voxel color id
     *
     * @param position VoxelOffset position in chunk
     * @return MatColorId material and color id
     */
    [[nodiscard]] inline MatColorId
    get_voxel_color_id(VoxelOffset position) const {
        return get_voxel_color_id(position.x, position.y, position.z);
    }

    /**
     * @brief Get the colors used in terrain.
     *
     * @return const std::vector<ColorInt>&
     */
    [[nodiscard]] const inline std::vector<ColorInt>&
    get_color_ids() const {
        return color_ids_;
    }
};

} // namespace terrain
