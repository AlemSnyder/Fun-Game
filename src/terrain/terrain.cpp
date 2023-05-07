#include "terrain.hpp"

#include "../logging.hpp"
#include "../types.hpp"
#include "../util/time.hpp"
#include "../util/voxel.hpp"
#include "../util/voxel_io.hpp"
#include "chunk.hpp"
#include "material.hpp"
#include "path/node.hpp"
#include "terrain_base.hpp"
#include "terrain_generation/land_generator.hpp"
#include "terrain_generation/noise.hpp"
#include "terrain_generation/tile_stamp.hpp"
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

// most important initializer
Terrain::Terrain(
    int x_tiles, int y_tiles, int area_size, int z_tiles, int seed_,
    const std::map<MaterialId, const Material>& material,
    std::vector<int> grass_grad_data, unsigned int grass_mid
) :
    TerrainBase(
        material, grass_grad_data, grass_mid, x_tiles, y_tiles, area_size, z_tiles
    ),
    seed(seed_) {}

// This function aims to generate a test that demonstrates the given terrain type
// to do this a three by three area where the given type is generated in the
// center is given
//
// 0,   0,  0
// 0, type, 0
// 0,   0,  0
//
// Is how the world map should look.

Terrain::Terrain(
    int area_size, int z_tiles, int tile_type, int seed_,
    const std::map<MaterialId, const Material>& material, const Json::Value biome_data,
    std::vector<int> grass_grad_data, unsigned int grass_mid
) :
    TerrainBase(
        3, 3, area_size, z_tiles, material, biome_data, grass_grad_data, grass_mid,
        {0, 0, 0, 0, tile_type, 0, 0, 0, 0}
    ),
    seed(seed_) {}

Terrain::Terrain(
    const std::string path, const std::map<MaterialId, const Material>& materials,
    std::vector<int> grass_grad_data, unsigned int grass_mid
) :
    TerrainBase(materials, grass_grad_data, grass_mid, voxel_utility::from_qb(path)) {
    /*replace path with some struct that is returned from qb read*/

    init_chunks();
}

Terrain::Terrain(
    int x, int y, int Area_size_, int z, int seed_,
    const std::map<uint8_t, const Material>& materials, const Json::Value biome_data,
    std::vector<int> grass_grad_data, unsigned int grass_mid
) :
    TerrainBase(x, y, Area_size_, z, materials, biome_data, grass_grad_data, grass_mid),
    seed(seed_) {
    auto millisec_since_epoch = time_util::epoch_millis();

    // grows the grass
    init_grass();

    LOG_INFO(logging::terrain_logger, "End of land generator: grass.");

    //  TODO make this faster 1
    init_chunks();

    LOG_INFO(
        logging::terrain_logger, "End of land generator. Time elapsed: {}.",
        time_util::epoch_millis() - millisec_since_epoch
    );
}

void
Terrain::init_chunks() {
    // chunk length in _ direction
    Dim C_length_X = ((X_MAX - 1) / Chunk::SIZE + 1);
    Dim C_length_Y = ((Y_MAX - 1) / Chunk::SIZE + 1);
    Dim C_length_Z = ((Z_MAX - 1) / Chunk::SIZE + 1);
    for (size_t xyz = 0; xyz < C_length_X * C_length_Y * C_length_Z; xyz += 1) {
        TerrainDim3 chunk_position = sop(xyz, C_length_X, C_length_Y, C_length_Z);
        chunks_.push_back(Chunk(chunk_position, this));
    }
}

int
Terrain::get_Z_solid(int x, int y, int z_start) const {
    for (int z = z_start; z >= 0; z--) {
        if (this->get_tile(x, y, z)->is_solid()) {
            return z;
        }
    }
    return 0;
}

int
Terrain::get_Z_solid(int x, int y) const {
    return get_Z_solid(x, y, Z_MAX - 1);
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
Terrain::paint(Tile* tile, const Material* mat, ColorId color_id) {
    // sets color_id if the material is the same.
    if (tile->get_material_id() == mat->element_id) {
        tile->set_color_id(color_id, mat);
        return true;
    }
    return false;
}

bool
Terrain::player_set_tile_material(int xyz, const Material* mat, ColorId color_id) {
    Tile* tile = get_tile(xyz);
    if (tile->is_solid() && mat->solid) {
        // Can't change something from one material to another.
        return 0;
    }
    get_tile(xyz)->set_material(mat, color_id);
    return 1;
}

void
Terrain::init_grass() {
    std::set<Tile*> all_grass;

    // Test all ties to see if they can be grass.
    for (size_t x = 0; x < X_MAX; x++)
        for (size_t y = 0; y < Y_MAX; y++)
            for (size_t z = 0; z < static_cast<Dim>(Z_MAX - 1); z++) {
                if (!get_tile(x, y, z + 1)->is_solid()) {
                    get_tile(x, y, z)->try_grow_grass(); // add to sources and sinks
                    // if grass add to some set
                    if (get_tile(x, y, z)->is_grass()) {
                        all_grass.insert(get_tile(x, y, z));
                    }
                    // both higher, and lower set
                }
            }
    size_t z = Z_MAX - 1;
    for (size_t x = 0; x < X_MAX; x++)
        for (size_t y = 0; y < Y_MAX; y++) {
            get_tile(x, y, z)->try_grow_grass(); // add to sources and sinks
            if (get_tile(x, y, z)->is_grass()) {
                all_grass.insert(get_tile(x, y, z));
            }
            // same thing here as above
        }
    grow_grass_low(all_grass);
    grow_grass_high(all_grass);
    for (Tile* t : all_grass) {
        t->set_grass_color(
            get_grass_grad_length(), get_grass_mid(), get_grass_colors()
        );
    }
}

// this should be the same as can_stand(x,y,z,1,1)
bool
Terrain::can_stand_1(int xyz) const {
    if (static_cast<TileIndex>(xyz) % Z_MAX < 1
        || static_cast<TileIndex>(xyz) >= X_MAX * Y_MAX * Z_MAX) {
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
Terrain::get_H_cost(TerrainDim3 position1, TerrainDim3 position2) {
    double D1 = 1.0;
    double D2 = 1.414;
    double D3 = 1.0;

    float DX = abs(position1.x - position2.x);
    float DY = abs(position1.y - position2.y);
    float DZ = abs(position1.z - position2.z);

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

uint16_t
Terrain::get_chunk_from_tile(uint8_t x, uint8_t y, uint8_t z) const {
    int px = floor(x) / Chunk::SIZE;
    int py = floor(y) / Chunk::SIZE;
    int pz = floor(z) / Chunk::SIZE;
    return (px * Y_MAX / Chunk::SIZE * Z_MAX / Chunk::SIZE)
           + (py * Z_MAX / Chunk::SIZE) + pz;
}

std::set<Node<const NodeGroup>*>
Terrain::get_adjacent_nodes(
    const Node<const NodeGroup>* const node,
    std::map<TileIndex, Node<const NodeGroup>>& nodes, uint8_t path_type
) const {
    std::set<Node<const NodeGroup>*> out;
    for (const NodeGroup* t : node->get_tile()->get_adjacent_clear(path_type)) {
        auto adj_node = nodes.find(pos_for_map(t));
        if (adj_node != nodes.end()) {
            out.emplace(&adj_node->second);
        }
    }
    return out;
}

std::set<Node<const Tile>*>
Terrain::get_adjacent_nodes(
    const Node<const Tile>* node, std::map<TileIndex, Node<const Tile>>& nodes,
    uint8_t path_type
) const {
    std::set<Node<const Tile>*> out;
    auto tile_it = get_tile_adjacent_iterator(pos(node->get_tile()), path_type);
    while (!tile_it.end()) {
        auto adj_node = nodes.find(tile_it.get_pos());
        if (adj_node != nodes.end()) {
            out.emplace(&adj_node->second);
        }
        tile_it++;
    }
    return out;
}

NodeGroup*
Terrain::get_node_group(int xyz) {
    auto node_group = tile_to_group_.find(xyz);
    if (node_group == tile_to_group_.end()) {
        return nullptr;
    }
    return node_group->second;
}

NodeGroup*
Terrain::get_node_group(const Tile t) {
    return get_node_group(pos(t));
}

NodeGroup*
Terrain::get_node_group(const Tile* t) {
    return get_node_group(pos(t));
}

const NodeGroup*
Terrain::get_node_group(int xyz) const {
    try {
        return tile_to_group_.at(xyz);
    } catch (const std::out_of_range& e) {
        return nullptr;
    }
}

const NodeGroup*
Terrain::get_node_group(const Tile t) const {
    return get_node_group(pos(t));
}

const NodeGroup*
Terrain::get_node_group(const Tile* t) const {
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
    uint8_t x_diff = abs(xs - xf); // difference in x direction
    uint8_t y_diff = abs(ys - yf); // difference in y direction
    uint8_t z_diff = abs(zs - zf); // difference in z direction

    // If there is a change in the horizontal position, then everything should
    // be bit shifted by 4, and if not, by 1.
    // This is because Directional flags are defined as follows:
    // 32   16  8  4  2 1
    // VH2 VH1  V H2 H1 O
    uint8_t horizontal_direction = (x_diff + y_diff) << (1 + 3 * z_diff);
    uint8_t vertical_direction = z_diff << 3;
    UnitPath type;
    if (horizontal_direction == 0)
        type = vertical_direction;
    else
        type = horizontal_direction;

    bool open;
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

    return type | UnitPath(open);
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
Terrain::get_path_Astar(const Tile* start_, const Tile* goal_) const {
    const NodeGroup* goal_node;
    const NodeGroup* start_node;
    int goal_z = goal_->get_z();
    const Tile* goal;

    if (!can_stand_1(goal_)) {
        goal_z = get_Z_solid(goal_->get_x(), goal_->get_y()) + 1;
        if (goal_z != 0) {
            goal = get_tile(goal_->get_x(), goal_->get_y(), goal_z);
        } else {
            return std::vector<const Tile*>();
        }
    } else {
        goal = goal_;
    }

    int start_z = start_->get_z();
    const Tile* start;

    if (!can_stand_1(start_)) {
        start_z = get_Z_solid(start_->get_x(), start_->get_y()) + 1;
        if (start_z != 0) {
            start = get_tile(start_->get_x(), start_->get_y(), start_z);
        } else {
            return std::vector<const Tile*>();
        }
    } else {
        start = start_;
    }

    if (!(goal_node = get_node_group(goal)))
        return std::vector<const Tile*>();

    if (!(start_node = get_node_group(start)))
        return std::vector<const Tile*>();

    std::vector<const NodeGroup*> Node_path = get_path_Astar(start_node, goal_node);
    // if Node_path is empty then return
    if (Node_path.size() == 0)
        return std::vector<const Tile*>();

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
Terrain::get_path_Astar(const NodeGroup* start, const NodeGroup* goal) const {
    std::function<bool(Node<const NodeGroup>*, Node<const NodeGroup>*)> compare =
        [](Node<const NodeGroup>* lhs, Node<const NodeGroup>* rhs) -> bool {
        return lhs->get_total_predicted_cost() > rhs->get_total_predicted_cost();
    };

    std::set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path(start, {goal}, search_through, compare);
}

std::vector<const NodeGroup*>
Terrain::get_path_breadth_first (
    const NodeGroup* start, const std::set<const NodeGroup*> goal
) const {
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
    bool no_goal = true;
    for (const Tile* g : goal_) {
        int goal_z = g->get_z();
        if (can_stand_1(g)) {
            no_goal = false;
            goal_nodes.insert(get_node_group(g));
        } else {
            goal_z = get_Z_solid(g->get_x(), g->get_y()) + 1;
            if (goal_z != 0) {
                no_goal = false;
                goal_nodes.insert(
                    get_node_group(get_tile(g->get_x(), g->get_y(), goal_z))
                );
            }
        }
    }
    if (no_goal) { // in this case there is no valid z position at one of the
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

    std::map<TileIndex, Node<const T>> nodes; // The nodes that can be walked through
    for (const T* t : search_through) {
        nodes[pos_for_map(t)] =
            Node<const T>(t, get_H_cost(t->sop(), (*goal.begin())->sop()));
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
                if (n->get_time_cots() > get_G_cost(*(n->get_tile()), *choice)) {
                    n->explore(choice, get_G_cost(*(n->get_tile()), *choice));

                    // will update open nodes if the fastest path has changed.
                    std::make_heap(
                        const_cast<Node<const T>**>(&openNodes.top()),
                        const_cast<Node<const T>**>(&openNodes.top())
                            + openNodes.size(),
                        T_compare
                    );
                } else {
                    n->explore(choice, get_G_cost(*(n->get_tile()), *choice));
                }
            }
        }
    }
    std::vector<const T*> path;
    return path;
}

void
Terrain::qb_save_debug(const std::string path) {
    // used to determine a debug color for each node group
    size_t debug_color = 0;
    for (Chunk& c : chunks_) {
        std::set<const NodeGroup*> node_groups;
        c.add_nodes_to(node_groups);
        for (const NodeGroup* NG : node_groups) {
            for (const Tile* t : NG->get_tiles()) {
                set_tile_material(
                    get_tile(pos(t->sop())), &get_materials().at(DEBUG_MATERIAL),
                    debug_color % NUM_DEBUG_COLORS
                );
            }
            debug_color++;
        }
    }
    qb_save(path);
}

// Save all tiles as .qb to path.
void
Terrain::qb_save(const std::string path) const {
    // Saves the tiles in this to the path specified
    voxel_utility::to_qb(std::filesystem::path(path), *this);
}

std::pair<const Tile*, const Tile*>
Terrain::get_start_end_test() const {
    std::pair<const Tile*, const Tile*> out;
    bool first = true;
    for (size_t xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
        if (get_tile(xyz)->get_material_id() == DEBUG_MATERIAL
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
