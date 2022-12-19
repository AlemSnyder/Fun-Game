#include "terrain.hpp"

#include "../logging.hpp"
#include "../util/time.hpp"
#include "../util/voxel_io.hpp"
#include "chunk.hpp"
#include "material.hpp"
#include "path/node.hpp"
#include "terrain_generation/land_generator.hpp"
#include "terrain_generation/noise.hpp"
#include "terrain_generation/tilestamp.hpp"
#include "terrain_helper.hpp"
#include "tile.hpp"

#include <json/json.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace terrain {

int Terrain::Area_size = 32;

Terrain::Terrain() : seed(0) {
    init(0, 0, 0);
}

Terrain::Terrain(
    int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed,
    const std::map<int, const Material>* material, Json::Value biome_data,
    std::vector<int> grass_grad_data, unsigned int grass_mid
) {
    if (grass_mid >= grass_grad_data.size()) {
        grass_mid_ = grass_grad_data.size() - 1;
        std::cerr << "Grass Mid (from biome_data.json) not valid";
    }

    for (size_t i = 0; i < grass_grad_data.size(); i++) {
        if (i == static_cast<size_t>(grass_mid)) {
            grass_mid_ = grass_colors_.size();
        }
        for (int j = 0; j < grass_grad_data[i]; j++) {
            grass_colors_.push_back(i);
        }
    }
    grass_grad_length_ = grass_colors_.size();
    init(
        x_tiles, y_tiles, Area_size_, z_tiles, seed, material, biome_data,
        generate_macro_map(x_tiles, y_tiles, biome_data["Terrain_Data"])
    );
}

Terrain::Terrain(
    int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type,
    const std::map<int, const Material>* material, Json::Value biome_data
) {
    init(x_tiles, y_tiles, Area_size_, z_tiles, seed, tile_type, material, biome_data);
}

void
Terrain::init(
    int x_tiles, int y_tiles, int Area_size_, int z_tiles, int seed, int tile_type,
    const std::map<int, const Material>* material, Json::Value biome_data
) {
    std::vector<int> Terrain_Maps;
    Terrain_Maps.resize(x_tiles * y_tiles, 0);
    Terrain_Maps[(x_tiles * y_tiles - 1) / 2] = tile_type;
    init(
        x_tiles, y_tiles, Area_size_, z_tiles, seed, material, biome_data, Terrain_Maps
    );
}

Terrain::Terrain(
    const std::string path, const std::map<int, const Material>* materials
) :
    materials_(materials) {
    std::map<uint32_t, std::pair<const Material*, uint8_t>> materials_inverse;
    for (auto it = materials_->begin(); it != materials_->end(); it++) {
        for (size_t color_id = 0; color_id < it->second.color.size(); color_id++) {
            materials_inverse.insert(
                std::map<uint32_t, std::pair<const Material*, uint8_t>>::value_type(
                    it->second.color.at(color_id).second,
                    std::make_pair(&it->second, (uint8_t)color_id)
                )
            );
        }
    }

    try {
        qb_read(path, &materials_inverse);
    } catch (const std::exception& e) {
        LOG_ERROR(logging::terrain_logger, "Could not load terrain save file due to {}", e.what());
        throw;
    }

    init_chunks();
}

// TODO remove this init, and make initializer lists for world
void
Terrain::init(int x, int y, int z) {
    X_MAX = x;
    Y_MAX = y;
    Z_MAX = z;
    tiles_.resize(0);
}

void
Terrain::init(
    int x, int y, int Area_size_, int z, int seed_,
    const std::map<int, const Material>* materials, Json::Value biome_data,
    std::vector<int> Terrain_Maps
) {
    auto millisec_since_epoch = time_util::epoch_millis();

    Area_size = Area_size_;
    seed = seed_;
    materials_ = materials;
    X_MAX = x * Area_size;
    Y_MAX = y * Area_size;
    Z_MAX = z;

    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        tiles_.push_back(Tile(sop(xyz), &materials_->at(0)));
    }

    srand(seed);
    LOG_INFO(logging::terrain_logger, "Start of land generator.");

    // create a map of int -> LandGenerator
    std::map<int, terrain_generation::LandGenerator> land_generators;

    // for tile macro in data biome
    for (unsigned int i = 0; i < biome_data["Tile_Macros"].size(); i++) {
        // create a land generator for each tile macro
        terrain_generation::LandGenerator gen(
            materials, biome_data["Tile_Macros"][i]["Land_Data"]
        );
        land_generators.insert(std::make_pair(i, gen));
    }
    // TODO make this faster 4
    for (int i = 0; i < x; i++)
        for (int j = 0; j < y; j++) {
            int tile_type = Terrain_Maps[j + i * y];
            Json::Value macro_types = biome_data["Tile_Data"][tile_type]["Land_From"];
            for (Json::Value generator_macro : macro_types) {
                init_area(i, j, land_generators[generator_macro.asInt()]);
            }
        }
    // TODO make this faster 3
    for (unsigned int i = 0; i < biome_data["After_Effects"]["Add_To_Top"].size();
         i++) {
        add_to_top(biome_data["After_Effects"]["Add_To_Top"][i], materials);
    }

    // grows the grass
    init_grass();

    //  TODO make this faster 1
    init_chunks();

    LOG_INFO(
        logging::terrain_logger, "End of land generator. Time elapsed: {}.",
        time_util::epoch_millis() - millisec_since_epoch
    );
}

void
Terrain::init_area(int area_x, int area_y, terrain_generation::LandGenerator gen) {
    while (!gen.empty()) {
        stamp_tile_region(gen.get_this_stamp(), area_x, area_y);
        ++gen;
    }
    gen.reset();
}

void
Terrain::init_chunks() {
    // chunk length in _ direction
    uint32_t C_length_X = ((X_MAX - 1) / Chunk::SIZE + 1);
    uint32_t C_length_Y = ((Y_MAX - 1) / Chunk::SIZE + 1);
    uint32_t C_length_Z = ((Z_MAX - 1) / Chunk::SIZE + 1);
    for (size_t xyz = 0; xyz < C_length_X * C_length_Y * C_length_Z; xyz += 1) {
        auto [x, y, z] = sop(xyz, C_length_X, C_length_Y, C_length_Z);
        chunks_.push_back(Chunk(x, y, z, this));
    }
}

void
Terrain::add_to_top(
    Json::Value top_data, const std::map<int, const Material>* materials
) {
    std::set<std::pair<int, int>> material_type;

    for (Json::Value::ArrayIndex i = 0; i < top_data["above_colors"].size(); i++) {
        int E = top_data["above_colors"][i]["E"].asInt();
        if (top_data["above_colors"][i]["C"].isInt()) {
            int C = top_data["above_colors"][i]["C"].asInt();
            material_type.insert(std::make_pair(E, C));
        } else if (top_data["above_colors"][i]["C"].asBool()) {
            for (unsigned int C = 0; C < (*materials).at(E).color.size(); C++) {
                material_type.insert(std::make_pair(E, C));
            }
        }
    }

    int guess = 0;
    // for loop
    for (int x = 0; x < X_MAX; x++)
        for (int y = 0; y < Y_MAX; y++) {
            // get first (not) z of material
            guess = get_first_not(material_type, x, y, guess);
            // if z is between some bounds
            // stop_h = get stop height (guess, top_data["how_to_add"])
            int max_height = get_stop_height(guess, top_data["how_to_add"]);
            for (int z = guess; z < max_height; z++) {
                set_tile_material(
                    get_tile(x, y, z),
                    &(*materials).at(top_data["Material_id"].asInt()),
                    top_data["Color_id"].asInt()
                );
            }
        }
}

int
Terrain::get_stop_height(int height, const Json::Value how_to_add) {
    for (unsigned int i = 0; i < how_to_add.size(); i++) {
        if (height >= how_to_add[i]["from"][0].asInt()
            && height < how_to_add[i]["from"][1].asInt()) {
            if (how_to_add[i]["to"].isInt()) {
                return how_to_add[i]["to"].asInt();
            } else {
                return height + how_to_add[i]["add"].asInt();
            }
        }
    }
    return 0;
}

int
Terrain::get_Z_solid(int x, int y, int z_start) {
    for (int z = z_start; z >= 0; z--) {
        if (this->get_tile(x, y, z)->is_solid()) {
            return z;
        }
    }
    return 0;
}

int
Terrain::get_Z_solid(int x, int y) {
    return get_Z_solid(x, y, Z_MAX - 1);
}

int
Terrain::get_first_not(
    const std::set<std::pair<int, int>> materials, int x, int y, int guess
) const {
    if (guess < 1) {
        guess = 1;
    }
    if (guess >= Z_MAX) {
        guess = Z_MAX - 1;
    }
    if (materials.find(std::make_pair(
            get_tile(x, y, guess - 1)->get_material_id(),
            get_tile(x, y, guess - 1)->get_color_id()
        ))
        != materials.end()) {
        if (materials.find(std::make_pair(
                get_tile(x, y, guess)->get_material_id(),
                get_tile(x, y, guess)->get_color_id()
            ))
            == materials.end()) {
            return guess;
        } else {
            // go up
            for (int z = guess + 1; z < Z_MAX; z++) {
                if (materials.find(std::make_pair(
                        get_tile(x, y, z)->get_material_id(),
                        get_tile(x, y, z)->get_color_id()
                    ))
                    == materials.end()) {
                    return z;
                }
            }
            return Z_MAX; // -1?
        }
    } else {
        // go down
        for (int z = guess - 2; z > 0; z--) {
            if (materials.find(std::make_pair(
                    get_tile(x, y, z)->get_material_id(),
                    get_tile(x, y, z)->get_color_id()
                ))
                != materials.end()) {
                return z + 1;
            }
        }
        return 0;
    }
}

bool
Terrain::can_stand(int x, int y, int z, int dz, int dxy) const {
    // x,y,z gives the position; dxy is the offset in x or y from the center,
    // and dz is the offset only upward
    if (!in_range(x + dxy - 1, y + dxy - 1, z - 1)
        || !in_range(x - dxy + 1, y - dxy + 1, z + dz - 1)) {
        return false;
    }

    if (!get_tile(x, y, z - 1)->is_solid()) {
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

bool
Terrain::can_stand(const Tile* tile, int dz, int dxy) const {
    return can_stand(tile->get_x(), tile->get_y(), tile->get_z(), dz, dxy);
}

bool
Terrain::can_stand(const Tile tile, int dz, int dxy) const {
    return can_stand(tile.get_x(), tile.get_y(), tile.get_z(), dz, dxy);
}

bool
Terrain::paint(Tile* tile, const Material* mat, uint8_t color_id) {
    // sets color_id if the material is the same.
    if (tile->get_material_id() == mat->element_id) {
        tile->set_color_id(color_id, mat);
        return true;
    }
    return false;
}

bool
Terrain::player_set_tile_material(int xyz, const Material* mat, uint8_t color_id) {
    Tile* tile = get_tile(xyz);
    if (tile->is_solid() && mat->solid) {
        // Can't change something from one material to another.
        return 0;
    }
    get_tile(xyz)->set_material(mat, color_id);
    return 1;
}

void
Terrain::stamp_tile_region(
    int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
    const Material* mat, std::set<std::pair<int, int>> elements_can_stamp,
    uint8_t color_id
) {
    // set tiles in region to mat and color_id if the current material is in
    // elements_can_stamp.
    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            for (int z = z_start; z < z_end; z++) {
                if (in_range(x, y, z)) {
                    Tile* tile = get_tile(x, y, z);
                    if (elements_can_stamp.find(std::make_pair(
                            static_cast<int>(tile->get_material_id()),
                            static_cast<int>(tile->get_color_id())
                        ))
                        != elements_can_stamp.end()) {
                        set_tile_material(tile, mat, color_id);
                    }
                }
            }
        }
    }
}

void
Terrain::stamp_tile_region(
    int x_start, int y_start, int z_start, int x_end, int y_end, int z_end,
    const Material* mat, uint8_t color_id
) {
    // set tiles in region to mat and color_id
    for (int x = x_start; x < x_end; x++) {
        for (int y = y_start; y < y_end; y++) {
            for (int z = z_start; z < z_end; z++) {
                if (in_range(x, y, z)) {
                    set_tile_material(get_tile(x, y, z), mat, color_id);
                }
            }
        }
    }
}

void
Terrain::init_grass() {
    std::set<Tile*> all_grass;

    // Test all ties to see if they can be grass.
    for (int x_ = 0; x_ < X_MAX; x_++)
        for (int y_ = 0; y_ < Y_MAX; y_++)
            for (int z_ = 0; z_ < Z_MAX - 1; z_++) {
                if (!get_tile(x_, y_, z_ + 1)->is_solid()) {
                    get_tile(x_, y_, z_)->try_grow_grass(); // add to sources and sinks
                    // if grass add to some set
                    if (get_tile(x_, y_, z_)->is_grass()) {
                        all_grass.insert(get_tile(x_, y_, z_));
                    }
                    // both higher, and lower set
                }
            }
    int z_ = Z_MAX - 1;
    for (int x_ = 0; x_ < X_MAX; x_++)
        for (int y_ = 0; y_ < Y_MAX; y_++) {
            get_tile(x_, y_, z_)->try_grow_grass(); // add to sources and sinks
            if (get_tile(x_, y_, z_)->is_grass()) {
                all_grass.insert(get_tile(x_, y_, z_));
            }
            // same thing here as above
        }
    grow_grass_high(all_grass);
    grow_grass_low(all_grass);
    for (Tile* t : all_grass) {
        t->set_grass_color(grass_grad_length_, grass_mid_, grass_colors_);
    }
}

// generates a size_x by size_y vector of macro tile types.
std::vector<int>
Terrain::generate_macro_map(
    unsigned int size_x, unsigned int size_y, Json::Value terrain_data
) {
    std::vector<int> out;
    int background = terrain_data["BackGround"].asInt(); // default terrain type.
    int numOctaves = terrain_data["NumOctaves"].asInt(); // number of octaves
    double persistance = terrain_data["Persistance"].asDouble();
    int range = terrain_data["Range"].asInt();
    int spacing = terrain_data["Spacing"].asInt();
    out.resize(size_x * size_y, background);
    terrain_generation::NoiseGenerator ng =
        terrain_generation::NoiseGenerator(numOctaves, persistance, 3);

    for (unsigned int i = 0; i < out.size(); i++) {
        auto [x, y, z] = sop(i, size_x, size_y, 1);
        auto p = ng.getValueNoise(
            static_cast<double>(x) * spacing, static_cast<double>(y) * spacing
        );
        out[i] = static_cast<int>((p + 1) * (p + 1) * range);
    }
    // TODO multi line logging
    for (unsigned int i = 0; i < size_x; i++) {
        for (unsigned int j = 0; j < size_y; j++) {
            std::cout << out[j + size_y * i] << " ";
        }
        std::cout << "\n";
    }
    return out;
}

// this should be the same as can_stand(x,y,z,1,1)
bool
Terrain::can_stand_1(int xyz) const {
    if (xyz % Z_MAX < 1 || xyz >= X_MAX * Y_MAX * Z_MAX) {
        return false;
    }
    return (!get_tile(xyz)->is_solid() && get_tile(xyz - 1)->is_solid());
}

float
Terrain::get_H_cost(std::array<float, 3> xyz1, std::array<float, 3> xyz2) {
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

float
Terrain::get_H_cost(std::array<int, 3> xyz1, std::array<int, 3> xyz2) {
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

template <class T>
float
Terrain::get_G_cost(const T tile, const Node<const T> node) {
    return node.get_time_cots() + get_H_cost(tile.sop(), node.get_tile()->sop());
}

int
Terrain::pos(const NodeGroup* const node_group) const {
    auto [x, y, z] = node_group->sop();
    int px = floor(x) / Chunk::SIZE;
    int py = floor(y) / Chunk::SIZE;
    int pz = floor(z) / Chunk::SIZE;
    return (px * Y_MAX / Chunk::SIZE * Z_MAX / Chunk::SIZE) + (py * Z_MAX / Chunk::SIZE)
           + pz;
}

std::set<Node<const NodeGroup>*>
Terrain::get_adjacent_nodes(
    const Node<const NodeGroup>* const node,
    std::map<size_t, Node<const NodeGroup>>& nodes, uint8_t path_type
) const {
    std::set<Node<const NodeGroup>*> out;
    for (const NodeGroup* t : node->get_tile()->get_adjacent_clear(path_type)) {
        try {
            Node<const NodeGroup>* tile = &nodes.at(pos_for_map(t));
            out.emplace(tile);
        } catch (const std::out_of_range& e) {}
    }
    return out;
}

std::set<Node<const Tile>*>
Terrain::get_adjacent_nodes(
    const Node<const Tile>* node, std::map<size_t, Node<const Tile>>& nodes,
    uint8_t path_type
) const {
    std::set<Node<const Tile>*> out;
    auto tile_it = get_tile_adjacent_iterator(pos(node->get_tile()), path_type);
    while (!tile_it.end()) {
        try {
            Node<const Tile>* tile = &nodes.at(tile_it.get_pos());
            out.emplace(tile);
        } catch (const std::out_of_range& e) {}
        tile_it++;
    }
    return out;
}

NodeGroup*
Terrain::get_node_group(int xyz) {
    try {
        return tile_to_group_.at(xyz);
    } catch (const std::out_of_range& e) {
        return nullptr;
    }
}

NodeGroup*
Terrain::get_node_group(const Tile t) {
    return get_node_group(pos(t));
}

NodeGroup*
Terrain::get_node_group(const Tile* t) {
    return get_node_group(pos(t));
}

void
Terrain::add_node_group(NodeGroup* NG) {
    for (const Tile* t : NG->get_tiles()) {
        tile_to_group_[pos(t)] = NG;
    }
}

void
Terrain::remove_node_group(NodeGroup* NG) {
    for (const Tile* t : NG->get_tiles()) {
        tile_to_group_.erase(pos(t));
    }
}

const UnitPath
Terrain::get_path_type(int xs, int ys, int zs, int xf, int yf, int zf) const {
    // the function should be passed the shape of the thing that wants to go on
    // the path just set them for now
    int dz = 3;
    int dxy = 1;

    // so what is going on? Only god knows.
    // abs(_s - _f) returns zero or one depending on wether the final and
    // initial positions are the same. same as bool (_s != _f)
    uint8_t x_diff = abs(xs - xf);
    uint8_t y_diff = abs(ys - yf);
    uint8_t z_diff = abs(zs - zf);

    // If there is a change in the horizontal position, then everything should
    // be bit shifted by 3, and if not, by 1.
    // This is because Directional flags are defined as follows:
    // 32   16  8  4  2 1
    // VH2 VH1  V H2 H1 O
    uint8_t horizontal_direction = (x_diff + y_diff) << (1 + 2 * z_diff);
    uint8_t vertical_direction = z_diff << 1;

    bool open;
    UnitPath type(horizontal_direction + vertical_direction);
    if (type == DirectionFlags::HORIZONTAL1 || type == DirectionFlags::VERTICAL) {
        // up / down or side to side
        // in this case the two tiles are bordering
        // same lever so the only thing that maters if the entity can stand on
        // both tiles
        open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz, dxy);
    } else if (type == DirectionFlags::HORIZONTAL2) {
        // still the same level
        // this test if the start and final locations are open,
        // and if the two between them are open
        // S O
        // O F
        open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz, dxy)
               && can_stand(xs, yf, zs, dz, dxy) && can_stand(xf, ys, zf, dz, dxy);
    } else if (type == DirectionFlags::UP_AND_OVER) {
        if (zf > zs) {
            // going up, and over
            open = can_stand(xs, ys, zs, dz + 1, dxy) && can_stand(xf, yf, zf, dz, dxy);
        } else {
            // going down and over
            open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz + 1, dxy);
        }

    } else if (type == DirectionFlags::UP_AND_DIAGONAL) {
        if (zf > zs) {
            // going up, and diagonal
            open = can_stand(xs, ys, zs, dz + 1, dxy) && can_stand(xf, yf, zf, dz, dxy)
                   && ((can_stand(xf, ys, zs, dz + 1, dxy)
                        || can_stand(xf, ys, zf, dz, dxy))
                       && (can_stand(xs, yf, zs, dz + 1, dxy)
                           || can_stand(xs, yf, zf, dz, dxy)));
        } else {
            // going down and diagonal
            open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz + 1, dxy)
                   && ((can_stand(xf, ys, zs, dz, dxy)
                        || can_stand(xf, ys, zf, dz + 1, dxy))
                       && (can_stand(xs, yf, zs, dz, dxy)
                           || can_stand(xs, yf, zf, dz + 1, dxy)));
        }
    }

    return type & UnitPath(open);
}

std::set<const NodeGroup*>
Terrain::get_all_node_groups() const {
    std::set<const NodeGroup*> out;
    for (size_t c = 0; c < chunks_.size(); c++) {
        chunks_[c].add_nodes_to(out);
    }
    return out;
}

std::vector<const Tile*>
Terrain::get_path_Astar(const Tile* start_, const Tile* goal_) {
    NodeGroup* goal_node;
    NodeGroup* start_node;
    int goal_z = goal_->get_z();
    Tile* goal;
    if (!can_stand_1(goal_)) {
        goal_z = get_Z_solid(goal_->get_x(), goal_->get_y()) + 1;
        if (goal_z != 0) {
            goal = get_tile(goal_->get_x(), goal_->get_y(), goal_z);
        } else {
            return std::vector<const Tile*>();
        }
    } else {
        return std::vector<const Tile*>();
    }
    int start_z = start_->get_z();
    Tile* start;
    if (!can_stand_1(start_)) {
        start_z = get_Z_solid(start_->get_x(), start_->get_y()) + 1;
        if (start_z != 0) {
            start = get_tile(start_->get_x(), start_->get_y(), start_z);
        } else {
            return std::vector<const Tile*>();
        }
    } else {
        return std::vector<const Tile*>();
    }

    if (!(goal_node = get_node_group(goal))) {
        return std::vector<const Tile*>();
    }
    if (!(start_node = get_node_group(start))) {
        return std::vector<const Tile*>();
    }

    std::vector<const NodeGroup*> Node_path = get_path_Astar(start_node, goal_node);
    // if Node_path is empty then return
    if (Node_path.size() == 0) {
        return std::vector<const Tile*>();
    }
    std::set<const Tile*> search_through;
    for (const NodeGroup* NG : Node_path) {
        auto tiles = NG->get_tiles();
        search_through.insert(tiles.begin(), tiles.end());
    }

    std::function<bool(Node<const Tile>*, Node<const Tile>*)> compare =
        [](Node<const Tile>* lhs, Node<const Tile>* rhs) -> bool {
        return lhs->get_total_predicted_cost() > rhs->get_total_predicted_cost();
    };

    return get_path(start, {goal}, search_through, compare);
}

std::vector<const NodeGroup*>
Terrain::get_path_Astar(const NodeGroup* start, const NodeGroup* goal) {
    std::function<bool(Node<const NodeGroup>*, Node<const NodeGroup>*)> compare =
        [](Node<const NodeGroup>* lhs, Node<const NodeGroup>* rhs) -> bool {
        return lhs->get_total_predicted_cost() > rhs->get_total_predicted_cost();
    };

    std::set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path(start, {goal}, search_through, compare);
}

std::vector<const NodeGroup*>
Terrain::get_path_breadth_first(
    const NodeGroup* start, const std::set<const NodeGroup*> goal
) {
    std::function<bool(Node<const NodeGroup>*, Node<const NodeGroup>*)> compare =
        [](Node<const NodeGroup>* lhs, Node<const NodeGroup>* rhs) -> bool {
        return lhs->get_time_cots() > rhs->get_time_cots();
    };

    std::set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path(start, goal, search_through, compare);
}

std::vector<const Tile*>
Terrain::get_path_breadth_first(const Tile* start, const std::set<const Tile*> goal_) {
    std::set<const NodeGroup*> goal_nodes;
    bool NoGoal = true;
    for (const Tile* g : goal_) {
        int goal_z = g->get_z();
        if (!can_stand_1(g)) {
            goal_z = get_Z_solid(g->get_x(), g->get_y()) + 1;
            if (goal_z != 0) {
                NoGoal = false;
                goal_nodes.insert(
                    get_node_group(get_tile(g->get_x(), g->get_y(), goal_z))
                );
            }
        } else {
            NoGoal = false;
            goal_nodes.insert(get_node_group(g));
        }
    }
    if (NoGoal) { // in this case there is no valid z position at one of the
                  // given x, y positions
        return std::vector<const Tile*>();
    }

    std::vector<const NodeGroup*> Node_path =
        get_path_breadth_first(get_node_group(start), goal_nodes);

    const Tile* goal = nullptr;
    const NodeGroup* end = Node_path.back();
    for (const Tile* g : goal_) {
        if (end->get_tiles().find(g) != end->get_tiles().end()) {
            goal = g;
            break;
        }
    }
    std::set<const Tile*> search_through;
    for (const NodeGroup* NG : Node_path) {
        search_through.insert(NG->get_tiles().begin(), NG->get_tiles().end());
    }

    std::function<bool(Node<const Tile>*, Node<const Tile>*)> compare =
        [](Node<const Tile>* lhs, Node<const Tile>* rhs) -> bool {
        return lhs->get_total_predicted_cost() > rhs->get_total_predicted_cost();
    };

    return get_path(start, {goal}, search_through, compare);
}

template <class T>
std::vector<const T*>
Terrain::get_path(
    const T* start, const std::set<const T*> goal,
    const std::set<const T*> search_through,
    std::function<bool(Node<const T>*, Node<const T>*)> compare
) const {
    auto T_compare = [&compare](Node<const T>* lhs, Node<const T>* rhs) {
        return compare(lhs, rhs);
    };

    std::priority_queue<
        Node<const T>*, std::vector<Node<const T>*>, decltype(T_compare)>
        openNodes(T_compare);

    std::map<size_t, Node<const T>> nodes; // The nodes that can be walked through
    for (const T* t : search_through) {
        nodes.insert(
            {pos_for_map(t),
             Node<const T>(t, get_H_cost(t->sop(), (*goal.begin())->sop()))}
        );
    }
    Node<const T> start_node = nodes[pos_for_map(start)];
    openNodes.push(&start_node); // gotta start somewhere
    start_node.explore();

    while (!openNodes.empty()) {
        Node<const T>* choice = openNodes.top();
        openNodes.pop(); // Remove the chosen node from openNodes
        // Expand openNodes around the best choice
        std::set<Node<const T>*> adjacent_nodes = get_adjacent_nodes(choice, nodes, 31);
        for (Node<const T>* n : adjacent_nodes) {
            // if can stand on the tile    and the tile is not explored
            // get_adjacent should only give open nodes
            if (!n->is_explored()) {
                n->explore(choice, get_G_cost(*(n->get_tile()), *choice));
                if (goal.find(n->get_tile()) != goal.end()) {
                    std::vector<const T*> path;
                    get_path_through_nodes(n, path, start);
                    return path;
                }
                openNodes.push(n); // n can be chose to expand around
            } else {
                // openNodes.remove n
                n->explore(choice, get_G_cost(*(n->get_tile()), *choice));
                // openNodes.add n
            }
        }
    }
    std::vector<const T*> path;
    return path;
}

uint32_t
Terrain::get_voxel(int x, int y, int z) const {
    // using static ints to prevent dereferencing
    // The previous material id, and color id are cashed so that materials do
    // not need to be dereferenced, and searched through.
    static uint8_t previous_mat_id = 0;
    static uint8_t previous_color_id = 0;
    static uint32_t previous_out_color = 0;

    if (!in_range(x, y, z)) {
        return 0;
    }

    uint8_t mat_id = tiles_[pos(x, y, z)].get_material_id();
    uint8_t color_id = tiles_[pos(x, y, z)].get_color_id();

    if (mat_id != previous_mat_id || color_id != previous_color_id) {
        previous_mat_id = mat_id;
        previous_color_id = color_id;

        auto mat = materials_->at(previous_mat_id);
        previous_out_color = mat.color[previous_color_id].second;
    }

    return previous_out_color;
}

// Set `color` to the color of the tile at `pos`.
void
Terrain::export_color(const int sop[3], uint8_t color[4]) const {
    uint32_t tile_color = get_voxel(sop[0], sop[1], sop[2]);
    color[0] = (tile_color >> 24) & 0xFF;
    color[1] = (tile_color >> 16) & 0xFF;
    color[2] = (tile_color >> 8) & 0xFF;
    color[3] = tile_color & 0xFF;
}

uint32_t
Terrain::compress_color(uint8_t v[4]) {
    return (uint32_t)v[3] | (uint32_t)v[2] << 8 | (uint32_t)v[1] << 16
           | (uint32_t)v[0] << 24;
}

void
Terrain::qb_save_debug(const std::string path) {
    int x = 0;
    for (Chunk& c : chunks_) {
        std::set<const NodeGroup*> node_groups;
        c.add_nodes_to(node_groups);
        for (const NodeGroup* NG : node_groups) {
            for (const Tile* t : NG->get_tiles()) {
                set_tile_material(get_tile(pos(t->sop())), &materials_->at(7), x % 4);
            }
            x++;
        }
    }
    qb_save(path);
}

// Save all tiles as .qb to path.
void
Terrain::qb_save(const std::string path) const {
    // Saves the tiles in this to the path specified
    voxel_utility::to_qb(path, *this);
}

void
Terrain::qb_read(
    const std::string path,
    const std::map<uint32_t, std::pair<const Material*, uint8_t>>* materials
) {
    std::vector<uint32_t> data;
    std::array<int32_t, 3> center;
    std::array<uint32_t, 3> size;

    voxel_utility::from_qb(path, data, center, size);

    X_MAX = size[0];
    Y_MAX = size[1];
    Z_MAX = size[2];
    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    std::set<uint32_t> unknown_materials;

    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        auto [x, y, z] = sop(xyz);
        uint32_t color = data[xyz];
        if (color == 0) {                      // if the qb voxel is transparent.
            auto mat_color = materials->at(0); // set the materials to air
            tiles_.push_back(Tile({x, y, z}, mat_color.first, mat_color.second));
        } else if (materials->count(color)) { // if the color is known
            auto mat_color = materials->at(color);
            tiles_.push_back(Tile({x, y, z}, mat_color.first, mat_color.second));
        } else { // the color is unknown
            unknown_materials.insert(color);
            auto mat_color = materials->at(0); // else set to air.
            tiles_.push_back(Tile({x, y, z}, mat_color.first, mat_color.second));
        }
    }

    for (uint32_t color : unknown_materials) {
        // is there any way to log hexadecimal
        LOG_WARNING(logging::terrain_logger, "Cannot find color: {:x}", color);
    }
}

std::pair<Tile*, Tile*>
Terrain::get_start_end_test() {
    std::pair<Tile*, Tile*> out;
    bool first = true;
    for (int xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        if (get_tile(xyz)->get_material_id() == 7
            && get_tile(xyz)->get_color_id() == 4) {
            if (first) {
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

} // namespace terrain
