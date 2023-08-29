
#pragma once

#include "noise.hpp"

#include <cmath>
#include <cstdint>
#include <set>

struct WorleyPoint {
    double x_position;
    double y_position;

    double radius;
};

class WorleyNoise {
 private:
    /* Should there be data? */
    double tile_size_ = 1;

 public:
    WorleyNoise(double tile_size);

    double get(double x, double y);

    std::set<WorleyPoint> get_points(int x_t, int y_t);

 private:
    double
    // returns a value between [0, 1)
    noise_(int i, int x, int y) {
        int32_t n = x * 53 + y * 59;
        n = (n << 13) ^ n;
        int32_t a = noise::PRIMES[i][0];
        int32_t b = noise::PRIMES[i][1];
        int32_t c = noise::PRIMES[i][2];
        int32_t t = (n * (n * n * a + b) + c) & INT32_MAX;
        return static_cast<double>(t) / INT32_MAX;
    }

    double
    distance(double x, double y, WorleyPoint);
};

WorleyNoise::WorleyNoise(double tile_size = 1) : tile_size_(tile_size) {}

[[nodiscard]] inline double
WorleyNoise::distance(double x, double y, WorleyPoint point) {
    return std::sqrt(
        std::pow(x - point.x_position, 2) + std::pow(y - point.y_position, 2)
    );
}

double
WorleyNoise::get(double x, double y) {
    int x_t = x / tile_size_;
    int y_t = y / tile_size_;

    auto worley_points = get_points(x_t, y_t);

    double value = tile_size_ * 2;
    for (const auto& point : worley_points) {
        if (auto d = distance(x, y, point) < value)
            value = d;
    }

    return value;
}

std::set<WorleyPoint>
get_points(int x_t, int y_t) {
    std::set<WorleyPoint> out;
    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            out.insert(WorleyPoint(
                {noise_(0, x_t + dx, y_t + dy), noise_(1, x_t + dx, y_t + dy), 1}
            ));
        }
    }
    return out;
}
