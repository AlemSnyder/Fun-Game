#include "terrain.hpp"

#include "chunk.hpp"
#include "generation/land_generator.hpp"
#include "generation/noise.hpp"
#include "generation/tile_stamp.hpp"
#include "logging.hpp"
#include "material.hpp"
#include "path/node.hpp"
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

Terrain::Terrain(const std::string& path, const generation::Biome& biome) :
    Terrain(biome, voxel_utility::from_qb(path)) {}

Terrain::Terrain(const generation::Biome& biome, voxel_utility::qb_data_t data) :
    area_size_(32), biome_(biome), X_MAX(data.size.x), Y_MAX(data.size.y),
    Z_MAX(data.size.z) {
    // tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    chunks_.reserve(X_MAX * Y_MAX * Z_MAX / Chunk::SIZE / Chunk::SIZE / Chunk::SIZE);

    auto materials_inverse = biome.get_colors_inverse_map();

    try {
        qb_read(data.data, materials_inverse);
    } catch (const std::exception& e) {
        LOG_ERROR(
            logging::terrain_logger, "Could not load terrain save file due to {}",
            e.what()
        );
        throw;
    }

    // grows the grass
    init_grass();

    LOG_INFO(logging::terrain_logger, "End of land generator: grass.");

    //  TODO make this faster 1
    init_chunks();

    LOG_INFO(logging::terrain_logger, "End of land generator: chunks.");
}

Terrain::Terrain(
    TerrainOffset x_map_tiles, TerrainOffset y_map_tiles, TerrainOffset area_size_,
    TerrainOffset z, int seed_, const generation::Biome& biome,
    const generation::TerrainMacroMap& macro_map
) :
    area_size_(area_size_), biome_(biome), seed(seed_), X_MAX(x_map_tiles * area_size_),
    Y_MAX(y_map_tiles * area_size_), Z_MAX(z) {
    // tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    chunks_.reserve(X_MAX * Y_MAX * Z_MAX / Chunk::SIZE / Chunk::SIZE / Chunk::SIZE);

    // for (size_t xyz = 0; xyz < X_MAX * Y_MAX * Z_MAX; xyz++) {
    //     tiles_.push_back(Tile(sop(xyz), biome_.get_material(0), 0));
    // }

    // srand(seed);
    LOG_INFO(logging::terrain_logger, "Start of land generator.");

    // TODO make this faster 4
    for (TerrainOffset i = 0; i < x_map_tiles; i++)
        for (TerrainOffset j = 0; j < y_map_tiles; j++) {
            generation::MapTile map_tile = macro_map.get_tile(i, j);
            for (auto generator_macro : map_tile.get_type()) {
                init_area(map_tile, *generator_macro);
            }
        }

    LOG_INFO(logging::terrain_logger, "End of land generator: place tiles.");

    // TODO make this faster 3
    for (const generation::AddToTop& top_data : biome.get_top_generators()) {
        add_to_top(top_data);
    }

    LOG_INFO(logging::terrain_logger, "End of land generator: top layer placement.");

    // grows the grass
    init_grass();

    LOG_INFO(logging::terrain_logger, "End of land generator: grass.");

    //  TODO make this faster 1
    init_chunks();

    LOG_INFO(logging::terrain_logger, "End of land generator: chunks.");
}

void
Terrain::qb_read(
    const std::vector<ColorInt> data,
    const std::unordered_map<ColorInt, MaterialColor>& materials_inverse
) {
    //    tiles_.reserve(X_MAX * Y_MAX * Z_MAX);

    std::unordered_set<ColorInt> unknown_colors;
    std::mutex unknown_colors_mutex_;

    for (TerrainOffset x = 0; x < X_MAX / Chunk::SIZE; x++) {
        for (TerrainOffset y = 0; y < Y_MAX / Chunk::SIZE; y++) {
            for (TerrainOffset z = 0; z < Z_MAX / Chunk::SIZE; z++) {
                chunks_.emplace(std::piecewise_construct, std::forward_as_tuple(TerrainDim3(x, y, z)), std::forward_as_tuple(x, y, z, this));
            }
        }
    }

    GlobalContext& context = GlobalContext::instance();

    // TODO pass X_MAX, Y_MAX, Z_MAX
    // define set tile in chunk

    for (auto& [chunk_position, chunk] : chunks_) {
        context.submit_task([chunk_position, &chunk, &materials_inverse, &data,
                             &unknown_colors, &unknown_colors_mutex_,
                             X_MAX = this->X_MAX, Y_MAX = this->X_MAX, Z_MAX = this->X_MAX]() {
            std::unique_lock chunk_lock(chunk.get_mutex());
            for (Dim xl = 0; xl < Chunk::SIZE; xl++) {
                for (Dim yl = 0; yl < Chunk::SIZE; yl++) {
                    for (Dim zl = 0; zl < Chunk::SIZE; zl++) {
                        TerrainDim3 tile_relative_position(xl, yl, zl);

                        TerrainOffset3 tile_position = tile_relative_position;
                            chunk_position * static_cast<TerrainOffset>(Chunk::SIZE);
                        size_t index = tile_position.x * Y_MAX * Z_MAX
                                       + tile_position.y * Z_MAX + tile_position.z;
                        ColorInt color = data[index];
                        const terrain::MaterialColor* mat_color;
                        if (color == 0) { // if the qb voxel is transparent.
                            mat_color =
                                &materials_inverse.at(0); // set the materials to air
                            // tiles_.push_back(Tile(tile_position, &mat_color.material,
                            // mat_color.color));
                        } else if (materials_inverse.count(color
                                   )) { // if the color is known
                            mat_color = &materials_inverse.at(color);
                            // tiles_.push_back(Tile(tile_position, &mat_color.material,
                            // mat_color.color));
                        } else { // the color is unknown
                            std::unique_lock lock(unknown_colors_mutex_);
                            unknown_colors.insert(color);
                            mat_color = &materials_inverse.at(0); // else set to air.
                            // tiles_.push_back(Tile(tile_position, &mat_color.material,
                            // mat_color.color));
                        }

                        Tile* tile = chunk.get_tile(tile_relative_position);

                        tile->set_material( &mat_color->material, mat_color->color);
                    }
                }
            }
        });
    }

    for (ColorInt color : unknown_colors) {
        LOG_WARNING(logging::terrain_logger, "Cannot find color: {:x}", color);
    }
}

TerrainOffset
Terrain::get_first_not(
    const MaterialGroup& materials, TerrainOffset x, TerrainOffset y,
    TerrainOffset guess
) const {
    if (guess < 1) {
        guess = 1;
    } else if (static_cast<TerrainOffset>(guess) >= Z_MAX) {
        guess = Z_MAX - 1;
    }
    if (has_tile_material(materials, x, y, guess - 1)) {
        if (!has_tile_material(materials, x, y, guess)) {
            return guess;
        } else {
            // go up
            for (TerrainOffset z = guess + 1; z < Z_MAX; z++) {
                if (!has_tile_material(materials, x, y, z)) {
                    return z;
                }
            }
            return Z_MAX; // -1? should not be minus one, but one should
                          // consider that this is a possible return value
        }
    } else if (guess == 1) {
        return 0;
    } else {
        // go down
        for (TerrainOffset z = guess - 2; z != 0; z--) {
            if (has_tile_material(materials, x, y, z)) {
                return z + 1;
            }
        }
        return 0;
    }
}

void
Terrain::add_to_top(const generation::AddToTop& top_data) {
    TerrainOffset guess = Z_MAX / 2;
    // for loop
    for (TerrainOffset x = 0; x < X_MAX; x++)
        for (TerrainOffset y = 0; y < Y_MAX; y++) {
            // get first (not) z of material
            guess = get_first_not(top_data.get_elements_above(), x, y, guess);
            // if z is between some bounds
            // stop_h = get stop height (guess, top_data["how_to_add"])
            TerrainOffset max_height = top_data.get_final_height(guess);
            for (TerrainOffset z = guess; z < max_height && z < Z_MAX; z++) {
                const Tile& tile = *get_tile(x, y, z);
                if (top_data.can_overwrite_material(
                        tile.get_material_id(), tile.get_color_id()
                    )) {
                    get_tile(x, y, z)->set_material(
                        biome_.get_material(top_data.get_material_id()),
                        top_data.get_color_id()
                    );
                }
            }
        }
}

void
Terrain::stamp_tile_region(
    const generation::TileStamp& stamp, TerrainOffset x_offset = 0,
    TerrainOffset y_offset = 0
) {
    // set tiles in region to mat and color_id if the current material is in
    // elements_can_stamp.
    TerrainOffset x_start = stamp.x_start + x_offset * area_size_ + area_size_ / 2;
    TerrainOffset y_start = stamp.y_start + y_offset * area_size_ + area_size_ / 2;
    TerrainOffset x_end = stamp.x_end + x_offset * area_size_ + area_size_ / 2;
    TerrainOffset y_end = stamp.y_end + y_offset * area_size_ + area_size_ / 2;

    for (TerrainOffset x = x_start; x < x_end; x++) {
        for (TerrainOffset y = y_start; y < y_end; y++) {
            for (TerrainOffset z = stamp.z_start; z < stamp.z_end; z++) {
                if (in_range(x, y, z)) {
                    Tile* tile = get_tile(x, y, z);
                    if (stamp.elements_can_stamp.has_value()) {
                        if (stamp.elements_can_stamp.value().material_in(
                                tile->get_material_id(), tile->get_color_id()
                            ))
                            tile->set_material(
                                biome_.get_material(stamp.mat), stamp.color_id
                            );
                    } else {
                        tile->set_material(
                            biome_.get_material(stamp.mat), stamp.color_id
                        );
                    }
                }
            }
        }
    }
}

void
Terrain::init_area(generation::MapTile& map_tile, generation::LandGenerator gen) {
    while (!gen.empty()) {
        stamp_tile_region(
            gen.get_stamp(map_tile.get_rand_engine()), map_tile.get_x(),
            map_tile.get_y()
        );
        gen.next();
    }
    gen.reset();
}

void
Terrain::init_chunks() {
    // chunk length in _ direction
    TerrainOffset C_length_X = ((X_MAX - 1) / Chunk::SIZE + 1);
    TerrainOffset C_length_Y = ((Y_MAX - 1) / Chunk::SIZE + 1);
    TerrainOffset C_length_Z = ((Z_MAX - 1) / Chunk::SIZE + 1);
    for (TerrainOffset x = 0; x < C_length_X; x++) {
        for (TerrainOffset y = 0; y < C_length_Y; y++) {
            for (TerrainOffset z = 0; z < C_length_Z; z++) {
                TerrainOffset3 chunk_position(x, y, z);
                chunks_.emplace(std::piecewise_construct, std::forward_as_tuple(chunk_position), std::forward_as_tuple(chunk_position, this));
            }
        }
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

// bool
// Terrain::can_stand(const Tile* tile, TerrainOffset dz, TerrainOffset dxy) const {
//     return can_stand(tile->get_x(), tile->get_y(), tile->get_z(), dz, dxy);
// }

// bool
// Terrain::can_stand(const Tile tile, TerrainOffset dz, TerrainOffset dxy) const {
//     return can_stand(tile.get_x(), tile.get_y(), tile.get_z(), dz, dxy);
// }

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
    TerrainOffset3 xyz, const material_t* mat, ColorId color_id
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
    for (TerrainOffset x = 0; x < X_MAX; x++)
        for (TerrainOffset y = 0; y < Y_MAX; y++)
            for (TerrainOffset z = 0; z < static_cast<TerrainOffset>(Z_MAX - 1); z++) {
                if (!get_tile(x, y, z + 1)->is_solid()) {
                    get_tile(x, y, z)->try_grow_grass(); // add to sources and sinks
                    // if grass add to some set
                    if (get_tile(x, y, z)->is_grass()) {
                        all_grass.insert(get_tile(x, y, z));
                    }
                    // both higher, and lower set
                }
            }
    TerrainOffset z = Z_MAX - 1;
    for (TerrainOffset x = 0; x < X_MAX; x++)
        for (TerrainOffset y = 0; y < Y_MAX; y++) {
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
// bool
// Terrain::can_stand_1(TileIndex xyz) const {
//     if (static_cast<TileIndex>(xyz) % Z_MAX < 1
//         || static_cast<TileIndex>(xyz) >= X_MAX * Y_MAX * Z_MAX) {
//         return false;
//     }
//     return (!get_tile(xyz)->is_solid() && get_tile(xyz - 1)->is_solid());
// }

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
Terrain::get_H_cost(TerrainOffset3 position1, TerrainOffset3 position2) {
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
    return node.get_time_cost() + get_H_cost(tile.average_position(), node.get_tile()->average_position());
}

// TileIndex
// Terrain::pos(const NodeGroup* const node_group) const {
//     auto [x, y, z] = node_group->sop();
//     TerrainOffset px = floor(x) / Chunk::SIZE;
//     TerrainOffset py = floor(y) / Chunk::SIZE;
//     TerrainOffset pz = floor(z) / Chunk::SIZE;
//     return (px * Y_MAX / Chunk::SIZE * Z_MAX / Chunk::SIZE) + (py * Z_MAX /
//     Chunk::SIZE)
//            + pz;
// }

TerrainOffset3
Terrain::get_chunk_from_tile(TerrainOffset x, TerrainOffset y, TerrainOffset z) const {
    TerrainOffset px = floor(x) / Chunk::SIZE;
    TerrainOffset py = floor(y) / Chunk::SIZE;
    TerrainOffset pz = floor(z) / Chunk::SIZE;
    return {px, py, pz};
}

std::unordered_set<Node<const NodeGroupWrapper>*>
Terrain::get_adjacent_nodes(
    const Node<const NodeGroupWrapper>* const node,
    std::unordered_map<TerrainOffset3, Node<const NodeGroupWrapper>>& nodes, path_t path_type
) const {
    std::unordered_set<Node<const NodeGroupWrapper>*> out;
    for (const NodeGroup* t : node->get_tile()->get_adjacent_clear(path_type)) {
        auto adj_node = nodes.find(t->unique_position());
        if (adj_node != nodes.end()) {
            out.emplace(&adj_node->second);
        }
    }
    return out;
}

std::unordered_set<Node<const PositionWrapper>*>
Terrain::get_adjacent_nodes(
    const Node<const PositionWrapper>* node,
    std::unordered_map<TerrainOffset3, Node<const PositionWrapper>>& nodes, path_t path_type
) const {
    std::unordered_set<Node<const PositionWrapper>*> out;
    auto tile_it = get_tile_adjacent_iterator(node->get_tile()->unique_position(), path_type);
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
Terrain::get_node_group(TerrainOffset3 xyz) {
    auto node_group = tile_to_group_.find(xyz);
    if (node_group == tile_to_group_.end()) {
        return nullptr;
    }
    return node_group->second;
}

// NodeGroup*
// Terrain::get_node_group(const Tile t) {
//     return get_node_group(pos(t));
// }

// NodeGroup*
// Terrain::get_node_group(const Tile* t) {
//     return get_node_group(pos(t));
// }

const NodeGroup*
Terrain::get_node_group(TerrainOffset3 xyz) const {
    auto out = tile_to_group_.find(xyz);
    if (out == tile_to_group_.end()) {
        return nullptr;
    }
    return out->second;
}

// const NodeGroup*
// Terrain::get_node_group(const Tile t) const {
//     return get_node_group(pos(t));
// }

// const NodeGroup*
// Terrain::get_node_group(const Tile* t) const {
//     return get_node_group(pos(t));
// }

void
Terrain::add_node_group(NodeGroup* NG) {
    auto positions = NG->get_tiles();
    for (const auto t : positions) {
        tile_to_group_[t] = NG;
    }
}

void
Terrain::remove_node_group(NodeGroup* NG) {
    auto positions = NG->get_tiles();
    for (const auto t : positions) {
        tile_to_group_.erase(t);
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
    for (const auto& [position, chunk] : chunks_) {
        chunk.add_nodes_to(out);
    }
    return out;
}

std::optional<std::vector<NodeGroupWrapper>>
Terrain::get_path_Astar(const NodeGroup* start, const NodeGroup* goal) const {
    std::unordered_set<const NodeGroup*> search_through = get_all_node_groups();

    std::unordered_set<NodeGroupWrapper> search_through_wrapped({});
    for (const auto& node : search_through) {
        search_through_wrapped.emplace(node);
    }

    return get_path<NodeGroupWrapper, helper::astar_compare>(NodeGroupWrapper(start), {NodeGroupWrapper(goal)}, search_through_wrapped);
}

std::optional<std::vector<TerrainOffset3>>
Terrain::get_path_Astar(TerrainOffset3 start, TerrainOffset3 goal) const {
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

    std::unordered_set<PositionWrapper> search_through({});
    for (const auto NG : node_path.value()) {
        auto tiles = NG.get_tiles();
        search_through.insert(tiles.begin(), tiles.end());
    }



    auto wrapped_path = get_path<PositionWrapper, helper::astar_compare>(
        PositionWrapper(start),
        {PositionWrapper(goal)}, search_through);


    if (!wrapped_path) {
        return {};
    }
    std::vector<TerrainOffset3> path;
    for (const auto position: wrapped_path.value()) {
        path.push_back(position.unique_position());
    }
    return path;
    
}

std::optional<std::vector<NodeGroupWrapper>>
Terrain::get_path_breadth_first(
    const NodeGroupWrapper start, const std::unordered_set<NodeGroupWrapper> goal
) const {
    std::unordered_set<const NodeGroup*> search_through = get_all_node_groups();

    std::unordered_set<NodeGroupWrapper> search_through_wrapped({});
    for (const auto& node : search_through) {
        search_through_wrapped.emplace(node);
    }
    return get_path<NodeGroupWrapper, helper::breadth_first_compare>(
        start, goal, search_through_wrapped
    );
}

std::optional<std::vector<TerrainOffset3>>
Terrain::get_path_breadth_first(
    const TerrainOffset3 start, const std::unordered_set<TerrainOffset3> goal_
) const {
    std::unordered_set<NodeGroupWrapper> goal_nodes({});
    bool no_goal = true;
    for (const TerrainOffset3 g : goal_) {
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
    NodeGroupWrapper end = node_path.value().back();
    TerrainOffset3 chunk_position = get_chunk_from_tile(
        end.unique_position()
    );

    std::unordered_set<PositionWrapper> goal({});
    for (const TerrainOffset3 g : goal_) {
        if (get_chunk_from_tile(g) == chunk_position) {
            if (end.contains(g)) {
                goal.insert(g);
            }
        }
    }
    if (goal.size() == 0)
        return {};
    std::unordered_set<PositionWrapper> search_through({});
    for (const NodeGroupWrapper& group : node_path.value()) {
        auto tiles = group.get_tiles();
        search_through.insert(tiles.begin(), tiles.end());
    }

    auto wrapped_path = get_path<PositionWrapper, helper::breadth_first_compare>(start, goal, search_through);

        if (!wrapped_path) {
        return {};
    }
    std::vector<TerrainOffset3> path;
    for (const auto position: wrapped_path.value()) {
        path.push_back(position.unique_position());
    }
    return path;
}

template <class T, bool compare(Node<const T>*, Node<const T>*)>
std::optional<std::vector<T>>
Terrain::get_path(
    const T start, const std::unordered_set<T> goal,
    const std::unordered_set<T> search_through
) const {
    std::priority_queue<Node<const T>*, std::vector<Node<const T>*>, decltype(compare)>
        openNodes(compare);

    std::unordered_map<TerrainOffset3, Node<const T>>
        nodes; // The nodes that can be walked through
    for (const T& t : search_through) {
        nodes[t.unique_position()] =
            Node<const T>(&t, get_H_cost(t.average_position(), (*goal.begin()).average_position()));
    }
    Node<const T> start_node = nodes[start.unique_position()];
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
                if (goal.find(*(n->get_tile())) != goal.end()) {
                    std::vector<T> path;
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
    for (auto& [pos, c] : chunks_) {
        std::unordered_set<const NodeGroup*> node_groups;
        c.add_nodes_to(node_groups);
        for (const NodeGroup* NG : node_groups) {
            for (const TerrainOffset3& t : NG->get_tiles()) {
                // Terrain owns the tile so this is fine

                Tile* non_const_tile = get_tile(t);
                set_tile_material(
                    non_const_tile, &get_materials().at(DEBUG_MATERIAL),
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

std::pair<TerrainOffset3, TerrainOffset3>
Terrain::get_start_end_test() const {
    std::pair<TerrainOffset3, TerrainOffset3> out;
    bool first = true;
    for (TerrainOffset x = 0; x < X_MAX; x++) {
        for (TerrainOffset y = 0; y < Y_MAX; y++) {
            for (TerrainOffset z = 0; z < Z_MAX; z++) {
                if (get_tile(x, y, z)->get_material_id() == DEBUG_MATERIAL
                    && get_tile(x, z, y)->get_color_id() == 4) {
                    if (first) {
                        out.first = {x, z, y};
                        first = false;
                    } else {
                        out.second = {x, z, y};
                        return out;
                    }
                }
            }
        }
    }
    return out;
}

} // namespace terrain
