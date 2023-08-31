#include "tile.hpp"

#include "material.hpp"
#include "terrain.hpp"

namespace terrain {

Tile::Tile(TerrainDim3 sop, const terrain::Material* material, ColorId color_id) :
    x(sop.x), y(sop.y), z(sop.z), mat_id_(0), color_id_(0), grow_data_high_(0),
    grow_data_low_(0), grow_sink_(false), grow_source_(false), grass_(false),
    solid_(false) {
    // set material should not fail so if material is bad for some reason
    // tile should still be fine.
    set_material(material, color_id);
}

// Set material, and color_id
void
Tile::set_material(const terrain::Material* const material, ColorId color_id_) {
    set_material(material);
    set_color_id(color_id_, material);
}

// Set the `mat_id_` to `material->element_id` and update `solid` and `color_id`.
void
Tile::set_material(const terrain::Material* const material) {
    mat_id_ = material->element_id;
    if (mat_id_ == DIRT_ID) { // being set to dirt
        color_id_ = (z + (x / 16 + y / 16) % 2) / 3 % 2 + NUM_GRASS;
    } else {
        color_id_ = 0;
    }
    solid_ = material->solid;
}

// If able, set `color_id` to `color_id_`.
void
Tile::set_color_id(ColorId color_id, const terrain::Material* const material) {
    if (color_id >= material->color.size()) {
        return;
    }
    if ((mat_id_ != DIRT_ID) || (color_id < NUM_GRASS && grass_)) {
        color_id_ = color_id;
    } // cannot set the color of dirt
}

// Set `grow_data_high` to `num`
void
Tile::set_grow_data_high(int num) {
    if (num < 0) {
        grow_data_high_ = 0;
    } else {
        grow_data_high_ = num;
    }
}

// Set `grow_data_low` to `num`.
void
Tile::set_grow_data_low(int num) {
    if (num < 0) {
        grow_data_low_ = 0;
    } else {
        grow_data_low_ = num;
    }
}

void
Tile::set_grass_color(
    unsigned int grass_grad_length, unsigned int grass_mid,
    std::vector<ColorId> grass_colors
) {
    if (!grass_)
        return;
    // This is how the gradient is determined.
    // clang-format off
/*
a = air, g = grass, s = solid

[a] [a] [g] [g]   [g]   [g]           [g]   [g]   [g] [g] [s] [s]       
[-] [-] [n] [n-1] [n-2] [...]       [...] [n-2] [n-1] [n] [-] [-]       
                                                                        
grow data high                        grow data low                     
- - \                                   / - -                           
     \                                 /                                
      \                               /                                 
grow data high is reversed

distance from air ->
|    //=======/
|   //       / 
|~~//~~~~~~~/~~
| //       /   
|//-------/    
<- distance to solid
heigh_influence /
low_influence  //
grass_mid      ~~

The if statements convert the data as follows, to get the grass color index

      /     /  <- grow_data_high_         / <-------- grow_data_high_   
     /     /                             /     /                        
    /     /                             /     /                         
-------------- <- grass_mid      or -------------- <- grass_mid         
  /     /                             /     /                           
 /     /                             /     /                            
/ < ------------- grow_data_low_    /     / <-------- grow_data_low_    

            /                                   /                       
           /                                   /                        
          /                                   /                         
   -------                       or          /                          
  /                                         /                           
 /                                         /                            
/                                         /                             

*/
    // clang-format on
    unsigned int heigh_influence = grass_grad_length - grow_data_high_ - 1;
    unsigned int low_influence = grow_data_low_;
    if (low_influence >= heigh_influence) {
        color_id_ = grass_colors[low_influence];
    } else if (low_influence >= grass_mid) {
        color_id_ = grass_colors[low_influence];
    } else if (heigh_influence <= grass_mid) {
        color_id_ = grass_colors[heigh_influence];
    } else {
        color_id_ = grass_colors[grass_mid];
    }
}

// Grow grass if `mat_id_` is dirt.
void
Tile::try_grow_grass() {
    if (mat_id_ == DIRT_ID) {
        grass_ = true;
        color_id_ = 0;
    }
}

// returns the element id and the color id as one number
MatColorId
Tile::get_mat_color_id() const {
    // element_id, and color_id are 8 bit this function
    // concatenates them together, and returns a 16 bit int
    if (mat_id_ == AIR_ID) {
        return 0;
    }
    return mat_id_ << 8 | color_id_;
}

bool
Tile::operator>(const Tile other) const {
    if (get_x() < other.get_x()) {
        return true;
    } else if (get_x() > other.get_x()) {
        return false;
    } else if (get_y() < other.get_y()) {
        return true;
    } else if (get_y() < other.get_y()) {
        return false;
    } else {
        return get_z() < other.get_z();
    }
}

bool
TilePCompare::operator()(const Tile* lhs, const Tile* rhs) const {
    if (lhs->get_x() < rhs->get_x()) {
        return true;
    } else if (lhs->get_x() > rhs->get_x()) {
        return false;
    } else if (lhs->get_y() < rhs->get_y()) {
        return true;
    } else if (lhs->get_y() > rhs->get_y()) {
        return false;
    } else {
        return lhs->get_z() < rhs->get_z();
    }
}

} // namespace terrain
