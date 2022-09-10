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

#include "chunk.hpp"
#include "material.hpp"
#include "path/node.hpp"
#include "path/node_group.hpp"
#include "terrain_generation/land_generator.hpp"
#include "terrain_generation/noise.hpp"
#include "terrain_generation/tilestamp.hpp"
#include "tile.hpp"
#include "path/tile_iterators.hpp"
#include "path/unit_path.hpp"

#include <stdio.h>

#include <array>
#include <cstdint>
//#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
//#include <unordered_set>
#include <vector>

namespace terrain {

// Forward declaration of Chunk
class Chunk;

// Forward declaration of AdjacentIterator
class AdjacentIterator;

/**
 * @brief The land in the world.
 *
 * @details Terrain holds all the tiles that exist. It also allows for
 * path-finding and its own generation.
 *
 */
class Terrain {
    friend class AdjacentIterator;
 private:
    // vector of voxels in terrain
    std::vector<Tile> tiles;
    // vector of chunks in terrain
    std::vector<Chunk> chunks;
    // length in the x direction
    int X_MAX;
    // length in the y direction
    int Y_MAX;
    // length in the z direction
    int Z_MAX;
    // size of terrain generation tile (see terrain generation)
    static int Area_size;
    // seed for randomness
    int seed;
    // map of tile position to the node group it is in
    std::map<int, NodeGroup*> tile_to_group;
    // vector that determines grass color from edge distance
    std::vector<uint8_t> grass_colors;
    // length of grass gradient
    int grass_grad_length;
    // gradient index of grass not by an edge
    int grass_mid;

    // save color at sop, to color
    void export_color(const int sop[3], uint8_t color[4]) const;
    // find color v in map, and save material to mat_, and color id to color_id
    void get_mat_from_qb(
        const std::map<int, const Material>* materials, uint8_t v[4], Material*& mat_,
        uint8_t& color_id
    );
    // convert 4 int 8 to 1 int 32 (reversed order)
    uint32_t compress_color(uint8_t v[4]);
    // create unit paths for this tile, and all it's adjacent tiles
    //void add_all_adjacent(int xyz);

    // get set of adjacent tiles with given path type
    //std::set<Tile*>
    //get_adjacent_clear(unsigned int xyz, int path_type);


    // trace nodes through parents to reach start
    template <class T>
    void get_path_through_nodes(Node<const T>* node,
        std::vector<const T*>& out,
        const T* start) const
    {
        out.push_back(node->get_tile());
        if (start == node->get_tile()) {
            return;
        }
        get_path_through_nodes(node->get_parent(), out, start);
    }

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

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param x x coordinate
     * @param y y coordinate
     * @param z z coordinate
     * @return int
     */
    int pos(int x, int y, int z) const { // for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }

    /**
     * @brief position in tiles vector of given tile position
     *
     * @param sop coordinate as an array
     * @return int
     */
    int pos(const std::array<int, 3> sop) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    int pos(const int sop[3]) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    int pos(const Tile* const tile) const { return pos(tile->sop()); }

    /**
     * @brief position in tiles vector of given tile
     *
     * @param tile tile to find position of
     * @return int
     */
    int pos(const Tile tile) const {
        return pos(tile.get_x(), tile.get_y(), tile.get_z());
    }

    /**
     * @brief position of chunk the node group is a part of
     *
     * @param node_group node group to find position of chunk
     * @return int
     */
    int pos(const NodeGroup* const node_group) const;

    /**
     * @brief return position in space of given vector index
     *
     * @param xyz vector index
     * @return const std::array<int, 3> position in space
     */
    const std::array<int, 3> sop(int xyz) const {
        return {xyz / (Y_MAX * Z_MAX), (xyz / Z_MAX) % Y_MAX, xyz % (Z_MAX)};
    }

    /**
     * @brief return position in space of given index
     *
     * @param xyz index
     * @param xm length in x direction
     * @param ym length in y direction
     * @param zm length in z direction
     * @return std::array<int, 3> position in 3D space
     */
    static std::array<int, 3> sop(int xyz, int xm, int ym, int zm) {
        if (xyz >= xm * ym * zm) {
            throw std::invalid_argument("index out of range");
        }
        return {xyz / (ym * zm), (xyz / zm) % ym, xyz % (zm)};
    }

    /**
     * @brief return position of given object
     *
     * @tparam T class that has position
     * @param t object that has position
     * @return std::array<int, 3> position of the object
     */
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
    void init_old(int x, int y, int z);
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
        std::vector<int> grass_grad_data, int grass_mid
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

    inline path::AdjacentIterator get_tile_adjacent_iterator(size_t pos, UnitPath path_type) const {
        return path::AdjacentIterator(*this, pos, path_type);
    }
    
    /**
     * @brief Get the nodes adjacent to this one
     *
     * @tparam T Type of underlying position
     * @param node node to find the adjacent of
     * @param nodes nodes that can be passed through
     * @param type path type allowed
     * @return std::set<Node<const T> *> adjacent nodes
     */
    //template <class T>
    std::set<Node<const NodeGroup>*> get_adjacent_nodes(
        const Node<const NodeGroup>* const node,
        std::map<size_t, Node<const NodeGroup>>& nodes,
        uint8_t type
    ) const;

    std::set<Node<const Tile>*> get_adjacent_nodes(
        const Node<const Tile>* const node,
        std::map<size_t, Node<const Tile>>& nodes,
        uint8_t type
    ) const;

    // std::vector<Chunk> get_chunks() { return chunks; }
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

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline int get_X_MAX() const { return X_MAX; };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline int get_Y_MAX() const { return Y_MAX; };

    /**
     * @brief Get length of terrain in x direction
     *
     * @return int length
     */
    inline int get_Z_MAX() const { return Z_MAX; };

    /**
     * @brief Get the size of terrain
     *
     * @return std::vector<unsigned int> vector of sizes
     */
    inline std::vector<unsigned int> get_size() const {
        return {(unsigned int)X_MAX, (unsigned int)Y_MAX, (unsigned int)Z_MAX};
    }

    /**
     * @brief Used for getting mesh
     *
     * @return std::vector<int> 0 3 times
     */
    inline std::vector<int> get_offset() const { return {0, 0, 0}; }

    /**
     * @brief test if tile position is within terrain bounds
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return true tile is in bounds
     * @return false tile is not in bounds
     */
    inline bool in_range(int x, int y, int z) const {
        return (x < X_MAX && x >= 0 && y < Y_MAX && y >= 0 && z < Z_MAX && z >= 0);
    }

    /**
     * @brief Get the tile object at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return Tile* tile at given position
     */
    Tile* get_tile(int x, int y, int z) {
        if (!in_range(x, y, z)) {
            std::cout << x << ";" << y << ";" << z << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[pos(x, y, z)];
        }
    };

    Tile* get_tile(int xyz) {
        if (xyz < 0 || xyz >= X_MAX * Y_MAX * Z_MAX) {
            std::cout << xyz << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[xyz];
        }
    }

    /**
     * @brief Get the tile at the given position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return Tile* tile at given position
     */
    const Tile* get_tile(int x, int y, int z) const {
        if ((x >= X_MAX || x < 0 || y >= Y_MAX || y < 0 || z >= Z_MAX || z < 0)) {
            std::cout << x << ";" << y << ";" << z << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[pos(x, y, z)];
        }
    };

    /**
     * @brief Get the tile at the given index
     *
     * @param xyz tile index
     * @return const Tile* tile at index
     */
    const Tile* get_tile(int xyz) const {
        if (xyz < 0 || xyz >= X_MAX * Y_MAX * Z_MAX) {
            std::cout << xyz << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[xyz];
        }
    }

    /**
     * @brief Get the color of a tile
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return uint32_t color or tile
     */
    uint32_t get_voxel(int x, int y, int z) const {
        if (in_range(x, y, z)) {
            return tiles[pos(x, y, z)].get_color();
        }
        return 0;
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
    inline void set_tile_material(Tile* tile, const Material* mat, uint8_t color_id) {
        tile->set_material(mat);
        tile->set_color_id(color_id);
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
    inline void stamp_tile_region(terrain_generation::TileStamp tStamp, int x, int y) {
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
     */
    void grow_all_grass_high();
    /**
     * @brief set the lower bound for grass color
     *
     */
    void grow_all_grass_low();

    /**
     * @brief test if 1 x 1 x 1 object can stand at the position
     *
     * @param x x position
     * @param y y position
     * @param z z position
     * @return true can stand
     * @return false cannot stand
     */
    inline bool can_stand_1(int x, int y, int z) const {
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
    inline bool can_stand_1(const Tile tile) const { return can_stand(tile, 1, 1); }

    /**
     * @brief test if 1 x 1 x 1 object can stand at given tile
     *
     * @param tile tile to test
     * @return true can stand
     * @return false cannot stand
     */
    bool can_stand_1(const Tile* tile) const { return can_stand(tile, 1, 1); }

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
     * @return int success status
     */
    int qb_save_debug(
        const std::string path, const std::map<int, const Material>* materials
    );
    /**
     * @brief save to path
     *
     * @param path path to save to
     * @return int success status
     */
    int qb_save(const std::string path) const;
    /**
     * @brief read from path
     *
     * @param path path to read from
     * @param materials materials in the terrain
     * @return int success status
     */
    int qb_read(
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
};

} // namespace terrain
