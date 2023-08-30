#include "worley_noise.hpp"


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
        if ((auto d = distance(x, y, point)) < value)
            value = d;
    }

    return value;
}

std::set<WorleyPoint>
get_points(int x_t, int y_t) {
    std::set<WorleyPoint> out;
    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            double x_position = get_double(0, x_t + dx, y_t + dy);
            double y_position = get_double(1, x_t + dx, y_t + dy);
            bool positive = get_double(2, x_t + dx, y_t + dy) < positive_chance;
            WorleyPoint point({x_position, y_position, 1});
            out.insert(WorleyPoint(
                point
            ));
        }
    }
    return out;
}

double
AlturnativeWorleyNoise::get(double x, double y) {
    int x_t = x / tile_size_;
    int y_t = y / tile_size_;

    auto worley_points = get_points(x_t, y_t);

    double value = 0;
    for (const auto& point : worley_points) {
        double d = distance(x, y, point);
        if (point.positive)
            value += f(d, point.radius);
        else
            value -= f(d, point.radius);
    }

    return value;
}

