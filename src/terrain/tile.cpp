#include "tile.hpp"
#include "material.hpp"
#include "terrain.hpp"

#define NUM_GRASS 8 // TODO this should be removed
#define AIR_ID    0
#define DIRT_ID   1

namespace terrain {

Tile::Tile() {
    init({1, 1, 1}, 0);
}

Tile::Tile(std::array<int, 3> sop, const terrain::Material* material) {
    init(sop, material->element_id);
    set_material(material, 0);
}

Tile::Tile(
    std::array<int, 3> sop, const terrain::Material* material, uint8_t color_id
) {
    // bool solid = materials[mat_id].solid;
    init(sop, material->element_id);
    set_material(material, color_id);
}

void
Tile::init(std::array<int, 3> sop, uint8_t mat_id) {
    // auto sop = Terrain::sop(xyz);
    x = sop[0];
    y = sop[1];
    z = sop[2];
    solid_ = false;
    color_id_ = 0;
    mat_id_ = mat_id;
    grow_data_high_ = 0;
    grow_data_low_ = 0;
    grow_sink_ = false;
    grow_source_ = false;
    grass_ = false;
}

void
Tile::init(
    std::array<int, 3> sop, const terrain::Material* material, uint8_t color_id
) {
    init(sop, material->element_id);
    set_material(material, color_id);
}

// Set material, and color_id
void
Tile::set_material(const terrain::Material* const material, uint8_t color_id_) {
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
Tile::set_color_id(uint8_t color_id, const terrain::Material* const material) {
    if (color_id > material->color.size()) {
        return;
    }
    if ((mat_id_ != DIRT_ID) | (color_id < NUM_GRASS)) {
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

// Updates the grass color to account for edge gradient.
void
Tile::set_grass_color(
    int grass_grad_length, int grass_mid, std::vector<uint8_t> grass_colors
) {
    if (grass_) {
        if (grass_grad_length - grow_data_high_ - 1 < grow_data_low_
            || grow_data_low_ > grass_mid) {
            color_id_ = grass_colors[grow_data_low_];
        } else if (grow_data_high_ > grass_mid) {
            color_id_ = grass_colors[grass_grad_length - grow_data_high_ - 1];
        } else {
            color_id_ = grass_colors[grass_mid];
        }
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

// return `color_id`.
uint8_t
Tile::get_color_id() const {
    return color_id_;
}

// returns the element id and the color id as one int
uint16_t
Tile::get_mat_color_id() const {
    // element_id, and color_id are 8 bit this function
    // concatenates them together, and returns a 16 bit int
    if (mat_id_ == AIR_ID) {
        return 0;
    }
    return mat_id_ << 8 | color_id_;
}

// return `grow_data_low`
uint8_t
Tile::get_grow_low() const {
    return grow_data_low_;
}

// return `grow_data_high`
uint8_t
Tile::get_grow_high() const {
    return grow_data_high_;
}

// return x, y, z positions as array
std::array<int, 3>
Tile::sop() const {
    return {x, y, z};
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
