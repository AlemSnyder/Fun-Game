#pragma once

#include "../../types.hpp"
#include "noise.hpp"

#include <mutex>
#include <random>

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

} // namespace generation

} // namespace terrain
