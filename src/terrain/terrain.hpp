// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
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

#include "../logging.hpp"
#include "../util/voxel.hpp"
#include "../util/voxel_io.hpp"
#include "chunk.hpp"
#include "material.hpp"
#include "path/node.hpp"
#include "path/node_group.hpp"
#include "path/tile_iterators.hpp"
#include "path/unit_path.hpp"
#include "terrain_generation/land_generator.hpp"
#include "terrain_generation/noise.hpp"
#include "terrain_generation/tilestamp.hpp"
#include "terrain_helper.hpp"
#include "terrain_base.hpp"
#include "tile.hpp"

#include <stdio.h>

#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace terrain {

// Forward declaration of Chunk
class Chunk;

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
 *
 */
class Terrain : voxel_utility::VoxelBase{
    friend class AdjacentIterator;
    friend class Chuck;

 private:

    TerrainBase terrain_base_;
    std::vector<Chunk> chunks_;
    std::map<int, NodeGroup*> tile_to_group_;
    // seed for randomness
    int seed;

 public:
    // test for path finding
    std::pair<Tile*, Tile*> get_start_end_test();

    /**
     * @brief Get the UnitPath defined by the path type between two tiles
     *
     * @param xs X start
     * @param ys Y start
     * @param zs Z start
     * @param xf X end
     * @param yf Y end
     * @param zf Z end
     * @return const UnitPath path type between the two tile positions
     */
    const UnitPath get_path_type(int xs, int ys, int zs, int xf, int yf, int zf) const;
    /**
     * @brief Get the minimum path time between two positions
     *
     * @param xyz1 position 1
     * @param xyz2 position 2
     * @return float time between positions
     */
    static float get_H_cost(std::array<float, 3> xyz1, std::array<float, 3> xyz2);
    /**
     * @brief Get the minimum path time between two positions
     *
     * @param xyz1 position 1
     * @param xyz2 position 2
     * @return float time between positions
     */
    static float get_H_cost(std::array<int, 3> xyz1, std::array<int, 3> xyz2);
    /**
     * @brief Get time required to get to node from start plus time required to
     * get from node to tile
     *
     * @tparam T Type of underlying position
     * @param tile final position of path
     * @param node place path passes through
     * @return float time required
     */
    template <class T>
    static float get_G_cost(const T tile, const Node<const T> node);

    //using TerrainBase::pos;
    //using TerrainBase::pos_for_map;

    /**
     * @brief position of chunk the node group is a part of
     *
     * @param node_group node group to find position of chunk
     * @return int
     */
    int pos(const NodeGroup* const node_group) const;

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    inline int
    pos_for_map(const NodeGroup NG) const {
        return terrain_base_.pos(*(NG.get_tiles().begin()));
    }

    /**
     * @brief unique map index
     *
     * @param tile
     * @return int
     */
    inline int
    pos_for_map(const NodeGroup* const NG) const {
        return terrain_base_.pos(*(NG->get_tiles().begin()));
    }

    /**
     * @brief generates a 2D 'height' map to use to generate the terrain
     *
     * @param size_x number of tile types in x direction
     * @param size_y number of tile types in y direction
     * @param map_data json data on how to generate map
     * @return std::vector<int> (size_x * size_y) vector of ints
     */
    static std::vector<int>
    generate_macro_map(unsigned int size_x, unsigned int size_y, Json::Value map_data);
    /**
     * @brief add material on top of extant voxels
     *
     * @param to_data json data determines number of layers, and where voxels
     * are added
     * @param material material type to add
     */
    void add_to_top(Json::Value to_data, const std::map<int, const Material>* material);
    /**
     * @brief Get the max allowable height of added material
     *
     * @param height height of terrain in question
     * @param how_to_add json data that defines biome generation
     * @return int max height
     */
    static int get_stop_height(int height, const Json::Value how_to_add);
    /**
     * @brief initialize area of terrain
     *
     * @param area_x area x coordinate
     * @param area_y area y coordinate
     * @param gen Generator object that generates tile types
     */
    void init_area(int area_x, int area_y, terrain_generation::LandGenerator gen);
    /**
     * @brief initialized terrain
     * @deprecated should only be used to implicitly initialize terrain
     *
     * @param x size in x direction
     * @param y size in y direction
     * @param z size in z direction
     */
    void init(int x, int y, int z);

    // TODO area_size should not be initialized like this
    /**
     * @brief Terrain initializer
     *
     * @param x number of tile areas in x direction
     * @param y number of tile areas in y direction
     * @param area_size size of each area
     * @param z height of terrain
     * @param seed seed for random number generation
     * @param materials materials used in this terrain
     * @param biome_data data on how to generate a biome
     * @param Terrain_Maps macro map defines witch tile types go where
     */
    void init(
        int x, int y, int Area_size_, int z, int seed,
        const std::map<int, const Material>* materials, Json::Value biome_data,
        std::vector<int> Terrain_Maps
    );
    /**
     * @brief Terrain initializer for biome test
     *
     * @param x_tiles number of macro tiles in x direction
     * @param y_tiles number of macro tiles in y direction
     * @param Area_size_ size of a macro map tile
     * @param z_tiles number of voxel tiles in z direction
     * @param seed seed of random number generator
     * @param tile_type id of map tile type
     * @param material set of materials used in the world
     * @param biome_data json data that contains biome data
     */
    void init(
        int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type,
        const std::map<int, const Material>* material, Json::Value biome_data
    );
    /**
     * @brief Construct a new Terrain object (default constructor)
     *
     */
    Terrain();
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
        int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed,
        const std::map<int, const Material>* material, Json::Value biome_data,
        std::vector<int> grass_grad_data, unsigned int grass_mid
    );
    /**
     * @brief Construct a new Terrain object
     *
     * @param x_tiles number of macro tiles in x direction
     * @param y_tiles number of macro tiles in y direction
     * @param Area_size_ size of a macro map tile
     * @param z_tiles number of voxel tiles in z direction
     * @param seed seed of random number generator
     * @param tile_type id of map tile type
     * @param material set of materials used in the world
     * @param biome_data json data that contains biome data
     */
    Terrain(
        int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type,
        const std::map<int, const Material>* material, Json::Value biome_data
    );
    /**
     * @brief Construct a new Terrain object
     *
     * @param path path to saved terrain
     * @param material materials of the world
     */
    Terrain(const std::string path, const std::map<int, const Material>* material);

    // TODO place block

    [[nodiscard]] inline TerrainBase* get_base(){
        return &terrain_base_;
    }

    using iterator = path::AdjacentIterator;

    /**
     * @brief Get a tile iterator to adjacent tiles
     *
     * @param pos position of
     * @param path_type UnitPath defining acceptable paths
     * @return iterator
     */
    inline iterator
    get_tile_adjacent_iterator(size_t pos, UnitPath path_type = 127U) const {
        return iterator(*this, pos, path_type);
    }

    /**
     * @brief Get the nodes adjacent to this one
     *
     * @param node node to find the adjacent of
     * @param nodes nodes that can be passed through
     * @param type path type allowed
     * @return std::set<Node<const T> *> adjacent nodes
     */
    std::set<Node<const NodeGroup>*> get_adjacent_nodes(
        const Node<const NodeGroup>* const node,
        std::map<size_t, Node<const NodeGroup>>& nodes, uint8_t type
    ) const;

    /**
     * @brief Get the nodes adjacent to this one
     *
     * @param node node to find the adjacent of
     * @param nodes nodes that can be passed through
     * @param type path type allowed
     * @return std::set<Node<const T> *> adjacent nodes
     */
    std::set<Node<const Tile>*> get_adjacent_nodes(
        const Node<const Tile>* const node, std::map<size_t, Node<const Tile>>& nodes,
        uint8_t type
    ) const;

    /**
     * @brief Get the node group from tile index
     *
     * @param xyz tile index in vector tiles
     * @return NodeGroup* NodeGroup tile is in
     */
    NodeGroup* get_node_group(int xyz);
    /**
     * @brief Get the node group from tile
     *
     * @param t tile
     * @return NodeGroup* NodeGroup tile is in
     */
    NodeGroup* get_node_group(const Tile t);
    /**
     * @brief Get the node group from tile
     *
     * @param t
     * @return NodeGroup* NodeGroup tile is in
     */
    NodeGroup* get_node_group(const Tile* t);
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

    inline const std::vector<Chunk>&
    get_chunks() const {
        return chunks_;
    }

    inline int
    get_grass_grad_length() const {
        return terrain_base_.grass_grad_length_;
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
    bool paint(Tile* tile, const Material* mat, uint8_t color_id);
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
    bool player_set_tile_material(int xyz, const Material* mat, uint8_t color_id);

    /**
     * @brief Set the tile material with no tests
     *
     * @param tile tile to set materials, and color
     * @param mat materials set to
     * @param color_id color id set to
     */
    inline void
    set_tile_material(Tile* tile, const Material* mat, uint8_t color_id) {
        tile->set_material(mat, color_id);
    }

    /**
     * @brief Set a group of tiles
     *
     * @param x_start lower x position
     * @param y_start lower y position
     * @param z_start lower z position
     * @param x_end greater x position
     * @param y_end greater y position
     * @param z_end greater z position
     * @param mat materials to set
     */
    void set_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const Material* mat
    );

    /**
     * @brief Set a group of tiles
     *
     * @param tStamp where the stamp is, and material and color
     * @param x macro map x position
     * @param y macro map y position
     */
    inline void
    stamp_tile_region(terrain_generation::TileStamp tStamp, int x, int y) {
        int32_t Area_size = terrain_base_.Area_size;
        stamp_tile_region(
            tStamp.x_start + x * Area_size + Area_size / 2,
            tStamp.y_start + y * Area_size + Area_size / 2, tStamp.z_start,
            tStamp.x_end + x * Area_size + Area_size / 2,
            tStamp.y_end + y * Area_size + Area_size / 2, tStamp.z_end, tStamp.mat,
            tStamp.elements_can_stamp, tStamp.color_id
        );
    }

    /**
     * @brief Set a group of tiles
     *
     * @param x_start lower x position
     * @param y_start lower y position
     * @param z_start lower z position
     * @param x_end greater x position
     * @param y_end greater y position
     * @param z_end greater z position
     * @param mat materials to set
     * @param color_id color id to set
     */
    void stamp_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const Material* mat, uint8_t color_id
    );
    /**
     * @brief Set a group of tiles
     *
     * @param x_start lower x position
     * @param y_start lower y position
     * @param z_start lower z position
     * @param x_end greater x position
     * @param y_end greater y position
     * @param z_end greater z position
     * @param mat materials to set
     * @param elements_can_stamp type of material that can be changed
     * @param color_id color id to set
     */
    void stamp_tile_region(
        int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
        const Material* mat, std::set<std::pair<int, int>> elements_can_stamp,
        uint8_t color_id
    );
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
    grow_grass_high(std::set<Tile*> all_grass) {
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
    grow_grass_low(std::set<Tile*> all_grass) {
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
     * @return true can stand
     * @return false cannot stand
     */
    inline bool
    can_stand_1(int x, int y, int z) const {
        return can_stand(x, y, z, 1, 1);
    }

    /**
     * @brief test if 1 x 1 x 1 object can stand at given index
     *
     * @param xyz index
     * @return true can stand
     * @return false cannot stand
     */
    bool can_stand_1(int xyz) const; // this is fast, and used for looping

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
    inline bool
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
    bool
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
     * @return true can stand
     * @return false cannot stand
     */
    bool can_stand(int x, int y, int z, int dz, int dxy) const;
    /**
     * @brief test if dxy x dyx x dz object can stand at given tile
     *
     * @param tile tile to test
     * @param dz height of object to test
     * @param dxy width of object to test
     * @return true can stand
     * @return false cannot stand
     */
    bool can_stand(const Tile tile, int dz, int dxy) const;
    /**
     * @brief test if dxy x dyx x dz object can stand at given tile
     *
     * @param tile tile to test
     * @param dz height of object to test
     * @param dxy width of object to test
     * @return true can stand
     * @return false cannot stand
     */
    bool can_stand(const Tile* tile, int dz, int dxy) const;
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
     * @brief read from path
     *
     * @param path path to read from
     * @param materials materials in the terrain
     */
    void qb_read(
        const std::string path,
        const std::map<uint32_t, std::pair<const Material*, uint8_t>>* materials
    );
    /**
     * @brief get all nod groups
     *
     * @return std::set<const NodeGroup *> set of all NodeGroups
     */
    std::set<const NodeGroup*> get_all_node_groups() const;
    /**
     * @brief Get a path between start, and goal using the A* algorithm
     *
     * @param start start tile
     * @param goal end tile
     * @return std::vector<const Tile *> path
     */
    std::vector<const Tile*> get_path_Astar(const Tile* start, const Tile* goal);
    /**
     * @brief Get a path between start, and goal using the A* algorithm
     *
     * @param start start NodeGroup
     * @param goal end NodeGroup
     * @return std::vector<const NodeGroup *> path
     */
    std::vector<const NodeGroup*>
    get_path_Astar(const NodeGroup* start, const NodeGroup* goal);
    /**
     * @brief Get a path between start, and any goal using the breadth first algorithm
     *
     * @param start start tile
     * @param goal set of excitable goals
     * @return std::vector<const Tile *> path to closest goal
     */
    std::vector<const Tile*>
    get_path_breadth_first(const Tile* start, const std::set<const Tile*> goal);
    /**
     * @brief Get a path between start, and any goal using the breadth first algorithm
     *
     * @param start start NodeGroup
     * @param goal set of excitable goals
     * @return std::vector<const NodeGroup *> path to closest goal
     */
    std::vector<const NodeGroup*> get_path_breadth_first(
        const NodeGroup* start, const std::set<const NodeGroup*> goal
    );
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
    template <class T>
    std::vector<const T*> get_path(
        const T* start, const std::set<const T*> goal,
        const std::set<const T*> search_through,
        std::function<bool(Node<const T>*, Node<const T>*)> compare
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
     * @return int height of heights solid z
     */
    int get_Z_solid(int x, int y);
    /**
     * @brief Get the hightest solid z below the given z
     *
     * @param x x position
     * @param y y position
     * @param z z height
     * @return int height of heights solid z
     */
    int get_Z_solid(int x, int y, int z);
    /**
     * @brief Get the heights z thats material is not in materials
     *
     * @param materials materials to exclude
     * @param x x position
     * @param y y position
     * @param guess expected height (for speed)
     * @return int height
     */
    int get_first_not(
        const std::set<std::pair<int, int>> materials, int x, int y, int guess
    ) const;

    inline int
    pos(int x, int y, int z) const { // for loops should go z than y than x
        return terrain_base_.pos(x,y,z);
    }

    inline int
    pos(const std::array<int, 3> sop) const {
        return terrain_base_.pos(sop);
    }

    inline int
    pos(const int sop[3]) const {
        return terrain_base_.pos(sop);
    }

    inline int
    pos(const Tile* const tile) const {
        return terrain_base_.pos(tile);
    }

    inline int
    pos(const Tile tile) const {
        return terrain_base_.pos(tile);
    }

    inline int
    pos_for_map(const Tile tile) const {
        return pos(tile);
    }

    inline int
    pos_for_map(const Tile* const tile) const {
        return pos(tile);
    }

    inline const std::array<int, 3>
    sop(int xyz) const {
        return terrain_base_.sop(xyz);
    }

    inline static std::array<int, 3>
    sop(int xyz, int xm, int ym, int zm) {
        return TerrainBase::sop(xyz, xm, ym, zm);
    }

    inline int
    get_X_MAX() const {
        return terrain_base_.get_X_MAX();
    };

    inline int
    get_Y_MAX() const {
        return terrain_base_.get_Y_MAX();
    };

    inline int
    get_Z_MAX() const {
        return terrain_base_.get_Z_MAX();
    };

    inline std::array<uint32_t, 3>
    get_size() const {
        return terrain_base_.get_size();
    }

    inline std::array<int32_t, 3>
    get_offset() const {
        return terrain_base_.get_offset();
    }

    inline bool
    in_range(int x, int y, int z) const {
        return terrain_base_.in_range(x,y,z);
    }

    inline Tile*
    get_tile(int x, int y, int z) {
        return terrain_base_.get_tile(x,y,z);
    };

    inline Tile*
    get_tile(int xyz) {
        return terrain_base_.get_tile(xyz);
    }

    inline const Tile*
    get_tile(int x, int y, int z) const {
        return terrain_base_.get_tile(x,y,z);
    };

    inline const Tile*
    get_tile(int xyz) const {
        return terrain_base_.get_tile(xyz);
    }

    inline uint32_t get_voxel(int x, int y, int z) const {
        return terrain_base_.get_voxel(x,y,z);
    }

    inline uint16_t
    get_voxel_color_id(int x, int y, int z) const {
        return terrain_base_.get_voxel_color_id(x,y,z);
    }

 private:
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
