
#pragma once

#include "../../types.hpp"
#include "noise.hpp"

#include <cmath>
#include <cstdint>
#include <set>

namespace terrain {

namespace generation {

struct WorleyPoint {
    NoisePosition x_position;
    NoisePosition y_position;

    NoisePosition radius;

    bool positive;

    std::partial_ordering
    operator<=>(const WorleyPoint& other) const {
        return x_position <=> other.x_position;
    }
};

class WorleyNoise : public Noise {
 protected:
    /* Should there be data? */
    NoisePosition tile_size_ = 1;
    double positive_chance_ = 1;

 public:
    inline WorleyNoise(NoisePosition tile_size = 1) : tile_size_(tile_size){};

    [[nodiscard]] double get(NoisePosition x, NoisePosition y) const;

 protected:
    [[nodiscard]] std::set<WorleyPoint>
    get_points_(NoiseTileIndex x_t, NoiseTileIndex y_t) const;

    [[nodiscard]] static double
    distance_(NoisePosition x, NoisePosition y, WorleyPoint);
};

class AlternativeWorleyNoise : public WorleyNoise {
 public:
    inline AlternativeWorleyNoise(NoisePosition tile_size = 1) :
        WorleyNoise(tile_size) {}

    [[nodiscard]] double get(NoisePosition x, NoisePosition y) const;

 private:
    [[nodiscard]] double
    modified_cos_(NoisePosition distance, NoisePosition effective_radius) const;
};

} // namespace generation

} // namespace terrain