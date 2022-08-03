#ifndef __TERRAIN_HPP__
#define __TERRAIN_HPP__
// TODO add comments
#include <array>
#include <iostream>
#include <set>
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <stdio.h>
#include <cstdint>
#include <map>

#include "node.hpp"
#include "unitpath.hpp"
#include "tile.hpp"
#include "node_group.hpp"
#include "chunk.hpp"
#include "TerrainGeneration/noise.hpp"
#include "TerrainGeneration/landgenerator.hpp"
#include "TerrainGeneration/material.hpp"
#include "TerrainGeneration/tilestamp.hpp"

class Chunk;

class Terrain {
private:
    std::vector<Tile> tiles;
    std::vector<Chunk> chunks;
    static int X_MAX; // should not be static
    static int Y_MAX;
    static int Z_MAX;
    static int Area_size;
    static int seed;

    std::map<int, NodeGroup*> tile_to_group;

    std::vector<uint8_t> grass_colors;
    int grass_grad_length;
    int grass_mid;

    template <typename T>
    void WRITE(T v, FILE * file) const{
        fwrite(&v, sizeof(T), 1, file);
    }
    template <typename T>
    void READ(T &v, FILE * file) const{
        fread(&v, sizeof(T), 1, file);
    }
    void export_color(const int sop[3], uint8_t color[4]) const;
    void get_mat_from_qb(const std::map<int, const Material> *materials, uint8_t v[4], Material * &mat_, uint8_t &color_id);
    uint32_t compress_color(uint8_t v[4]);

    void add_all_adjacent(int xyz);

    Node<Tile> new_node(Node<Tile> &parent, Tile &tile, Tile goal);
    void add_node(std::set<Node<Tile>> &node_list, Node<Tile> &node);
    template<class T>
    void get_path_through_nodes(Node<const T> *node, std::vector<const T *> &out, const T *start) const {
        out.push_back(node->get_tile());
        if (start == node->get_tile()) {
            return;
        }
        get_path_through_nodes(node->get_parent(), out, start);
    }

public:

    std::pair<Tile*, Tile*> get_start_end_test();

    const UnitPath get_path_type(int xs, int ys, int zs, int xf, int yf, int zf);

    static float get_H_cost(std::array<float, 3> xyz1, std::array<float, 3> xyz2);
    static float get_H_cost(std::array<int, 3> xyz1, std::array<int, 3> xyz2);

    template<class T>
    static float get_G_cost(const T tile, const Node<const T> node);

    int pos(int x, int y, int z) const {// for loops should go z than y than x
        return x * Y_MAX * Z_MAX + y * Z_MAX + z;
    }
    int pos(const std::array<int, 3> sop) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }
    int pos(const int sop[3]) const {
        return sop[0] * Y_MAX * Z_MAX + sop[1] * Z_MAX + sop[2];
    }
    int pos(const Tile *const tile) const {
        return pos(tile->get_x(), tile->get_y(), tile->get_z());
    }
    int pos(const Tile tile) const {
        return pos(tile.get_x(), tile.get_y(), tile.get_z());
    }
    const std::array<int, 3> sop(int xyz) {
        return {xyz / (Y_MAX * Z_MAX), (xyz / Z_MAX) % Y_MAX, xyz % (Z_MAX)};
    }
    static std::array<int, 3> sop(int xyz, int xm, int ym, int zm) {
        if (xyz >= xm*ym*zm){
            throw std::invalid_argument("index out of range");
        }
        return {xyz / (ym * zm), (xyz / zm) % ym, xyz % (zm)};
    }
    template<class T>
    static std::array<int, 3> sop(T t) {
        if (std::is_pointer<T>::value){
            return {t->get_x(), t->get_y(), t->get_z()};
        } else { return {t.get_x(), t.get_y(), t.get_z() };}
    }

    static std::vector<int> generate_macro_map(unsigned int size_x, unsigned int size_y, Json::Value map_data);
    void add_to_top(Json::Value to_data, const std::map<int, const Material> * material);
    static int get_stop_height(int height, const Json::Value how_to_add);
    void init_area(int area_x, int area_y, LandGenerator gen);

    void init_old(int x, int y, int z);
    void init(int x, int y, int Area_size_, int z, int seed, const std::map<int, const Material> *materials, Json::Value biome_data, std::vector<int> Terrain_Maps);
    void init(int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type, const std::map<int, const Material> * material, Json::Value biome_data);
    Terrain();
    Terrain(int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, const std::map<int, const Material> * material, Json::Value biome_data, std::vector<int> grass_grad_data, int grass_mid);
    Terrain(int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type, const std::map<int, const Material> * material, Json::Value biome_data);
    Terrain(const char * path, const std::map<int, const Material> * material);

    // TODO plack block
    template<class T>
    std::set<Node<const T> *> get_adjacent_nodes(const Node<const T> *const node, std::map<const T*, Node<const T>> &nodes, uint8_t type) const;

    std::vector<Chunk> get_chunks(){ return chunks; }

    NodeGroup* get_node_group(int xyz);
    NodeGroup* get_node_group(const Tile t);
    NodeGroup* get_node_group(const Tile* t);

    void add_node_group(NodeGroup* NG);
    void remove_node_group(NodeGroup* NG);

    inline int get_X_MAX() const { return X_MAX; };
    inline int get_Y_MAX() const { return Y_MAX; };
    inline int get_Z_MAX() const { return Z_MAX; };

    inline bool in_range(int x, int y, int z) const {
        return (x < X_MAX && x >= 0 && y < Y_MAX && y >= 0 && z < Z_MAX && z >= 0);
    }

    // return true when the point is within the bounds of terrain
    bool is_valid_pos(int x, int y, int z) const {
        return (x < X_MAX && x >= 0 && y < Y_MAX && y >= 0 && z < Z_MAX &&
                z >= 0);
    }

    Tile *get_tile(int x, int y, int z) {
        if (!in_range(x, y, z)) {
            std::cout << x << ";" << y << ";" << z << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[pos(x, y, z)];
        }
    };
    Tile *get_tile(int xyz) {
        if (xyz < 0 || xyz >= X_MAX * Y_MAX * Z_MAX) {
            std::cout << xyz << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[xyz];
        }
    }
    const Tile *get_tile(int x, int y, int z) const {
        if ((x >= X_MAX || x < 0 || y >= Y_MAX || y < 0 || z >= Z_MAX ||
             z < 0)) {
            std::cout << x << ";" << y << ";" << z << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[x * Y_MAX * Z_MAX + y * Z_MAX + z];
        }
    };
    const Tile *get_tile(int xyz) const {
        if (xyz < 0 || xyz >= X_MAX * Y_MAX * Z_MAX) {
            std::cout << xyz << "\n";
            throw std::invalid_argument("index out of range");
        } else {
            return &tiles[xyz];
        }
    }

    bool paint(Tile* tile, const Material * mat, uint8_t color_id);
    bool player_set_tile_material(int xyz, const Material *mat, uint8_t color_id);

    inline void set_tile_material(Tile * tile, const Material * mat, uint8_t color_id){
        tile->set_material(mat);
        tile->set_color_id(color_id);
    }

    void set_tile_region(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const Material * mat);

    void stamp_tile_region(TileStamp tStamp, int x ,int y);
    void stamp_tile_region(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const Material * mat, uint8_t color_id);
    //void stamp_tile_region(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const Material * mat, std::set<int> elements_can_stamp);
    void stamp_tile_region(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const Material * mat, std::set<std::pair<int, int>> elements_can_stamp, uint8_t color_id);

    void init_grass();
    void grow_all_grass_high();
    void grow_all_grass_low();
    //std::set<int[3]> Terrain::grow_grass_high(int pos[3], int level);
    //std::set<int[3]> Terrain::grow_grass_down(int pos[3], int level);

    inline bool can_stand_1(int x, int y, int z) const{
        return can_stand(x, y, z, 1, 1);
    }
    bool can_stand_1(int xyz) const; // this is fast, and used for looping
    // Ok so basically when running through a loop the cpu moves a large chunk of memory that is close together
    // into the cpu's memory, then this function iterates over memory space, rather then coordinate space.
    // someone should test if this is true.

    inline bool can_stand_1(const Tile tile) const{
        return can_stand(tile, 1, 1);
    }
    bool can_stand_1(const Tile *tile) const{
        return can_stand(tile, 1, 1);
    }
    bool can_stand(int x, int y, int z, int dz, int dxy) const;
    bool can_stand(const Tile tile, int dz, int dxy) const;
    bool can_stand(const Tile *tile, int dz, int dxy) const;

    int qb_save_debug(const char * path, const std::map<int, const Material>* materials);
    int qb_save(const char * path)const;
    int qb_read(const char * path, const std::map<uint32_t, std::pair<const Material*, uint8_t>> *materials);

    std::set<const NodeGroup*> get_all_node_groups() const ;

    std::vector<const Tile *> get_path_Astar(const Tile *start, const Tile *goal);
    std::vector<const NodeGroup *> get_path_Astar(const NodeGroup *start, const NodeGroup *goal);

    std::vector<const Tile *> get_path_breadth_first(const Tile *start, const std::set<const Tile *> goal);
    std::vector<const NodeGroup *> get_path_breadth_first(const NodeGroup *start, const std::set<const NodeGroup *> goal); //TODO this should return a pair

    template<class T>
    std::vector<const T *> get_path(const T * start,
                                        const std::set<const T*> goal,
                                        const std::set<const T*> search_through,
                                        std::function<bool(Node<const T>*, Node<const T>*)> compare) const;

    void init_chunks();
    void stitch_chunks_at(Tile* tile);

    int get_Z_solid(int x, int y);
    int get_Z_solid(int x, int y, int z);

    int get_first_not(const std::set< std::pair <int, int>> materials, int x, int y, int guess) const;
    //int get_first_not(const std::set< std::pair <int, int>> materials, int x, int y) const;


    void test();
};

#endif
