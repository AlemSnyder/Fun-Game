#include "tile.hpp"
#include "terrain.hpp"

#define NUM_GRASS 8
#define DIRT_ID 1

Tile::Tile() { init({1,1,1}, false); }
Tile::Tile(std::array<int, 3> sop, const Material *mat) { init(sop, mat);}
Tile::Tile(std::array<int, 3> sop, const Material * mat_, uint8_t color_id_){
    init( sop, mat_, color_id_);
}
void Tile::init(std::array<int, 3> sop, bool solid_) {
    //auto sop = Terrain::sop(xyz);
    x = sop[0];
    y = sop[1];
    z = sop[2];
    solid = solid_;
    color_id = 0;
    grow_data_high = 0;
    grow_data_low = 0;
    grow_sink = false;
    grow_source = false;
    grass = false;
}
void Tile::init(std::array<int, 3> sop, const Material * mat_) {
    init(sop, mat_->solid);
    mat=mat_;
}
void Tile::init(std::array<int, 3> sop, const Material * mat_, uint8_t color_id_) {
    init(sop, mat_);
    color_id = color_id_;
}
// Set material, and color_id
void Tile::set_material(const Material * mat_, uint8_t color_id_){
    set_material(mat_);
    set_color_id(color_id_);
}
// Set the `mat` to `mat_` and update `solid` and `color_id`.
void Tile::set_material(const Material * mat_){
    mat=mat_;
    if ( mat->element_id == 1 ){ // being set to dirt
        color_id = (z + (x / 16 + y / 16) % 2 ) / 3 % 2 + NUM_GRASS;
    } else {
        color_id = 0;
    }
    solid = mat->solid;
}
// If able, set `color_id` to `color_id_`.
void Tile::set_color_id(uint8_t color_id_){
    if ( mat->element_id != 1 ){
        color_id = color_id_;
    } // cannot set the color of dirt
}
// Set `grow_data_high` to `num`
void Tile::set_grow_data_high(int num){
    if (num < 0){
        grow_data_high = 0;
    } else{
        grow_data_high = num;
    }
}
// Set `grow_data_low` to `num`.
void Tile::set_grow_data_low(int num){
    if (num < 0){
        grow_data_low = 0;
    } else{
        grow_data_low = num;
    }
}
// Updates the grass color to account for edge gradient.
void Tile::set_grass_color(int grass_grad_length, int grass_mid, std::vector<uint8_t> grass_colors){
    if (grass){
        if (grass_grad_length - grow_data_high -1 < grow_data_low || grow_data_low > grass_mid){
            color_id = grass_colors[grow_data_low];
        } else if (grow_data_high > grass_mid){
            color_id = grass_colors[grass_grad_length - grow_data_high - 1];
        } else {
            color_id = grass_colors[grass_mid];
        }
    }
}
// Grow grass if `mat` is dirt.
void Tile::try_grow_grass(){
    if (mat->element_id == 1){
        grass = true;
        color_id = 0;
    }
}
// return the color of this tile.
uint32_t Tile::get_color()const{
    return (mat->color[color_id]).second;
}
// return `color_id`.
uint8_t Tile::get_color_id() const {
    return color_id;
}
// returns the element id and the color id as one int
uint16_t Tile::get_mat_color_id() const{
    // element_id, and color_id are 8 bit this function 
    // concatenates them together, and returns a 16 bit int
    if (mat->element_id == 0){
        return 0;
    }
    return mat->element_id << 8 | color_id; 
}
// return `grow_data_low`
uint8_t Tile::get_grow_low() const{
    return grow_data_low;
}
// return `grow_data_high`
uint8_t Tile::get_grow_high() const{
    return grow_data_high;
}
// return x, y, z positions as array
std::array<int, 3> Tile::sop() const {
    return { x, y, z };
}

void Tile::add_adjacent(Tile *tile, OnePath type) {
    adjacent.insert(std::make_pair(tile, type));
}
void Tile::add_adjacent(std::map<Tile *, OnePath>::iterator it, Tile *tile, OnePath type) {
    adjacent.insert(it, std::make_pair(tile, type));
}

void Tile::clear_adjacent(){
    adjacent.clear();
}

bool Tile::operator> (const Tile other) const{
    if (get_x() < other.get_x()){
        return true;
    } else if (get_x() > other.get_x()){
        return false;
    } else if (get_y() < other.get_y()){
        return true;
    } else if (get_y() < other.get_y()){
        return false;
    } else {return get_z() < other.get_z();}
}

bool TilePCompare::operator() (const Tile* lhs, const Tile* rhs) const{
    if (lhs->get_x() < rhs->get_x()){
        return true;
    } else if (lhs->get_x() > rhs->get_x()){
        return false;
    } else if (lhs->get_y() < rhs->get_y()){
        return true;
    } else if (lhs->get_y() < rhs->get_y()){
        return false;
    } else {return lhs->get_z() < rhs->get_z();}
}
