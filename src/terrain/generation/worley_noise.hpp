
#pragma once

#include "noise.hpp"

#include "../../types.hpp"

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
};

class WorleyNoise : public Noise {
 protected:
    /* Should there be data? */
    NoisePosition tile_size_ = 1;
    NoisePosition positive_chance_ = 1;

 public:
    WorleyNoise(NoisePosition tile_size = 1);

    NoisePosition get(NoisePosition x, NoisePosition y) const;

    std::set<WorleyPoint> get_points(int x_t, int y_t) const;

 protected:
    static NoisePosition distance(NoisePosition x, NoisePosition y, WorleyPoint);
};

class AlternativeWorleyNoise : public WorleyNoise {
 public:
    inline AlternativeWorleyNoise(NoisePosition tile_size = 1) : WorleyNoise(tile_size) {}

    NoisePosition get(NoisePosition x, NoisePosition y) const;

 private:
    NoisePosition modified_cos_(NoisePosition distance, NoisePosition effective_radius) const;
};

} // namespace generation

} // namespace terrain