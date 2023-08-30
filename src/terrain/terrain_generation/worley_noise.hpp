
#pragma once

#include "noise.hpp"

#include <cmath>
#include <cstdint>
#include <set>

namespace terrain {

namespace terrain_generation {

struct WorleyPoint {
    double x_position;
    double y_position;

    double radius;

    bool positive;
};

class WorleyNoise : public Noise {
 protected:
    /* Should there be data? */
    double tile_size_ = 1;
    double positive_chance = 1;

 public:
    WorleyNoise(double tile_size);

    double get(double x, double y) const;

    std::set<WorleyPoint> get_points(int x_t, int y_t) const;

    static double distance(double x, double y, WorleyPoint);
};

class AlturnativeWorleyNoise : public WorleyNoise {
 public:
    inline AlturnativeWorleyNoise(double tile_size) : WorleyNoise(tile_size) {}

    double get(double x, double y) const;

 private:
    double modified_cos(double distance, double effective_radius) const;
};

} // namespace terrain_generation

} // namespace terrain