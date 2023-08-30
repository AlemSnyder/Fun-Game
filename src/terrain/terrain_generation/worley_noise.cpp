#include "worley_noise.hpp"

namespace terrain {

namespace terrain_generation {

WorleyNoise::WorleyNoise(double tile_size = 1) : tile_size_(tile_size) {}

[[nodiscard]] inline double
WorleyNoise::distance(double x, double y, WorleyPoint point) {
    return std::sqrt(
        std::pow(x - point.x_position, 2) + std::pow(y - point.y_position, 2)
    );
}

double
WorleyNoise::get(double x, double y) const {
    int x_t = x / tile_size_;
    int y_t = y / tile_size_;

    auto worley_points = get_points(x_t, y_t);

    double value = tile_size_ * 2;
    for (const auto& point : worley_points) {
        double d = distance(x, y, point);
        if (d < value)
            value = d;
    }

    return value;
}

std::set<WorleyPoint>
WorleyNoise::get_points(int x_t, int y_t) const {
    std::set<WorleyPoint> out;
    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            double x_position = get_double(0, x_t + dx, y_t + dy);
            double y_position = get_double(1, x_t + dx, y_t + dy);
            bool positive = get_double(2, x_t + dx, y_t + dy) < positive_chance;
            WorleyPoint point({x_position, y_position, 1});
            out.insert(WorleyPoint(point));
        }
    }
    return out;
}

double
AlturnativeWorleyNoise::get(double x, double y) const {
    int x_t = x / tile_size_;
    int y_t = y / tile_size_;

    auto worley_points = get_points(x_t, y_t);

    double value = 0;
    for (const auto& point : worley_points) {
        double d = distance(x, y, point);
        if (point.positive)
            value += modified_cos(d, point.radius);
        else
            value -= modified_cos(d, point.radius);
    }

    return value;
}

double
AlturnativeWorleyNoise::modified_cos(double distance, double effective_radius) const {
    if (distance > 2 * effective_radius) {
        return 0;
    }
    if (distance < 0) {
        return 0;
    }

    return std::cos(distance / effective_radius);
}

} // namespace terrain_generation

} // namespace terrain
