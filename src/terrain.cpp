#include <stdlib.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <ctime>
#include <deque>
//#include <list>
#include <map>
#include <queue>
#include <set>
//#include <unordered_set>
#include <vector>
#include <cstring>
#include <fstream>
#include <stdint.h>
#include <fstream>

#include "json/json.h"
#include "node.hpp"
#include "tile.hpp"
#include "terrain.hpp"
#include "chunk.hpp"

#include "TerrainGeneration/noise.hpp"
#include "TerrainGeneration/land_generator.hpp"
#include "TerrainGeneration/material.hpp"
#include "TerrainGeneration/tile_stamp.hpp"


#define DIRT_ID 1

int Terrain::X_MAX = 1;
int Terrain::Y_MAX = 1;
int Terrain::Z_MAX = 1;
int Terrain::Area_size = 32;
int Terrain::seed = 0;

Terrain::Terrain() { init_old(1, 1, 1); seed = 0;}

Terrain::Terrain(int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, const std::map<int, const Material> * material, Json::Value biome_data, std::vector<int> grass_grad_data, int grass_mid_){
    if (grass_mid_ < 0){
        grass_mid_ = 0;
    }
    for (size_t i = 0; i < grass_grad_data.size(); i++){
        if (i == (size_t)grass_mid_){
            grass_mid = grass_colors.size();
        }
        for (int j = 0; j < grass_grad_data[i]; j++){
            grass_colors.push_back(i);
        }
    }
    grass_grad_length = grass_colors.size();
    init(x_tiles, y_tiles, Area_size_, z_tiles, seed, material, biome_data, generate_macro_map(x_tiles, y_tiles, biome_data["Terrain_Data"]));
}

Terrain::Terrain(int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type, const std::map<int, const Material> * material, Json::Value biome_data){
    init(x_tiles, y_tiles, Area_size_, z_tiles, seed, tile_type, material, biome_data);
}

void Terrain::init(int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type, const std::map<int, const Material> * material, Json::Value biome_data){
    std::vector<int> Terrain_Maps;
    Terrain_Maps.resize(x_tiles*y_tiles, 0);
    Terrain_Maps[(x_tiles*y_tiles-1)/2]=tile_type;
    init(x_tiles, y_tiles, Area_size_, z_tiles, seed, material, biome_data, Terrain_Maps);
}

Terrain::Terrain(const char * path, const std::map<int, const Material> * material){
    std::map<uint32_t, std::pair<const Material*, uint8_t>> materials;
    for (auto it = material->begin(); it != material->end(); it++){
        for (size_t color_id =0; color_id < it->second.color.size(); color_id++){
            materials.insert( std::map<uint32_t, std::pair<const Material*, uint8_t>>::value_type(it->second.color.at(color_id).second, std::make_pair(&it->second, (uint8_t)color_id)) );
        }
    }
    qb_read(path, &materials);
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        if (can_stand_1(xyz)){
            add_all_adjacent(xyz);
        }
    }
    init_chunks();
}

void Terrain::init_old(int x, int y, int z) {
    X_MAX = x;
    Y_MAX = y;
    Z_MAX = z;
    tiles.resize(X_MAX * Y_MAX * Z_MAX);
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        this->get_tile(xyz)->init(sop(xyz), true);
    }
}
void Terrain::init(int x, int y, int Area_size_, int z, int seed_, const std::map<int, const Material> *materials, Json::Value biome_data, std::vector<int> Terrain_Maps){
    auto millisec_since_epoch =std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    Area_size = Area_size_;
    seed = seed_;
    X_MAX = x * Area_size;
    Y_MAX = y * Area_size;
    Z_MAX = z;

    tiles.resize(X_MAX * Y_MAX * Z_MAX);

    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - millisec_since_epoch << " time tiles resize\n";

    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        this->get_tile(xyz)->init(sop(xyz), &(*materials).at(0));
    }

    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - millisec_since_epoch << " time Terrain init\n";

    srand(seed);
    std::cout << "start of land Generator" << std::endl;

    // create a map of int -> Land_Generator
    std::map<int, Land_Generator> land_generators;

    // for tile macro in data biome
    for (unsigned int i = 0; i < biome_data["Tile_Macros"].size(); i++){
        // create a land generator for each tile macro
        Land_Generator gen(materials, biome_data["Tile_Macros"][i]["Land_Data"]);
        land_generators.insert(std::make_pair(i, gen));
    }

    for (int i = 0; i < x; i++)
    for (int j = 0; j < y; j++){
        for (unsigned int k = 0; k < biome_data["Tile_Data"][Terrain_Maps[j+i*y]]["Land_From"].size(); k ++){
            init_area(i, j, land_generators[biome_data["Tile_Data"][Terrain_Maps[j+i*y]]["Land_From"][k].asInt()]);
        }
    }

    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - millisec_since_epoch << " time init area" << std::endl;

    for (unsigned int i = 0; i < biome_data["After_Effects"]["Add_To_Top"].size(); i++){
        add_to_top(biome_data["After_Effects"]["Add_To_Top"][i], materials);
    }

    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - millisec_since_epoch << " time Add to Top" << std::endl;

    //grow_grass();
    init_grass();
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        if (can_stand_1(xyz)){
            add_all_adjacent(xyz);
        }
    }

    init_chunks();

    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - millisec_since_epoch << " Total time Terrain_init" << std::endl;
}

void Terrain::init_area(int area_x, int area_y, Land_Generator gen){
    int count = 0;
    while (!gen.empty()){
        stamp_tile_region(gen.get_this_stamp(), area_x, area_y);
        ++gen;
        count++;
    }
    gen.reset();
    //std::cout << count << " total stamps\n";
}

void Terrain::init_chunks(){
    for (int xyz=0; xyz<((X_MAX-1)/Chunk::size+1)*((Y_MAX-1)/Chunk::size+1)*((Z_MAX-1)/Chunk::size+1); xyz+=1){
        auto [x,y,z] = sop(xyz, ((X_MAX-1)/Chunk::size+1),((Y_MAX-1)/Chunk::size+1),((Z_MAX-1)/Chunk::size+1));
        chunks.push_back(Chunk(x,y,z, this));
    }
}

void Terrain::add_to_top(Json::Value top_data, const std::map<int, const Material> * materials){

    std::set< std::pair <int, int>> material_type;

    //std::cout << top_data << std::endl;

    for (Json::Value::ArrayIndex i = 0; i < top_data["above_colors"].size(); i++ ){
        int E = top_data["above_colors"][i]["E"].asInt();
        if (top_data["above_colors"][i]["C"].isInt()){
            int C = top_data["above_colors"][i]["C"].asInt();
            material_type.insert(std::make_pair(E, C));
        }
        else if (top_data["above_colors"][i]["C"].asBool()){
            for (unsigned int C = 0; C < (*materials).at(E).color.size(); C++){
                material_type.insert(std::make_pair(E, C));
            }
        }
    }

    //for (auto p : material_type){
    //    std::cout << p.first << " " << p.second << std::endl;
    //}

    int guess = 0;
    // for loop
    for (int x = 0; x < X_MAX; x++)
    for (int y = 0; y < Y_MAX; y++){
        // get first (not) z of material
        guess = get_first_not(material_type, x, y, guess);
        //std::cout << guess << std::endl;
        // if z is between some bounds
        // stop_h = get stop height (guess, top_data["how_to_add"])
        int max_height = get_stop_height(guess, top_data["how_to_add"]);
        for (int z = guess; z < max_height; z++){
            set_tile_material(get_tile(x,y,z), &(*materials).at(top_data["Material_id"].asInt()), top_data["Color_id"].asInt());
        }
    }
}

int Terrain::get_stop_height(int height, const Json::Value how_to_add){
    for (unsigned int i = 0; i < how_to_add.size(); i++){
        if (height >= how_to_add[i]["from"][0].asInt() && height < how_to_add[i]["from"][1].asInt()){
            //std::cout << "fails here" << std::endl;
            if (how_to_add[i]["to"].isInt()){
                return how_to_add[i]["to"].asInt();
            } else {
                return height + how_to_add[i]["add"].asInt();
            }
        }
    }
    return 0;
}

int Terrain::get_Z_solid(int x, int y, int z_start) {
    for (int z = z_start; z >= 0; z--) {
        if (this->get_tile(x, y, z)->is_solid()) {
            return z;
        }
    }
    return 0;
}
inline int Terrain::get_Z_solid(int x, int y) {
    return get_Z_solid(x, y, Z_MAX - 1);
}

int Terrain::get_first_not(const std::set< std::pair <int, int>> materials, int x, int y, int guess) const {
    if (guess < 1){
        guess = 1;
    }
    if (guess >= Z_MAX){
        guess = Z_MAX-1;
    }
    if (materials.find( std::make_pair(get_tile(x, y, guess - 1)->get_material()->element_id, get_tile(x,y,guess - 1)->get_color_id()) ) != materials.end()){
        if ( materials.find( std::make_pair(get_tile(x,y,guess)->get_material()->element_id, get_tile(x,y,guess)->get_color_id()) )== materials.end()){
            return guess;
        } else{
            // go up
            for (int z = guess + 1; z < Z_MAX; z++){
                if (materials.find( std::make_pair(get_tile(x,y,z)->get_material()->element_id, get_tile(x,y,z)->get_color_id()) )== materials.end()){
                    return z;
                }
            }
            return Z_MAX;// -1?
        }
    } else{
        // go down
        for (int z = guess - 2; z > 0; z-- ){
            if (materials.find( std::make_pair(get_tile(x,y,z)->get_material()->element_id, get_tile(x,y,z)->get_color_id()) ) != materials.end()){
                return z + 1;
            }
        }
        return 0;
    }
}

bool Terrain::can_stand(int x, int y, int z, int dz, int dxy) const {
    // x,y,z gives the position; dxy is the offset in x or y from the center,
    // and dz is the offset only upward
    if (!in_range(x + dxy - 1, y + dxy - 1, z - 1) ||
        !in_range(x - dxy + 1, y - dxy + 1, z + dz - 1)) {
        return false;
    }

    if (!get_tile(x, y, z-1)->is_solid()){
        return false;
    }
    // is there air in the volume
    for (int xp = -dxy + 1; xp < dxy; xp++) {
        for (int yp = -dxy + 1; yp < dxy; yp++) {
            for (int zp = 0; zp < dz; zp++) {
                if (get_tile(x + xp, y + yp, z + zp)->is_solid()) {
                    return false;
                };
            }
        }
    }
    // are there other tiles that need to be stood on solid?
    // might want to change this to all tiles
    // currently this only requires one solid tile.
    return true;
}
bool Terrain::can_stand(const Tile * tile, int dz, int dxy) const {
    return can_stand(tile->get_x(), tile->get_y(), tile->get_z(), dz, dxy);
}
bool Terrain::can_stand(const Tile tile, int dz, int dxy) const {
    return can_stand(tile.get_x(), tile.get_y(), tile.get_z(), dz, dxy);
}

bool Terrain::paint(Tile *tile, const Material *mat, uint8_t color_id){
    // sets color_id if the material is the same.
    if (tile->get_material()->element_id == mat->element_id){
        tile->set_color_id(color_id);
        return true;
    }
    return false;
}

bool Terrain::player_set_tile_material(int xyz, const Material *mat, uint8_t color_id){
    Tile * tile = get_tile(xyz);
    if (tile->get_material()->solid && mat->solid){
        // Can't change something from one material to another.
        return 0;
    }
    get_tile(xyz)->set_material(mat, color_id);
    return 1;
}

void Terrain::stamp_tile_region(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const Material * mat, std::set<std::pair<int, int>> elements_can_stamp, uint8_t color_id){
    // set tiles in region to mat and color_id if the current material is in elements_can_stamp.
    for (int x = x_start; x < x_end; x++){
        for (int y = y_start; y < y_end; y++){
            for (int z = z_start; z < z_end; z++){
                if (in_range(x,y,z)){
                    Tile * tile = get_tile(x,y,z);
                    if (elements_can_stamp.find( std::make_pair( (int)tile->get_material()->element_id, (int)tile->get_color_id() ) ) != elements_can_stamp.end()){
                        set_tile_material(tile, mat, color_id);
                        //std::cout << mat->element_id;
                    }
                }
            }
        }
    }
}
void Terrain::stamp_tile_region(int x_start, int y_start, int z_start, int x_end, int y_end, int z_end, const Material * mat,  uint8_t color_id){
    // set tiles in region to mat and color_id 
    for (int x = x_start; x < x_end; x++){
        for (int y = y_start; y < y_end; y++){
            for (int z = z_start; z < z_end; z++){
                if (in_range(x,y,z)){
                    set_tile_material(get_tile(x,y,z),mat, color_id);
                }
            }
        }
    }
}
inline void Terrain::stamp_tile_region(Tile_Stamp tStamp, int x, int y){ // unpack Tile_Stamp
    stamp_tile_region(tStamp.x_start + x*Area_size + Area_size/2, tStamp.y_start + y*Area_size + Area_size/2 , tStamp.z_start, tStamp.x_end + x*Area_size + Area_size/2, tStamp.y_end + y*Area_size + Area_size/2, tStamp.z_end, tStamp.mat, tStamp.elements_can_stamp, tStamp.color_id);
}

void Terrain::init_grass(){
    // Test all ties to see if they can be grass.
    for (int x_ = 0; x_ < X_MAX; x_++)
    for (int y_ = 0; y_ < Y_MAX; y_++)
    for (int z_ = 0; z_ < Z_MAX - 1; z_++){
        if (!get_tile(x_,y_,z_+1)->is_solid()){
            get_tile(x_,y_,z_)->try_grow_grass(); // add to sources and sinks
        }
    }
    int z_ = Z_MAX-1;
    for (int x_ = 0; x_ < X_MAX; x_++)
    for (int y_ = 0; y_ < Y_MAX; y_++){
        get_tile(x_,y_,z_)->try_grow_grass(); // add to sources and sinks
    }
    for (int i = 0; i<grass_grad_length; i++){
        grow_all_grass_high();
    }

    for (int i = 0; i<grass_grad_length; i++){
        grow_all_grass_low();
    }
    for (Tile& t : tiles){
        t.set_grass_color(grass_grad_length, grass_mid, grass_colors);
    }
}
// .1 sec
void Terrain::grow_all_grass_high(){
    // extends higher bound of grass color
    for (Tile& t : tiles){
        if (t.is_grass()){
            int level = 0;
            for (int x = -1; x < 2; x++)
            for (int y = -1; y < 2; y++){
                if (x == 0 && y == 0){
                    continue;
                }
                // safety function to test if you xyz is in range;
                if (in_range(t.get_x()+x, t.get_y()+y, t.get_z())){
                    const Tile* tile  = get_tile(t.get_x()+x, t.get_y()+y, t.get_z());
                    if (!tile->is_grass()){
                        level = grass_grad_length;
                        break;
                    }
                    if (tile->get_grow_high() > level){
                        level = tile->get_grow_high();
                    }
                }
            }
            t.set_grow_data_high(level-1);
        }
    }
}
void Terrain::grow_all_grass_low(){
    // extends lower bound of grass color
    for (Tile& t : tiles){ //for tile in sources
        if (t.is_grass()){
            int level = 0;
            for (int x = -1; x < 2; x++)
            for (int y = -1; y < 2; y++){
                // safety function to test if you xyz is in range;
                if (in_range(t.get_x()+x, t.get_y()+y, t.get_z())){
                    if (x == 0 && y == 0){
                        continue;
                    }
                    Tile* tile = get_tile(t.get_x()+x, t.get_y()+y, t.get_z());
                    
                    if (!tile->is_solid()){
                        level = grass_grad_length;
                        break;
                    }
                    if (tile->get_grow_low() > level){
                        level = tile->get_grow_low();
                    }
                }
            }
            t.set_grow_data_low(level-1);
        }
    }
}

// generates a size_x by size_y vector of macro tile types.
std::vector<int> Terrain::generate_macro_map(unsigned int size_x, unsigned int size_y, Json::Value terrain_data) {
    std::vector<int> out;
    int background = terrain_data["BackGround"].asInt(); // default terrain type.
    int numOctaves = terrain_data["NumOctaves"].asInt(); // number of octaves
    double persistance = terrain_data["Persistance"].asDouble();
    int range = terrain_data["Range"].asInt();
    int spacing = terrain_data["Spacing"].asInt();
    out.resize(size_x * size_y, background);
    NoiseGenerator ng = NoiseGenerator(numOctaves, persistance, 3);

    for (unsigned int i = 0; i < out.size(); i++){
        auto [x,y,z] = sop(i,size_x,size_y,1);
        auto p = ng.GetValueNoise((double)x*spacing,(double)y*spacing);
        out[i] = (int) (pow((p+1),2)*range);
    }
    for (unsigned int i = 0; i < size_x; i++){
        for (unsigned int j = 0; j < size_y; j++){
            std::cout << out[j + size_y*i] << " ";
        }
        std::cout << "\n";
    }
    return out;
}

// this should be the same as can_stand(x,y,z,1,1)
bool Terrain::can_stand_1(int x, int y, int z) const {
    if (x >= X_MAX || x < 0 || y >= Y_MAX || y < 0 || z >= Z_MAX || z < 1) {
        return false;
    }
    return (!get_tile(x, y, z)->is_solid() && get_tile(x, y, z - 1)->is_solid());
}
// this should be the same as can_stand(x,y,z,1,1)
bool Terrain::can_stand_1(int xyz) const {
    if (xyz % Z_MAX < 1 || xyz >= X_MAX*Y_MAX*Z_MAX) {
        return false;
    }
    return (!get_tile(xyz)->is_solid() && get_tile(xyz-1)->is_solid());
}
bool Terrain::can_stand_1(const Tile tile) const {
    if (tile.get_z() <= 0) {
        return false;
    }
    return ((!tile.is_solid()) &&
            this->get_tile(tile.get_x(), tile.get_y(), tile.get_z() - 1)
                ->is_solid());  // can_stand_1(tile.x, tile.y, tile.z);
}
inline bool Terrain::can_stand_1(const Tile *tile) const {
    return can_stand_1(tile->get_x(), tile->get_y(), tile->get_z());
}
// can_stand should take an int xyz
/*
template<class T>
float Terrain::get_H_cost(const T tile1, const T tile2) const {
    return get_H_cost(tile1.sop(), tile2.sop());
}
template<class T>
float Terrain::get_H_cost(const T *tile1, const T *tile2) const{
    return get_H_cost(tile1->sop(), tile2->sop());
}  // might need to define this for other orientations*/
float Terrain::get_H_cost(std::array<float, 3> xyz1, std::array<float, 3> xyz2) {
    double D1 = 1.0;
    double D2 = 1.414;
    double D3 = 1.0;
    auto [x1, y1, z1] = xyz1;
    auto [x2, y2, z2] = xyz2;

    float DX = abs(x1 - x2);
    float DY = abs(y1 - y2);
    float DZ = abs(z1 - z2);

    return (DZ * D3 + abs(DX - DY) * D1 + D2 * std::min(DX, DY));
}
float Terrain::get_H_cost(std::array<int, 3> xyz1, std::array<int, 3> xyz2) {
    double D1 = 1.0;
    double D2 = 1.414;
    double D3 = 1.0;
    auto [x1, y1, z1] = xyz1;
    auto [x2, y2, z2] = xyz2;

    float DX = abs(x1 - x2);
    float DY = abs(y1 - y2);
    float DZ = abs(z1 - z2);

    return (DZ * D3 + abs(DX - DY) * D1 + D2 * std::min(DX, DY));
}
template<class T>
inline float Terrain::get_G_cost(const T tile, const Node<const T> node){
    return node.get_gCost() + get_H_cost(tile.sop(), node.get_tile()->sop());
}/*
template<class T>
inline float Terrain::get_G_cost(const T *const tile, const Node<T> *const node) {
    return node->get_gCost() + get_H_cost(tile->sop(), node->get_tile()->sop());
}
template<class T>
inline float Terrain::get_G_cost(const T tile, const Node<T> *const node) {
    return node->get_gCost() + get_H_cost(tile.sop(), node->get_tile()->sop());
}
template<class T>
inline float Terrain::get_G_cost(const T *const tile, const Node<T> node) {
    return node.get_gCost() + get_H_cost(tile->sop(), node.get_tile()->sop());
}*/

void Terrain::add_all_adjacent(int xyz) {
    tiles[xyz].clear_adjacent();
    //std::map<Tile*,OnePath>::iterator it = tiles[xyz].get_adjacent().begin();

    for (int xyz_ = 0; xyz_ < 27; xyz_++) {
        if (xyz_ == 13) {
            continue;
        }  //
        auto [x_, y_, z_] = sop(xyz);
        auto [xs, ys, zs] = sop(xyz_, 3, 3, 3);
        // is valid position might take too long this can be optimized away
        if (is_valid_pos(x_ + xs - 1, y_ + ys - 1, z_ + zs - 1)) {
            // Tile t = ;
            Tile *other = get_tile(x_ + xs - 1, y_ + ys - 1, z_ + zs - 1);
            OnePath path_type = get_path_type(x_, y_, z_, x_ + xs - 1, y_ + ys - 1, z_ + zs - 1);
            //tiles[xyz].add_adjacent(it, other, path_type);
            tiles[xyz].add_adjacent(other, path_type);
            //it++;
        }
    }
    //std::cout << "adding adjacent" << std::endl;
}

std::list<int> Terrain::ExportVoxelsAsList() const{
    std::list<int> out;
    for (Tile t : tiles){
        int i=0;
        if (t.is_solid()){
            i=1;
        }
        out.push_back(i);
    }
    return out;
}

void Terrain::Get_Mesh_Greedy(std::vector<MoreVectors::vector3> &vertices, std::vector<MoreVectors::vector4> &faces) {
    int dims[3] = {X_MAX, Y_MAX, Z_MAX};
    for (std::size_t axis = 0; axis < 3; ++axis) {
        // printf("axis: %ld\n", axis);

        const std::size_t u = (axis + 1) % 3;
        const std::size_t v = (axis + 2) % 3;

        // printf("u: %ld, v: %ld\n", u, v);

        int x[3] = {0};
        int q[3] = {0};
        std::vector<int> mask(dims[u] * dims[v]);
        // printf("dims[u]: %d, dims[v]: %d\n", dims[u], dims[v]);

        printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
        // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

        // Compute mask
        q[axis] = 1;
        for (x[axis] = -1; x[axis] < dims[axis];) {
            // printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
            // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

            std::size_t counter = 0;
            for (x[v] = 0; x[v] < dims[v]; ++x[v])
                for (x[u] = 0; x[u] < dims[u]; ++x[u], ++counter) {
                    const int a = 0 <= x[axis] ? get_tile(x[0], x[1], x[2])->get_material()->element_id : 0;
                    const int b = x[axis] < dims[axis] - 1
                                      ? get_tile(x[0] + q[0], x[1] + q[1], x[2] + q[2])->get_material()->element_id
                                      : 0;
                    const bool ba = static_cast<bool>(a);
                    if (ba == static_cast<bool>(b))
                        mask[counter] = 0;
                    else if (ba)
                        mask[counter] = a;
                    else
                        mask[counter] = -b;
                }

            ++x[axis];

            // for (size_t a = 0; a < dims[u] * dims[v]; ++a)
            //   printf("%d, ", mask[a]);
            // printf("\n");

            // Generate mesh for mask using lexicographic ordering
            int width = 0, height = 0;

            counter = 0;
            for (int j = 0; j < dims[v]; ++j)
                for (int i = 0; i < dims[u];) {
                    int c = mask[counter];
                    if (c) {
                        // Compute width
                        for (width = 1;
                             c == mask[counter + width] && i + width < dims[u];
                             ++width) {
                        }

                        // Compute height
                        bool done = false;
                        for (height = 1; j + height < dims[v]; ++height) {
                            for (int k = 0; k < width; ++k)
                                if (c != mask[counter + k + height * dims[u]]) {
                                    done = true;
                                    break;
                                }

                            if (done) break;
                        }

                        // Add quad
                        x[u] = i;
                        x[v] = j;

                        int du[3] = {0}, dv[3] = {0};

                        if (c > 0) {
                            dv[v] = height;
                            du[u] = width;
                        } else {
                            c = -c;
                            du[v] = height;
                            dv[u] = width;
                        }

                        const std::size_t vertexSize = vertices.size();
                        vertices.push_back(MoreVectors::vector3(x[0], x[1], x[2]));
                        vertices.push_back(
                            MoreVectors::vector3(x[0] + du[0], x[1] + du[1], x[2] + du[2]));
                        vertices.push_back(MoreVectors::vector3(x[0] + du[0] + dv[0],
                                                   x[1] + du[1] + dv[1],
                                                   x[2] + du[2] + dv[2]));
                        vertices.push_back(
                            MoreVectors::vector3(x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]));

                        faces.push_back(MoreVectors::vector4(vertexSize, vertexSize + 1,
                                                vertexSize + 2, c));
                        faces.push_back(MoreVectors::vector4(vertexSize + 2, vertexSize + 3,
                                                vertexSize, c));

                        for (int b = 0; b < width; ++b)
                            for (int a = 0; a < height; ++a)
                                mask[counter + b + a * dims[u]] = 0;

                        // Increment counters
                        i += width;
                        counter += width;
                    } else {
                        ++i;
                        ++counter;
                    }
                }
        }

        // printf("x: %d, %d, %d\n", x[0], x[1], x[2]);
        // printf("q: %d, %d, %d\n", q[0], q[1], q[2]);

        // printf("\n");
    }
}
//! should be removed
std::set<Tile *> Terrain::get_adjacent_Tiles(const Tile *const tile, uint8_t type) {
    std::set<Tile *> out;

    for (std::pair<Tile *, OnePath> t : tile->get_adjacent()) {
        if (t.second.compatible(type)) {
            out.insert(t.first);
        }
    }
    return out;
};
//! should be removed
std::set<const Tile *> Terrain::get_adjacent_Tiles(const Tile *const tile, uint8_t type) const {
    std::set<const Tile *> out;

    for (const std::pair<Tile *,OnePath> t : tile->get_adjacent()) {
        if (t.second.compatible(type)) {
            out.insert(t.first);
        }
    }
    return out;
};



template<class T>
std::set<Node<const T> *> Terrain::get_adjacent_Nodes(const Node<const T> *const node, std::map<const T*, Node<const T>> & nodes, uint8_t path_type) const {
    std::set<Node<const T> *> out;
    for (const T* t : node->get_adjacent(path_type)) {
        try{
            Node<const T>* tile = &nodes.at(t);
            out.emplace(tile);
        }
        catch(const std::exception& e){ }
    }
    return out;
};

NodeGroup* Terrain::get_NodeGroup(int xyz){
    try{
        return tile_to_group.at(xyz);
    }
    catch(const std::exception& e){
        return nullptr;
    }
}

NodeGroup* Terrain::get_NodeGroup(const Tile t){
    return get_NodeGroup(pos(t));
}
NodeGroup* Terrain::get_NodeGroup(const Tile * t){
    return get_NodeGroup(pos(t));
}

void Terrain::add_NodeGroup(NodeGroup* NG){
    for (const Tile* t : NG->get_tiles()){
        tile_to_group[pos(t)] = NG;
    }
}

void Terrain::remove_NodeGroup(NodeGroup* NG){
    for (const Tile* t : NG->get_tiles()){
        tile_to_group.erase(pos(t));
    }
}

void Terrain::test() {
    Tile *goal = this->get_tile(2, 2, 2);
    auto millisec_since_epoch =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    std::vector<Node<Tile> *> nodes;
    // std::cout <<
    // std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
    // - millisec_since_epoch << " time\n";
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        nodes.push_back(new Node(this->get_tile(xyz), get_H_cost(sop(xyz), goal->sop())));
    }
    std::cout << sizeof(nodes) << " Size of nodes\n";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                         .count() -
                     millisec_since_epoch
              << " time define all nodes\n";
    int x = 0;
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        auto node = new Node(this->get_tile(xyz), get_H_cost(sop(xyz), goal->sop()));
        if (node->is_explored()) {
            x++;
        };
        //! delete node?
    }
    std::cout << x << " x\n";
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
                     std::chrono::system_clock::now().time_since_epoch())
                         .count() -
                     millisec_since_epoch
              << " time\n";
}

const OnePath Terrain::get_path_type(int xs, int ys, int zs, int xf, int yf, int zf) {
    // the function should be passed the shape of the thing that wants to go on
    // the path just set them for now
    int dz = 3;
    int dxy = 1;

    int8_t type = abs(xs - xf) + abs(ys - yf) + 4 * abs(zs - zf);
    bool open;
    if (type == 1 || type == 4) {
        // up / down or side to side
        // in this case the two tiles are bordering
        // same lever so the only thing that maters if the entity can stand on
        // both tiles
        open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz, dxy);
    } else if (type == 2) {
        // still the same level
        // this test if the start and final locations are open,
        // and if the two between them are open
        // S O
        // O F
        open = can_stand(xs, ys, zs, dz, dxy) &&
               can_stand(xf, yf, zf, dz, dxy) &&
               can_stand(xs, yf, zs, dz, dxy) &&
               can_stand(xf, ys, zf, dz, dxy);
    } else if (type == 5) {
        if (zf > zs) {
            // going up, and over
            open = can_stand(xs, ys, zs, dz + 1, dxy) &&
                   can_stand(xf, yf, zf, dz, dxy);
        } else {
            // going down and over
            open = can_stand(xs, ys, zs, dz, dxy) &&
                   can_stand(xf, yf, zf, dz + 1, dxy);
        }

    } else if (type == 6) {
        if (zf > zs) {
            // going up, and over
            open = can_stand(xs, ys, zs, dz + 1, dxy) &&
                   can_stand(xf, yf, zf, dz, dxy) &&
                 ((can_stand(xf, ys, zs, dz + 1, dxy) ||
                   can_stand(xf, ys, zf, dz, dxy)) &&
                  (can_stand(xs, yf, zs, dz + 1, dxy) ||
                   can_stand(xs, yf, zf, dz, dxy)));
        } else {
            // going down and over
            open = can_stand(xs, ys, zs, dz, dxy) &&
                   can_stand(xf, yf, zf, dz + 1, dxy)&&
                 ((can_stand(xf, ys, zs, dz, dxy) ||
                   can_stand(xf, ys, zf, dz + 1, dxy)) &&
                  (can_stand(xs, yf, zs, dz, dxy) ||
                   can_stand(xs, yf, zf, dz + 1, dxy)));
        }
    }

    return int8_t(type + 8 * open);
}

std::set<const NodeGroup*> Terrain::get_all_node_groups() const {
    std::set<const NodeGroup*> out;
    for (size_t c = 0; c < chunks.size(); c++){
        chunks[c].add_nodes_to(out);
    }
    return out;
}

std::vector<const Tile *> Terrain::get_path_Astar(const Tile *start_, const Tile *goal_) {

    NodeGroup * GoalNode;
    NodeGroup * StartNode;
    int goal_z = goal_->get_z();
    Tile* goal;
    if (!can_stand_1(goal_)) {
        goal_z = get_Z_solid(goal_->get_x(), goal_->get_y()) + 1;
        if (goal_z !=0){
            goal = get_tile(goal_->get_x(), goal_->get_y(), goal_z);
        } else {
            return std::vector<const Tile *>();
        }
    } else{
        return std::vector<const Tile *>();
    }
    int start_z = start_->get_z();
    Tile* start;
    if (!can_stand_1(start_)) {
        start_z = get_Z_solid(start_->get_x(), start_->get_y()) + 1;
        if (start_z !=0){
            start = get_tile(start_->get_x(), start_->get_y(), start_z);
        } else {
            return std::vector<const Tile *>();
        }
    } else{
        return std::vector<const Tile *>();
    }
    if ((GoalNode = get_NodeGroup(goal))) { }
    else { return std::vector<const Tile *>(); }
    if ((StartNode = get_NodeGroup(start))) { }
    else { return std::vector<const Tile *>(); }

    std::vector<const NodeGroup *> Node_path = get_path_Astar(StartNode, GoalNode);
    // if Node_path is empty then return
    if (Node_path.size() == 0){
        return std::vector<const Tile *>();
    }
    std::set<const Tile*> search_through;
    for (const NodeGroup* NG : Node_path){
        auto tiles = NG->get_tiles();
        search_through.insert(tiles.begin(), tiles.end());
    }

    std::function<bool (Node<const Tile> *, Node<const Tile> *)> compare = [](Node<const Tile> *lhs, Node<const Tile> *rhs)->bool {
        return lhs->get_fCost() > rhs->get_fCost();
    };

    return get_path(start, {goal}, search_through, compare);

}

std::vector<const NodeGroup *> Terrain::get_path_Astar(const NodeGroup *start, const NodeGroup *goal) {
    
    std::function<bool (Node<const NodeGroup> *, Node<const NodeGroup> *)> compare = [](Node<const NodeGroup> *lhs, Node<const NodeGroup> *rhs)->bool {
        return lhs->get_fCost() > rhs->get_fCost();
    };

    std::set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path(start, {goal}, search_through, compare);
}

std::vector<const NodeGroup *> Terrain::get_path_BreadthFirst(const NodeGroup *start, std::set<const NodeGroup *> goal) {

    std::function<bool (Node<const NodeGroup> *, Node<const NodeGroup> *)> compare = [](Node<const NodeGroup> *lhs, Node<const NodeGroup> *rhs)->bool {
        return lhs->get_gCost() > rhs->get_gCost();
    };

    std::set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path(start, goal, search_through, compare);
}

std::vector<const Tile *> Terrain::get_path_BreadthFirst(const Tile *start, const std::set<const Tile *> goal_) {

    std::set<const NodeGroup *> GoalNodes;
    bool NoGoal = true;
    for (const Tile* g : goal_){
        int goal_z = g->get_z();
        if (!can_stand_1(g)) {
            goal_z = get_Z_solid(g->get_x(), g->get_y()) + 1;
            if (goal_z !=0){
                NoGoal = false;
                GoalNodes.insert(get_NodeGroup(get_tile(g->get_x(), g->get_y(), goal_z)));
            }
        } else{
            NoGoal = false;
            GoalNodes.insert(get_NodeGroup(g));
        }
    }
    if (NoGoal) {  // in this case there is no valid z position at one of the given x, y positions
        return std::vector<const Tile *>();
    }

    std::vector<const NodeGroup *> Node_path = get_path_BreadthFirst(get_NodeGroup(start), GoalNodes);

    const Tile * goal = nullptr;
    const NodeGroup* end = Node_path.back();
    for (const Tile *g : goal_){
        if (end->get_tiles().find(g) != end->get_tiles().end()){
            goal = g;
            break;
        }
    }
    std::set<const Tile*> search_through;
    for (const NodeGroup* NG : Node_path){
        search_through.insert(NG->get_tiles().begin(), NG->get_tiles().end());
    }

    std::function<bool (Node<const Tile> *, Node<const Tile> *)> compare = [](Node<const Tile> *lhs, Node<const Tile> *rhs)->bool {
        return lhs->get_fCost() > rhs->get_fCost();
    };

    return get_path(start, {goal}, search_through, compare);

}

template<class T>
std::vector<const T *> Terrain::get_path(const T *start,
                                         const std::set<const T*> goal,
                                         const std::set<const T*> search_through,
                                         std::function<bool(Node<const T>*, Node<const T>*)> compare) const {

    auto T_compare = [&compare](Node<const T> *lhs, Node<const T> *rhs) {
        return compare(lhs, rhs);
    };

    std::priority_queue<Node<const T> *, std::vector<Node<const T> *>, decltype(T_compare)> openNodes(T_compare);

    std::set<Node<const T>*> searched;

    std::map<const T*, Node<const T>> nodes; // The nodes that can be walked through
    for (const T* t : search_through){
        nodes[t] = Node<const T>(t, get_H_cost(t->sop(), (*goal.begin())->sop()));
    }
    Node<const T> start_node = nodes[start];
    openNodes.push(&start_node);  // gotta start somewhere
    start_node.explore();
    //searched.insert(&start_node);
    
    while (!openNodes.empty()) {
        Node<const T> *choice = openNodes.top();
        openNodes.pop();  // Remove the chosen node from openNodes
        // Expand openNodes around the best choice
        std::set<Node<const T>*> adjacent_nodes = get_adjacent_Nodes(choice, nodes, 63); //! onepath needs to be updated. this should be returned to 31
        for (Node<const T> *n : adjacent_nodes) {
            // if can stand on the tile    and the tile is not explored
            // get_adjacent should only give open nodes
            if (!n->is_explored()) {
                n->explore(choice, get_G_cost(*(n->get_tile()), *choice));
                if (goal.find(n->get_tile()) != goal.end()) {
                    std::vector<const T *> path;
                    get_path_through_nodes(n, path, start);
                    return path;
                }
                openNodes.push(n);  // n can be chose to expand around
                //searched.insert(n);
            }
            else {
                //openNodes.remove n
                n->explore(choice, get_G_cost(*(n->get_tile()), *choice));
                //openNodes.add n
            }
        }
    }
    std::vector<const T*> out;
    return out;
}

// Set `color` to the color of the tile at `pos`.
void Terrain::export_color(const int sop[3], uint8_t color[4]) const{
    int position = pos(sop);
    uint32_t tile_color = get_tile(position)->get_color();
    color[0]=(tile_color >> 24) & 0xFF;
    color[1]=(tile_color >> 16) & 0xFF;
    color[2]=(tile_color >> 8) & 0xFF;
    color[3]= tile_color & 0xFF;
}
uint32_t Terrain::compress_color(uint8_t v[4]){
    return (uint32_t)v[3] | (uint32_t)v[2] << 8 | (uint32_t)v[1] << 16 | (uint32_t)v[0] << 24;
}

int Terrain::qb_save_debug(const char * path, const std::map<int, const Material>* materials) {
    int x=0;
    for (Chunk & c : get_chunks()){
        for ( NodeGroup& NG : c.get_NodeGroups() ){
            for (const Tile* t : NG.get_tiles()){
                set_tile_material(get_tile(pos(t->sop())), &materials->at(7), x%4);
            }
            x++;
        }
    }
    return qb_save(path);
}

// Save all tiles as .qb to path.
int Terrain::qb_save(const char * path)const{
    //Saves the tiles in this to the path specified

    //This is from goxel with GPL license
    std::cout << "Saving to " << path << "\n";
    std::cout << "    max X: " << X_MAX << std::endl;
    std::cout << "    max Y: " << Y_MAX << std::endl;
    std::cout << "    max Z: " << Z_MAX << std::endl;
    FILE *file;
    int count, x, y, z, sop[3];
    uint8_t v[4];

    count = 1; //the number of layers

    file = fopen(path, "wb");
    WRITE<uint32_t>(257, file); // version
    WRITE<uint32_t>(0, file);   // color format RGBA
    WRITE<uint32_t>(1, file);   // orientation right handed // c
    WRITE<uint32_t>(0, file);   // no compression
    WRITE<uint32_t>(0, file);   // vmask
    WRITE<uint32_t>(count, file);

    const char *name = "Main World";
    WRITE<int8_t>(std::strlen(name), file);
    fwrite(name, strlen(name), 1, file);
    WRITE<uint32_t>(X_MAX, file);  // x
    WRITE<uint32_t>(Z_MAX, file);  // z
    WRITE<uint32_t>(Y_MAX, file);  // y
    WRITE<int32_t>(-X_MAX/2, file); // x
    WRITE<int32_t>(0, file);       // z
    WRITE<int32_t>(-Y_MAX/2, file); // y
    //iter = mesh_get_accessor(mesh);
    int tiles_written = 0;
    for (x = 0; x < X_MAX; x++)
    for (z = 0; z < Z_MAX; z++)
    for (y = 0; y < Y_MAX; y++) {
        sop[0] = x;
        sop[1] = y;
        sop[2] = z;
        export_color(sop, v);
        if (v[3]!=0x0){
            v[3]=0xFF;
        }
        fwrite(v, 4, 1, file);
        tiles_written++;
    }
    fclose(file);
    std::cout << "    tiles written: " << tiles_written << std::endl;
    return 0;
}

int Terrain::qb_read(const char * path, const std::map<uint32_t, std::pair<const Material*, uint8_t>> *materials){
    //Read the tiles from the path specified, and save

    //This is partially from goxel with GPL license
    std::cout << "Reading form " << path << "\n";
    FILE *file;
    int x, y, z; // position in terrain
    uint32_t void_; // void int 32 used to read 32 bites without saving it.
    int32_t x_offset, y_offset, z_offset;
    uint8_t v[4];

    file = fopen(path, "rb");
    READ<uint32_t>(void_, file); // version
    //std::cout << void_ << std::endl;
    (void) void_;
    READ<uint32_t>(void_, file);   // color format RGBA
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);   // orientation right handed // c
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);   // no compression
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);   // vmask
    //std::cout << void_ << std::endl;(void) void_;
    READ<uint32_t>(void_, file);
    //std::cout << void_ << std::endl;(void) void_;
    // none of these are used

    int8_t name_len;
    READ<int8_t>(name_len, file);
    std::cout << "name length: " << (int) name_len << std::endl;
    char* name = (char*) malloc (name_len);
    fread(name, sizeof(char), name_len, file);
    std::cout << "Name: " << name << std::endl;
    uint32_t X_max, Y_max, Z_max;
    READ<uint32_t>(X_max, file);  // x
    READ<uint32_t>(Z_max, file);  // z
    READ<uint32_t>(Y_max, file);  // y
    X_MAX = X_max; Y_MAX=Y_max;Z_MAX=Z_max;
    READ<int32_t>(x_offset, file); // x
    READ<int32_t>(z_offset, file); // z
    READ<int32_t>(y_offset, file); // y
    std::cout << "    max X: " << X_MAX << std::endl;
    std::cout << "    max Y: " << Y_MAX << std::endl;
    std::cout << "    max Z: " << Z_MAX << std::endl;
    tiles.resize(X_MAX * Y_MAX * Z_MAX);

    std::set<uint32_t> unknown_materials;

    int tiles_read = 0;
    for (x = 0; x < X_MAX; x++)
    for (z = 0; z < Z_MAX; z++)
    for (y = 0; y < Y_MAX; y++) {
        fread(v, sizeof(uint8_t), 4, file);// read the color to v
        if (v[3] == 0){ // if the qb voxel is transparent.
            auto mat_color = materials->at(0); // set the materials to air
            get_tile(x,y,z)->init({x,y,z}, mat_color.first, mat_color.second);
        } else {
            auto CC = compress_color(v);// get the complete color
            if (materials->count(CC)){// if the color is known
                auto mat_color = materials->at(CC);
                get_tile(x,y,z)->init({x,y,z}, mat_color.first, mat_color.second);
                tiles_read++;
            } else {
                unknown_materials.insert(CC);
                auto mat_color = materials->at(0); // else set to air.
                get_tile(x,y,z)->init({x,y,z}, mat_color.first, mat_color.second);
            }
        }
    }
    for (uint32_t CC : unknown_materials){
        std::cout << "    cannot find color: " << CC << std::endl;
    }
    fclose(file);
    std::cout << "    tiles read: " << tiles_read << std::endl;
    return 0;
}

std::pair<Tile*, Tile*> Terrain::get_start_end_test(){
    std::pair<Tile*, Tile*>out;
    bool first = true;
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        if (get_tile(xyz)->get_material()->element_id == 7 && get_tile(xyz)->get_color_id() == 4){
            if (first){
                out.first = get_tile(xyz);
                first = false;
            } else {
                out.second = get_tile(xyz);
                return out;
            }
        }
    }
    return out;
}

