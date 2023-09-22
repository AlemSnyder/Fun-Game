#include "../../types.hpp"

#include <mutex>
#include <random>

#pragma once

namespace terrain {

namespace generation {

class MapTile {
 private:
    MacroDim x_; // x position
    MacroDim y_; // y position

    MapTile_t tile_type_; // map tile type
    // TODO add mutex
    // This requires >= 64 x 64 map macro tiles. This would correspond to
    // std::mutex mut_; // mutex for locking to avoid collisions during generation
    std::default_random_engine rand_engine_; // random number generator

 public:
    MapTile(MapTile_t tile_type, MacroDim x = 0, MacroDim y = 0);

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
