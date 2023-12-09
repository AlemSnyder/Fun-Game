#pragma once

#include "../../types.hpp"
#include "../../util/png_image.hpp"
#include "noise.hpp"

#include <random>
#include <vector>

namespace terrain {

namespace generation {

// a number chosen at random by the developer
constexpr size_t RANDOM_NUMBER = 7;

class MapTile {
 private:
    MacroDim x_; // x position
    MacroDim y_; // y position

    MapTile_t tile_type_; // map tile type
    // TODO add mutex
    // This requires >= 64 x 64 map macro tiles. This would correspond to
    // 2^30 tiles. This is 4+ gigabytes.
    // std::mutex mut_; // mutex for locking to avoid collisions during generation
    std::default_random_engine rand_engine_; // random number generator

 public:
    MapTile(MapTile_t tile_type, size_t seed, MacroDim x = 0, MacroDim y = 0) :
        x_(x), y_(y), tile_type_(tile_type),
        rand_engine_(
            Noise::get_double((seed ^ tile_type) % RANDOM_NUMBER, x, y) * INT32_MAX
        ){};

    [[nodiscard]] inline MacroDim
    get_x() const {
        return x_;
    }

    [[nodiscard]] inline MacroDim
    get_y() const {
        return y_;
    }

    [[nodiscard]] inline MapTile_t
    get_tile_type() const {
        return tile_type_;
    }

    [[nodiscard]] inline std::default_random_engine&
    get_rand_engine() {
        return rand_engine_;
    }
};

class TerrainMacroMap {
 private:
    MacroDim width_;
    MacroDim height_;

    std::vector<MapTile> terrain_map_;

 public:
    inline TerrainMacroMap() : width_(0), height_(0){};

    inline TerrainMacroMap(
        std::vector<MapTile> terrain_map, size_t width, size_t height
    ) :
        width_(width),
        height_(height), terrain_map_(terrain_map) {
        assert(terrain_map_.size() == width_ * height_);
    };

    const MapTile&
    get_tile(size_t i, size_t j) const {
        assert(i < width_ && j < height_);
        return terrain_map_[height_ * j + i];
    }

    // MapTile_t
    png_byte
    get_color(size_t i, size_t j) const {
        return get_tile(i, j).get_tile_type();
    }

    const auto
    begin() const {
        return terrain_map_.begin();
    }

    const auto
    end() const {
        return terrain_map_.end();
    }

    const std::vector<MapTile>&
    data() const {
        return terrain_map_;
    }

    MacroDim
    get_width() const {
        return width_;
    }

    MacroDim
    get_height() const {
        return height_;
    }
};

} // namespace generation

} // namespace terrain
