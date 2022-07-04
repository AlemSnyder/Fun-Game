#ifndef __TILE_HPP__
#define __TILE_HPP__

#include <functional>
#include <unordered_set>
#include <set>
#include <map>
#include <stdint.h>
#include "TerrainGeneration/material.hpp"
#include "onepath.hpp"
//#include <iostream>

class Tile;

struct TilePCompare{
    bool operator() (const Tile* lhs, const Tile* rhs) const;
};

class Tile {  // this represents a tile in the world
private:
    int x; // The x index
    int y; // The y index
    int z; // The z index
    // does this need to know where it is?
    uint8_t color_id; // The tile color is determined by this and the material type.
    uint8_t grow_data_high; // Determined by the horizontal manhattan distance from a wall
    uint8_t grow_data_low; // Determined by the horizontal manhattan distance from a edge
    bool grow_sink; // not used
    bool grow_source; // not used
    bool grass; // Does this tile obey grass color Gradient?
    
    bool solid;  // used for can stand, grass edges, etc. Should be the same as mat->solid.
    const Material * mat; // The material of the tile
    std::map<Tile *,OnePath, TilePCompare> adjacent;//(tile_pair_hash, tile_pair_equals);


public:

    Tile();
    Tile(std::array<int, 3> sop, const Material *mat);
    Tile(std::array<int, 3> sop, const Material *mat, uint8_t color_id);
    void init(std::array<int, 3> sop, bool solid);
    void init(std::array<int, 3> sop, const Material *mat);
    void init(std::array<int, 3> sop, const Material *mat, uint8_t color_id);
    // Setters
    void set_material(const Material * mat_); //Set mat to mat_, and updates color, and solid
    void set_material(const Material * mat_, uint8_t color_id_);
    void set_color_id(uint8_t color_id_); // Set color_id to color_id_
    void set_grow_data_low(int num); 
    void set_grow_data_high(int num); // Set grow_data_low to num
    void try_grow_grass();
    void set_grass_color(int grass_grad_length, int grass_mid, std::vector<uint8_t> grass_colors);
    // Getters
    int get_x() const { return x; }
    int get_y() const { return y; }
    int get_z() const { return z; }
    std::array<int, 3> sop() const;
        // Is this tile grass?
    bool is_grass() const {return grass;}
    const Material * get_material() const { return mat; }
    //int pos() const;

    uint32_t get_color() const ;
    uint8_t get_color_id() const ;
    uint8_t get_grow_low() const;
    uint8_t get_grow_high() const;

    void add_adjacent(Tile *tile, OnePath type);
    void add_adjacent(std::map<Tile *, OnePath>::iterator it, Tile *tile, OnePath type);
    void clear_adjacent();

    std::map<Tile *,OnePath, TilePCompare> & get_adjacent() { return adjacent; };
    const std::map<Tile *,OnePath, TilePCompare> & get_adjacent() const { return adjacent; };
    std::set<Tile *> get_adjacent_clear(int path_type);
    std::set<const Tile *> get_adjacent_clear(int path_type) const;

    inline bool is_solid() const { return solid; }
    bool operator==(const Tile other) const {
        return (this->x == other.x && this->y == other.y && this->z == other.z);
    }
    bool operator==(const Tile *other) const {
        return (this->x == other->x && this->y == other->y &&
                this->z == other->z);
    }
    bool operator>(const Tile other) const;
};

#endif  // __TILE_HPP__