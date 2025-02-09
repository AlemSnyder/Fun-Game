#include "terrain.hpp"

#include "chunk.hpp"
#include "generation/land_generator.hpp"
#include "generation/noise.hpp"
#include "generation/tile_stamp.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "path/node.hpp"
#include "terrain_base.hpp"
#include "terrain_helper.hpp"
#include "tile.hpp"
#include "types.hpp"
#include "util/time.hpp"
#include "util/voxel.hpp"
#include "util/voxel_io.hpp"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

namespace terrain {

namespace helper {

template <class T>
inline bool
astar_compare(Node<T>* lhs, Node<T>* rhs) {
    return lhs->get_total_predicted_cost() > rhs->get_total_predicted_cost();
}

template <class T>
inline bool
breadth_first_compare(Node<T>* lhs, Node<T>* rhs) {
    return lhs->get_time_cost() > rhs->get_time_cost();
}

} // namespace helper

Terrain::Terrain(
    const std::string& path, const generation::Biome& biome

) :
    TerrainBase(biome, voxel_utility::from_qb(path)) {
    // grows the grass
    init_grass();

    LOG_DEBUG(logging::terrain_logger, "End of land generator: grass.");

    //  TODO make this faster 1
    init_chunks();

    LOG_DEBUG(logging::terrain_logger, "End of land generator: chunks.");
}

Terrain::Terrain(
    Dim x, Dim y, Dim area_size_, Dim z, int seed_, const generation::Biome& biome,
    const generation::TerrainMacroMap& macro_map
) :
    TerrainBase(x, y, area_size_, z, biome, macro_map),
    seed(seed_) {
    // grows the grass
    init_grass();

    LOG_DEBUG(logging::terrain_logger, "End of land generator: grass.");

    //  TODO make this faster 1
    init_chunks();

    LOG_DEBUG(logging::terrain_logger, "End of land generator: chunks.");
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

TerrainOffset
Terrain::get_Z_solid(TerrainOffset x, TerrainOffset y, TerrainOffset z_start) const {
    for (TerrainOffset z = z_start; z >= 0; z--) {
        if (this->get_tile(x, y, z)->is_solid()) {
            return z;
        }
    }
    return 0;
}

TerrainOffset
Terrain::get_Z_solid(TerrainOffset x, TerrainOffset y) const {
    return get_Z_solid(x, y, Z_MAX - 1);
}

bool
Terrain::can_stand(
    TerrainOffset x, TerrainOffset y, TerrainOffset z, TerrainOffset dz,
    TerrainOffset dxy
) const {
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
    for (TerrainOffset xp = -dxy + 1; xp < dxy; xp++) {
        for (TerrainOffset yp = -dxy + 1; yp < dxy; yp++) {
            for (TerrainOffset zp = 0; zp < dz; zp++) {
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
Terrain::can_stand(const Tile* tile, TerrainOffset dz, TerrainOffset dxy) const {
    return can_stand(tile->get_x(), tile->get_y(), tile->get_z(), dz, dxy);
}

bool
Terrain::can_stand(const Tile tile, TerrainOffset dz, TerrainOffset dxy) const {
    return can_stand(tile.get_x(), tile.get_y(), tile.get_z(), dz, dxy);
}

bool
Terrain::paint(Tile* tile, const material_t* mat, ColorId color_id) {
    // sets color_id if the material is the same.
    if (tile->get_material_id() == mat->material_id) {
        tile->set_color_id(color_id, mat);
        return true;
    }
    return false;
}

bool
Terrain::player_set_tile_material(
    TileIndex xyz, const material_t* mat, ColorId color_id
) {
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
    std::unordered_set<Tile*> all_grass;

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
Terrain::can_stand_1(TileIndex xyz) const {
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
    return node.get_time_cost() + get_H_cost(tile.sop(), node.get_tile()->sop());
}

TileIndex
Terrain::pos(const NodeGroup* const node_group) const {
    auto [x, y, z] = node_group->sop();
    TerrainOffset px = floor(x) / Chunk::SIZE;
    TerrainOffset py = floor(y) / Chunk::SIZE;
    TerrainOffset pz = floor(z) / Chunk::SIZE;
    return (px * Y_MAX / Chunk::SIZE * Z_MAX / Chunk::SIZE) + (py * Z_MAX / Chunk::SIZE)
           + pz;
}

ChunkIndex
Terrain::get_chunk_index_from_tile(Dim x, Dim y, Dim z) const {
    TerrainOffset px = floor(x) / Chunk::SIZE;
    TerrainOffset py = floor(y) / Chunk::SIZE;
    TerrainOffset pz = floor(z) / Chunk::SIZE;
    return (px * Y_MAX / Chunk::SIZE * Z_MAX / Chunk::SIZE) + (py * Z_MAX / Chunk::SIZE)
           + pz;
}

ChunkPos
Terrain::get_chunk_pos_from_tile(Dim x, Dim y, Dim z) const {
    TerrainOffset px = floor(x) / Chunk::SIZE;
    TerrainOffset py = floor(y) / Chunk::SIZE;
    TerrainOffset pz = floor(z) / Chunk::SIZE;
    return {px, py, pz};
}

ChunkIndex
Terrain::get_chunk_index_from_pos(ChunkPos pos) const {
    return (pos.x * Y_MAX / Chunk::SIZE * Z_MAX / Chunk::SIZE)
           + (pos.y * Z_MAX / Chunk::SIZE) + pos.z;
}

std::unordered_set<Node<const NodeGroup>*>
Terrain::get_adjacent_nodes(
    const Node<const NodeGroup>* const node,
    std::unordered_map<TileIndex, Node<const NodeGroup>>& nodes, path_t path_type
) const {
    std::unordered_set<Node<const NodeGroup>*> out;
    for (const NodeGroup* t : node->get_tile()->get_adjacent_clear(path_type)) {
        auto adj_node = nodes.find(pos_for_map(t));
        if (adj_node != nodes.end()) {
            out.emplace(&adj_node->second);
        }
    }
    return out;
}

std::unordered_set<Node<const Tile>*>
Terrain::get_adjacent_nodes(
    const Node<const Tile>* node,
    std::unordered_map<TileIndex, Node<const Tile>>& nodes, path_t path_type
) const {
    std::unordered_set<Node<const Tile>*> out;
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
Terrain::get_node_group(TileIndex xyz) {
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
Terrain::get_node_group(TileIndex xyz) const {
    auto out = tile_to_group_.find(xyz);
    if (out == tile_to_group_.end()) {
        return nullptr;
    }
    return out->second;
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
Terrain::get_path_type(
    TerrainOffset xs, TerrainOffset ys, TerrainOffset zs, TerrainOffset xf,
    TerrainOffset yf, TerrainOffset zf
) const {
    // #lizard forgives the complexity
    // Because there are 8 path types that can be open or closed. This also
    // tests if 2-6 tiles are open which also increases the complexity.
    // the function should be passed the shape of the thing that wants to go on
    // the path just set them for now
    TerrainOffset dz = 3;
    TerrainOffset dxy = 1;

    // so what is going on? Only god knows.
    // abs(_s - _f) returns zero or one depending on wether the final and
    // initial positions are the same. same as bool (_s != _f)
    path_t x_diff = abs(xs - xf); // difference in x direction
    path_t y_diff = abs(ys - yf); // difference in y direction
    path_t z_diff = abs(zs - zf); // difference in z direction

    // If there is a change in the horizontal position, then everything should
    // be bit shifted by 4, and if not, by 1.
    // This is because Directional flags are defined as follows:
    // 32   16  8  4  2 1
    // VH2 VH1  V H2 H1 O
    path_t horizontal_direction = (x_diff + y_diff) << (1 + 3 * z_diff);
    path_t vertical_direction = z_diff << 3;
    UnitPath type;
    if (horizontal_direction == 0)
        type = vertical_direction;
    else
        type = horizontal_direction;

    bool path_open = false;
    if (type == DirectionFlags::HORIZONTAL1 || type == DirectionFlags::VERTICAL) {
        // up / down or side to side
        // in this case the two tiles are bordering
        // same lever so the only thing that maters if the entity can stand on
        // both tiles
        path_open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz, dxy);
    } else if (type == DirectionFlags::HORIZONTAL2) {
        // still the same level
        // this test if the start and final locations are open,
        // and if the two between them are open
        // S O
        // O F
        path_open = can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz, dxy)
                    && can_stand(xs, yf, zs, dz, dxy) && can_stand(xf, ys, zf, dz, dxy);
    } else if (type == DirectionFlags::UP_AND_OVER) {
        if (zf > zs) {
            // going up, and over
            path_open =
                can_stand(xs, ys, zs, dz + 1, dxy) && can_stand(xf, yf, zf, dz, dxy);
        } else {
            // going down and over
            path_open =
                can_stand(xs, ys, zs, dz, dxy) && can_stand(xf, yf, zf, dz + 1, dxy);
        }

    } else if (type == DirectionFlags::UP_AND_DIAGONAL) {
        if (zf > zs) {
            // going up, and diagonal
            path_open = can_stand(xs, ys, zs, dz + 1, dxy)
                        && can_stand(xf, yf, zf, dz, dxy)
                        && ((can_stand(xf, ys, zs, dz + 1, dxy)
                             || can_stand(xf, ys, zf, dz, dxy))
                            && (can_stand(xs, yf, zs, dz + 1, dxy)
                                || can_stand(xs, yf, zf, dz, dxy)));
        } else {
            // going down and diagonal
            path_open = can_stand(xs, ys, zs, dz, dxy)
                        && can_stand(xf, yf, zf, dz + 1, dxy)
                        && ((can_stand(xf, ys, zs, dz, dxy)
                             || can_stand(xf, ys, zf, dz + 1, dxy))
                            && (can_stand(xs, yf, zs, dz, dxy)
                                || can_stand(xs, yf, zf, dz + 1, dxy)));
        }
    }

    return type | UnitPath(path_open);
}

std::unordered_set<const NodeGroup*>
Terrain::get_all_node_groups() const {
    std::unordered_set<const NodeGroup*> out;
    for (size_t c = 0; c < chunks_.size(); c++) {
        chunks_[c].add_nodes_to(out);
    }
    return out;
}

std::optional<std::vector<const NodeGroup*>>
Terrain::get_path_Astar(const NodeGroup* start, const NodeGroup* goal) const {
    std::unordered_set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path<NodeGroup, helper::astar_compare>(start, {goal}, search_through);
}

std::optional<std::vector<const Tile*>>
Terrain::get_path_Astar(const Tile* start, const Tile* goal) const {
    const NodeGroup* goal_node;
    const NodeGroup* start_node;

    if (!(goal_node = get_node_group(goal)))
        return {};
    if (!(start_node = get_node_group(start)))
        return {};
    auto node_path = get_path_Astar(start_node, goal_node);
    // if node_path is empty then return
    if (!node_path.has_value())
        return {};

    std::unordered_set<const Tile*> search_through;
    for (const NodeGroup* NG : node_path.value()) {
        auto tiles = NG->get_tiles();
        search_through.insert(tiles.begin(), tiles.end());
    }

    return get_path<Tile, helper::astar_compare>(start, {goal}, search_through);
}

std::optional<std::vector<const NodeGroup*>>
Terrain::get_path_breadth_first(
    const NodeGroup* start, const std::unordered_set<const NodeGroup*> goal
) const {
    std::unordered_set<const NodeGroup*> search_through = get_all_node_groups();

    return get_path<NodeGroup, helper::breadth_first_compare>(
        start, goal, search_through
    );
}

std::optional<std::vector<const Tile*>>
Terrain::get_path_breadth_first(
    const Tile* start, const std::unordered_set<const Tile*> goal_
) {
    std::unordered_set<const NodeGroup*> goal_nodes;
    bool no_goal = true;
    for (const Tile* g : goal_) {
        if (can_stand_1(g)) {
            no_goal = false;
            const NodeGroup* goal_node = get_node_group(g);
            goal_nodes.insert(goal_node);
        }
    }
    if (no_goal) { // in this case there is no valid z position at one of the
                   // given x, y positions
        return {};
    }
    auto start_node = get_node_group(start);
    if (!start_node)
        return {};
    auto node_path = get_path_breadth_first(start_node, goal_nodes);
    if (!node_path)
        return {};
    const NodeGroup* end = node_path.value().back();
    ChunkIndex chunk_index = get_chunk_index_from_tile(
        end->get_center_x(), end->get_center_y(), end->get_center_z()
    );

    const Tile* goal = nullptr;
    for (const Tile* g : goal_) {
        if (get_chunk_index_from_tile(g->sop()) == chunk_index) {
            if (end->get_tiles().find(g) != end->get_tiles().end()) {
                goal = g;
                break;
            }
        }
    }
    if (!goal)
        return {};
    std::unordered_set<const Tile*> search_through;
    for (const NodeGroup* group : node_path.value()) {
        auto tiles = group->get_tiles();
        search_through.insert(tiles.begin(), tiles.end());
    }

    return get_path<Tile, helper::breadth_first_compare>(start, {goal}, search_through);
}

template <class T, bool compare(Node<const T>*, Node<const T>*)>
std::optional<std::vector<const T*>>
Terrain::get_path(
    const T* start, const std::unordered_set<const T*> goal,
    const std::unordered_set<const T*> search_through
) const {
    std::priority_queue<Node<const T>*, std::vector<Node<const T>*>, decltype(compare)>
        openNodes(compare);

    std::unordered_map<TileIndex, Node<const T>>
        nodes; // The nodes that can be walked through
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
        std::unordered_set<Node<const T>*> adjacent_nodes =
            get_adjacent_nodes(choice, nodes, 31);
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
                if (n->get_time_cost() > get_G_cost(*(n->get_tile()), *choice)) {
                    n->explore(choice, get_G_cost(*(n->get_tile()), *choice));

                    // will update open nodes if the fastest path has changed.
                    std::make_heap(
                        const_cast<Node<const T>**>(&openNodes.top()),
                        const_cast<Node<const T>**>(&openNodes.top())
                            + openNodes.size(),
                        compare
                    );
                } else {
                    n->explore(choice, get_G_cost(*(n->get_tile()), *choice));
                }
            }
        }
    }
    return {};
}

void
Terrain::qb_save_debug(const std::string path) {
    // used to determine a debug color for each node group
    size_t debug_color = 0;
    for (Chunk& c : chunks_) {
        std::unordered_set<const NodeGroup*> node_groups;
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
