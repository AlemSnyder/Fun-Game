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
 * @file terrain.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines Terrain class
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "chunk.hpp"
#include "generation/land_generator.hpp"
#include "generation/map_tile.hpp"
#include "generation/noise.hpp"
#include "generation/terrain_map.hpp"
#include "generation/tile_stamp.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "path/node.hpp"
#include "path/node_group.hpp"
#include "path/tile_iterators.hpp"
#include "path/unit_path.hpp"
#include "terrain_helper.hpp"
#include "tile.hpp"
#include "types.hpp"
#include "util/voxel.hpp"
#include "util/voxel_io.hpp"
#include "world/biome.hpp"

#include <array>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace terrain {

namespace helper {

// high is for if the grass reaches a cliff
inline bool
edge_detector_high(Tile* t) {
    return !t->is_grass() && t->is_solid();
}

inline void
setter_high(Tile* t, int set_to) {
    t->set_grow_data_high(set_to);
}

inline int
getter_high(Tile* t) {
    return t->get_grow_data_high();
}

// low is for if the grass reaches an edge
inline bool
edge_detector_low(Tile* t) {
    return !t->is_solid();
}

inline void
setter_low(Tile* t, int set_to) {
    t->set_grow_data_low(set_to);
}

inline int
getter_low(Tile* t) {
    return t->get_grow_data_low();
}

} // namespace helper

/**
 * @brief The land in the world.
 *
 * @details Terrain holds all the tiles that exist. It also allows for
 * path-finding and its own generation.
 */
class Terrain : public voxel_utility::VoxelBase {
    friend class AdjacentIterator;

 private:
    // vector of voxels in terrain
    std::vector<Tile> tiles_;
    // size of terrain generation tile (see terrain generation)
    const Dim area_size_;
    // mat of material id to material that describes materials in this terrain
    const generation::Biome& biome_;

    std::vector<Chunk> chunks_;
    std::unordered_map<TileIndex, NodeGroup*> tile_to_group_;
    // seed for randomness
    int seed;

 public:
    // length in the x direction
    const Dim X_MAX;
    // length in the y direction
    const Dim Y_MAX;
    // length in the z direction
    const Dim Z_MAX;

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     * @return TileIndex
     */
    [[nodiscard]] inline TileIndex
    pos(Dim x, Dim y, Dim z) const noexcept {
        // for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param sop coordinate as an array
     * @return TileIndex
     */
    [[nodiscard]] inline TileIndex
    pos(const TerrainDim3& sop) const {
        return sop.x * Y_MAX * Z_MAX + sop.y * Z_MAX + sop.z;
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return TileIndex
     */
    [[nodiscard]] inline TileIndex
    pos(const Tile* const tile) const {
        return pos(tile->sop());
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return TileIndex
     */
    [[nodiscard]] inline TileIndex
    pos(const Tile tile) const {
        return pos(tile.get_x(), tile.get_y(), tile.get_z());
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos_for_map(const Tile tile) const {
        return pos(tile);
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos_for_map(const Tile* const tile) const {
        return pos(tile);
    }

    /**
     * @brief return position in space of given vector index
     *
     * @param xyz vector index
     * @return const TerrainDim3 position in space
     */
    [[nodiscard]] inline const TerrainDim3
    sop(TileIndex xyz) const {
        return {
            static_cast<Dim>(xyz / (Y_MAX * Z_MAX)),
            static_cast<Dim>((xyz / Z_MAX) % Y_MAX), static_cast<Dim>(xyz % (Z_MAX))};
    }

    /**
     * @brief return position in space of given index
     *
     * @param xyz index
     * @param xm length in x direction
     * @param ym length in y direction
     * @param zm length in z direction
     * @return TerrainDim3 position in 3D space
     */
    [[nodiscard]] inline static TerrainDim3
    sop(TileIndex xyz, TileIndex xm, TileIndex ym, TileIndex zm) {
        if (xyz >= xm * ym * zm) {
            throw std::invalid_argument("index out of range");
        }
        return {
            static_cast<Dim>(xyz / (ym * zm)), static_cast<Dim>((xyz / zm) % ym),
            static_cast<Dim>(xyz % (zm))};
    }

    /**
     * @brief Get the size of terrain
     *
     * @details overloaded so must use base class definition.
     *
     * @return VoxelSize array of sizes
     */
    [[nodiscard]] inline VoxelSize
    get_size() const noexcept {
        return {X_MAX, Y_MAX, Z_MAX};
    }

    /**
     * @brief Used for getting mesh
     *
     * @return glm::i32vec3 0 3 times
     */
    [[nodiscard]] inline VoxelOffset
    get_offset() const noexcept {
        return {0, 0, 0};
    }

    /**
     * @brief test if tile position is within terrain bounds
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return true tile is in bounds
     * @return false tile is not in bounds
     */
    [[nodiscard]] inline bool
    in_range(TerrainOffset x, TerrainOffset y, TerrainOffset z) const {
        return (
            static_cast<Dim>(x) < X_MAX && x >= 0 && static_cast<Dim>(y) < Y_MAX
            && y >= 0 && static_cast<Dim>(z) < Z_MAX && z >= 0
        );
    }

    [[nodiscard]] inline bool
    in_range(TerrainOffset3 xyz) const {
        return (xyz.x < X_MAX && xyz.y < Y_MAX && xyz.z < Z_MAX);
    }

    /**
     * @brief Get the tile object at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     *
     * @return Tile* tile at given position
     */
    [[nodiscard]] inline Tile*
    get_tile(TerrainOffset x, TerrainOffset y, TerrainOffset z) {
        return const_cast<Tile*>(std::as_const(*this).get_tile(x, y, z));
    };

    [[nodiscard]] inline Tile*
    get_tile(TileIndex xyz) {
        return const_cast<Tile*>(std::as_const(*this).get_tile(xyz));
    }

    /**
     * @brief Get the tile at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return Tile* tile at given position
     */
    [[nodiscard]] inline const Tile*
    get_tile(TerrainOffset x, TerrainOffset y, TerrainOffset z) const {
        if (!in_range(x, y, z)) {
            LOG_CRITICAL(
                logging::terrain_logger, "Tile position ({}, {}, {}), out of range.", x,
                y, z
            );
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[pos(
            static_cast<Dim>(x), static_cast<Dim>(y), static_cast<Dim>(z)
        )];
    };

    /**
     * @brief Get the tile at the given index
     *
     * @param xyz tile index
     * @return const Tile* tile at index
     */
    [[nodiscard]] inline const Tile*
    get_tile(TileIndex xyz) const {
        if (xyz >= X_MAX * Y_MAX * Z_MAX) {
            LOG_CRITICAL(logging::terrain_logger, "Tile index {}, out of range.", xyz);
            throw std::invalid_argument("index out of range");
        }
        return &tiles_[xyz];
    }

    /**
     * @brief Get the color of a tile
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return ColorInt color or tile
     */
    [[nodiscard]] inline ColorInt
    get_voxel(VoxelDim x, VoxelDim y, VoxelDim z) const {
        // using static ints to prevent dereferencing
        // The previous material id, and color id are cached so that materials do
        // not need to be dereferenced, and searched through.
        static MaterialId previous_mat_id = 0;
        static ColorId previous_color_id = 0;
        static ColorInt previous_out_color = 0;

        if (!in_range(x, y, z))
            return 0;

        MaterialId mat_id = tiles_[pos(x, y, z)].get_material_id();
        ColorId color_id = tiles_[pos(x, y, z)].get_color_id();

        if (mat_id != previous_mat_id || color_id != previous_color_id) {
            previous_mat_id = mat_id;
            previous_color_id = color_id;

            auto mat = biome_.get_materials().at(previous_mat_id);
            previous_out_color = mat.color[previous_color_id].hex_color;
        }

        return previous_out_color;
    }

    [[nodiscard]] inline ColorInt
    get_voxel(VoxelOffset position) const {
        return get_voxel(position.x, position.y, position.z);
    }

    [[nodiscard]] inline VoxelColorId
    get_voxel_color_id(Dim x, Dim y, Dim z) const {
        // if not in range, then considered to be air
        if (!in_range(x, y, z))
            return 0;
        // use mat color id for voxel color id because the tile has that data
        // it is therefore just a look up. (pointer dereference)
        return get_tile(x, y, z)->get_mat_color_id();
    }

    [[nodiscard]] inline VoxelColorId
    get_voxel_color_id(VoxelOffset position) const {
        return get_voxel_color_id(position.x, position.y, position.z);
    }

    /**
     * @brief Get the heights z thats material is not in materials
     *
     * @param materials materials to exclude
     * @param x x position
     * @param y y position
     * @param guess expected height (for speed)
     *
     * @return int height
     */
    [[nodiscard]] Dim
    get_first_not(const MaterialGroup& materials, Dim x, Dim y, Dim guess = 0) const;

    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return biome_.get_grass_grad_length();
    }

    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return biome_.get_grass_mid();
    }

    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return biome_.get_grass_colors();
    }

    [[nodiscard]] inline const std::unordered_map<
        MaterialId, const terrain::material_t>&
    get_materials() const {
        return biome_.get_materials();
    }

    /**
     * @brief initialize area of terrain
     *
     * @param area_x area x coordinate
     * @param area_y area y coordinate
     * @param gen Generator object that generates tile types
     */
    void init_area(generation::MapTile& map_tile, generation::LandGenerator gen);

    /**
     * @brief Set a group of tiles
     *
     * @param tStamp where the stamp is, and material and color
     * @param x macro map x position
     * @param y macro map y position
     */
    inline void stamp_tile_region(const generation::TileStamp& tStamp, int x, int y);

    /**
     * @brief add material on top of extant voxels
     *
     * @param to_data json data determines number of layers, and where voxels
     * are added
     * @param material material type to add
     */
    void add_to_top(const generation::AddToTop& to_data);

    [[nodiscard]] inline bool
    has_tile_material(const MaterialGroup& material_test, Dim x, Dim y, Dim z) const {
        return has_tile_material(material_test, get_tile(x, y, z));
    }

    [[nodiscard]] inline bool
    has_tile_material(const MaterialGroup& material_test, const Tile* tile) const {
        MaterialId mat_id = tile->get_material_id();
        ColorId color_id = tile->get_color_id();

        return material_test.material_in(mat_id, color_id);
    }

    // test for path finding
    std::pair<const Tile*, const Tile*> get_start_end_test() const;

    /**
     * @brief Get the UnitPath defined by the path type between two tiles
     * @param xs X start
     * @param ys Y start
     * @param zs Z start
     * @param xf X end
     * @param yf Y end
     * @param zf Z end
     * @return const UnitPath path type between the two tile positions
     */
    [[nodiscard]] const UnitPath
    get_path_type(int xs, int ys, int zs, int xf, int yf, int zf) const;

    [[nodiscard]] inline const UnitPath
    get_path_type(TerrainDim3 start_position, TerrainDim3 final_position) const {
        return get_path_type(
            start_position.x, start_position.y, start_position.z, final_position.x,
            final_position.y, final_position.z
        );
    }

    /**
     * @brief Get the minimum path time between two positions
     *
     * @param xyz1 position 1
     * @param xyz2 position 2
     * @return float time between positions
     */
    [[nodiscard]] static float
    get_H_cost(std::array<float, 3> xyz1, std::array<float, 3> xyz2);
    /**
     * @brief Get the minimum path time between two positions
     *
     * @param xyz1 position 1
     * @param xyz2 position 2
     * @return float time between positions
     */
    [[nodiscard]] static float get_H_cost(TerrainDim3 xyz1, TerrainDim3 xyz2);
    /**
     * @brief Get time required to get to node from start plus time required
     * to get from node to tile
     *
     * @tparam T Type of underlying position
     * @param tile final position of path
     * @param node nodes path can pass through
     * @return float time required
     */
    template <class T>
    [[nodiscard]] static float get_G_cost(const T tile, const Node<const T> node);

    /**
     * @brief position of chunk the node group is a part of
     *
     * @param node_group node group to find position of chunk
     * @return ChunkIndex chunk pos
     */
    [[nodiscard]] ChunkIndex pos(const NodeGroup* const node_group) const;

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos_for_map(const NodeGroup NG) const {
        return pos(*(NG.get_tiles().begin()));
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    [[nodiscard]] inline TileIndex
    pos_for_map(const NodeGroup* const NG) const {
        return pos(*(NG->get_tiles().begin()));
    }

    /**
     * @brief Construct a new Terrain object
     *
     * @param x_tiles number of macro tiles in x direction
     * @param y_tiles number of macro tiles in y direction
     * @param Area_size_ size of a macro map tile
     * @param z_tiles number of voxel tiles in z direction
     * @param seed seed of random number generator
     * @param material set of materials used in the world
     * @param biome_data json data that contains biome data
     * @param grass_grad_data vector that determines grass color from edge
     * distance
     * @param grass_mid gradient index of grass not effected by an edge
     */
    Terrain(
        Dim x_tiles, Dim y_tiles, Dim area_size_, Dim z_tiles, int seed,
        const generation::Biome& biome, const generation::TerrainMacroMap& macro_map
    );
    /**
     * @brief Construct a new Terrain object
     *
     * @param path path to saved terrain
     * @param material materials of the world
     */
    Terrain(const std::string& path, const generation::Biome& biome);

    /**
     * @brief Construct a new Terrain Base object from qb data
     *
     * @details This constructor loads a terrain object from a file
     *
     * @param materials Materials that make up the terrain
     * @param grass_grad_data grass gradient data
     * @param grass_mid position in grass gradient data that denotes the middle
     * @param data qb_data_t read from file
     */
    Terrain(const generation::Biome& biome, voxel_utility::qb_data_t data);

    // TODO place block

    using iterator = path::AdjacentIterator;

    /**
     * @brief Get a tile iterator to adjacent tiles
     *
     * @param pos position of
     * @param path_type UnitPath defining acceptable paths
     *
     * @return iterator
     */
    [[nodiscard]] inline iterator
    get_tile_adjacent_iterator(size_t pos, UnitPath path_type = 127U) const {
        return iterator(*this, pos, path_type);
    }

    /**
     * @brief Get the nodes adjacent to this one
     *
     * @param node node to find the adjacent of
     * @param nodes nodes that can be passed through
     * @param type path type allowed
     * @return std::unordered_set<Node<const T> *> adjacent nodes
     */
    [[nodiscard]] std::unordered_set<Node<const NodeGroup>*> get_adjacent_nodes(
        const Node<const NodeGroup>* const node,
        std::unordered_map<TileIndex, Node<const NodeGroup>>& nodes, path_t type
    ) const;

    /**
     * @brief Get the nodes adjacent to this one
     *
     * @param node node to find the adjacent of
     * @param nodes nodes that can be passed through
     * @param type path type allowed
     * @return std::unordered_set<Node<const T> *> adjacent nodes
     */
    [[nodiscard]] std::unordered_set<Node<const Tile>*> get_adjacent_nodes(
        const Node<const Tile>* const node,
        std::unordered_map<TileIndex, Node<const Tile>>& nodes, path_t type
    ) const;

    /**
     * @brief Get the node group from tile index
     *
     * @param xyz tile index in vector tiles
     * @return NodeGroup* NodeGroup tile is in
     */
    [[nodiscard]] NodeGroup* get_node_group(TileIndex xyz);
    /**
     * @brief Get the node group from tile
     *
     * @param t tile
     *
     * @return NodeGroup* NodeGroup tile is in
     */
    [[nodiscard]] NodeGroup* get_node_group(const Tile t);
    /**
     * @brief Get the node group from tile
     *
     * @param t
     *
     * @return NodeGroup* NodeGroup tile is in
     */
    [[nodiscard]] NodeGroup* get_node_group(const Tile* t);

    /**
     * @brief Get the node group from tile index
     *
     * @param xyz tile index in vector tiles
     * @return NodeGroup* NodeGroup tile is in
     */
    [[nodiscard]] const NodeGroup* get_node_group(TileIndex xyz) const;
    /**
     * @brief Get the node group from tile
     *
     * @param t tile
     * @return NodeGroup* NodeGroup tile is in
     */
    [[nodiscard]] const NodeGroup* get_node_group(const Tile t) const;
    /**
     * @brief Get the node group from tile
     *
     * @param t
     * @return NodeGroup* NodeGroup tile is in
     */
    [[nodiscard]] const NodeGroup* get_node_group(const Tile* t) const;

    /**
     * @brief Add a node group to possible node groups
     *
     * @param NG NodeGroup to add
     */
    void add_node_group(NodeGroup* NG);
    /**
     * @brief Remove a node group
     *
     * @param NG NodeGroup to remove
     */
    void remove_node_group(NodeGroup* NG);

    inline ChunkIndex
    get_chunk_from_tile(TileIndex pos) const {
        TerrainDim3 tile_sop = sop(pos);
        return get_chunk_from_tile(tile_sop.x, tile_sop.y, tile_sop.z);
    }

    inline ChunkIndex
    get_chunk_from_tile(TerrainDim3 tile_sop) const {
        return get_chunk_from_tile(tile_sop.x, tile_sop.y, tile_sop.z);
    }

    ChunkIndex get_chunk_from_tile(Dim x, Dim y, Dim z) const;

    [[nodiscard]] inline const std::vector<Chunk>&
    get_chunks() const {
        return chunks_;
    }

    [[nodiscard]] inline const Chunk&
    get_chunk(size_t i) const {
        assert(i < chunks_.size());
        return chunks_[i];
    }

    /**
     * @brief charge the color id but not the material of the tile
     *
     * @param tile the tile to change
     * @param mat material to set
     * @param color_id color to set
     * @return true successful change materials is the same
     * @return false unsuccessful change materials is different
     */
    bool paint(Tile* tile, const material_t* mat, ColorId color_id);
    /**
     * @brief add or remove tile, only works when either previous of materials
     * or set material is air
     *
     * @param xyz tile index
     * @param mat materials to set
     * @param color_id color id to set
     * @return true success
     * @return false failure
     */
    bool
    player_set_tile_material(TileIndex xyz, const material_t* mat, ColorId color_id);

    /**
     * @brief Set the tile material with no tests
     *
     * @param tile tile to set materials, and color
     * @param mat materials set to
     *
     * @param color_id color id set to
     */
    inline void
    set_tile_material(Tile* tile, const material_t* mat, ColorId color_id) {
        tile->set_material(mat, color_id);
    }

    /**
     * @brief initialize grass
     *
     */
    void init_grass();

    /**
     * @brief set the upper bound for grass color
     *
     * @param all_grass set of pointers to tiles that are grass
     */
    inline void
    grow_grass_high(std::unordered_set<Tile*> all_grass) {
        helper::grow_grass_recursive<
            helper::edge_detector_high, helper::getter_high, helper::setter_high>(
            *this, all_grass
        );
    }

    /**
     * @brief set the lower bound for grass color
     *
     * @param all_grass set of pointers to tiles that are grass
     */
    inline void
    grow_grass_low(std::unordered_set<Tile*> all_grass) {
        helper::grow_grass_recursive<
            helper::edge_detector_low, helper::getter_low, helper::setter_low>(
            *this, all_grass
        );
    }

    /**
     * @brief test if 1 x 1 x 1 object can stand at the position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     *
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] inline bool
    can_stand_1(TerrainOffset x, TerrainOffset y, TerrainOffset z) const {
        return can_stand(x, y, z, 1, 1);
    }

    /**
     * @brief test if 1 x 1 x 1 object can stand at given index
     *
     * @param xyz index
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] bool can_stand_1(TileIndex xyz
    ) const; // this is fast, and used for looping

    // Ok so basically when running through a loop the cpu moves a large chunk
    // of memory that is close together into the cpu's memory, then this
    // function iterates over memory space, rather then coordinate space.
    // someone should test if this is true.
    /**
     * @brief test if 1 x 1 x 1 object can stand at given tile
     *
     * @param tile tile to test
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] inline bool
    can_stand_1(const Tile tile) const {
        return can_stand(tile, 1, 1);
    }

    /**
     * @brief test if 1 x 1 x 1 object can stand at given tile
     *
     * @param tile tile to test
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] inline bool
    can_stand_1(const Tile* tile) const {
        return can_stand(tile, 1, 1);
    }

    /**
     * @brief test if dxy x dyx x dz object can stand at given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @param dz height of object to test
     * @param dxy width of object to test
     *
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] bool can_stand(
        TerrainOffset x, TerrainOffset y, TerrainOffset z, TerrainOffset dz,
        TerrainOffset dxy
    ) const;

    /**
     * @brief test if dxy x dyx x dz object can stand at given tile
     *
     * @param tile tile to test
     * @param dz height of object to test
     * @param dxy width of object to test
     *
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] bool
    can_stand(const Tile tile, TerrainOffset dz, TerrainOffset dxy) const;

    /**
     * @brief test if dxy x dyx x dz object can stand at given tile
     *
     * @param tile tile to test
     * @param dz height of object to test
     * @param dxy width of object to test
     *
     * @return true can stand
     * @return false cannot stand
     */
    [[nodiscard]] bool
    can_stand(const Tile* tile, TerrainOffset dz, TerrainOffset dxy) const;

    void qb_read(
        std::vector<ColorInt> data,
        const std::unordered_map<ColorInt, MaterialColor>& materials_inverse
    );

    /**
     * @brief save with debug visible
     *
     * @param path path to save to
     * @param materials materials in the terrain for debug materials
     */
    void qb_save_debug(const std::string path);

    /**
     * @brief save to path
     *
     * @param path path to save to
     */
    void qb_save(const std::string path) const;

    /**
     * @brief get all nod groups
     *
     * @return std::unordered_set<const NodeGroup> set of all NodeGroups
     */
    [[nodiscard]] std::unordered_set<const NodeGroup*> get_all_node_groups() const;

    /**
     * @brief Get a path between start, and goal using the A* algorithm
     *
     * @param start start tile
     * @param goal end tile
     * @return std::vector<const Tile *> path
     */
    [[nodiscard]] std::optional<std::vector<const Tile*>>
    get_path_Astar(const Tile* start, const Tile* goal) const;

    /**
     * @brief Get a path between start, and goal using the A* algorithm
     *
     * @param start start NodeGroup
     * @param goal end NodeGroup
     * @return std::optional<std::vector<const NodeGroup*>> path
     */
    [[nodiscard]] std::optional<std::vector<const NodeGroup*>>
    get_path_Astar(const NodeGroup* start, const NodeGroup* goal) const;

    /**
     * @brief Get a path between start, and any goal using the breadth first
     * algorithm
     *
     * @param start start tile
     * @param goal set of excitable goals
     * @return std::optional<std::vector<const Tile*>> path to closest goal
     */
    [[nodiscard]] std::optional<std::vector<const Tile*>> get_path_breadth_first(
        const Tile* start, const std::unordered_set<const Tile*> goal
    );

    /**
     * @brief Get a path between start, and any goal using the breadth first
     * algorithm
     *
     * @param start start NodeGroup
     * @param goal set of excitable goals
     * @return std::optional<std::vector<const NodeGroup*>> path to closest goal
     */
    [[nodiscard]] std::optional<std::vector<const NodeGroup*>> get_path_breadth_first(
        const NodeGroup* start, const std::unordered_set<const NodeGroup*> goal
    ) const;

    /**
     * @brief Get the path from start to a goal optimized by compare
     *
     * @tparam T position type
     * @param start start position
     * @param goal goal positions
     * @param search_through available positions for path
     * @param compare way to sort best path
     * @return std::vector<const T *> path optimized by compare
     */
    template <class T, bool compare(Node<const T>*, Node<const T>*)>
    [[nodiscard]] std::optional<std::vector<const T*>> get_path(
        const T* start, const std::unordered_set<const T*> goal,
        const std::unordered_set<const T*> search_through
    ) const;

    /**
     * @brief initialize chunks
     *
     */
    void init_chunks();

    /**
     * @brief Get the hightest solid z at the x, y quadrates
     *
     * @param x x position
     * @param y y position
     *
     * @return TerrainOffset height of heights solid z
     */
    [[nodiscard]] TerrainOffset get_Z_solid(TerrainOffset x, TerrainOffset y) const;

    /**
     * @brief Get the hightest solid z below the given z
     *
     * @param x x position
     * @param y y position
     * @param z z height
     * @return TerrainOffset height of heights solid z
     */
    [[nodiscard]] TerrainOffset
    get_Z_solid(TerrainOffset x, TerrainOffset y, TerrainOffset z) const;

 private:
    // TODO This is probably the least safe function that could possibly exist
    // trace nodes through parents to reach start
    template <class T>
    void
    get_path_through_nodes(
        Node<const T>* node, std::vector<const T*>& out, const T* start
    ) const {
        out.push_back(node->get_tile());
        if (start == node->get_tile()) {
            return;
        }
        get_path_through_nodes(node->get_parent(), out, start);
    }
};

} // namespace terrain
