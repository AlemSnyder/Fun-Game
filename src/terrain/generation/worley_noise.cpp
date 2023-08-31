#include "worley_noise.hpp"

namespace terrain {

namespace generation {

WorleyNoise::WorleyNoise(NoisePosition tile_size) : tile_size_(tile_size) {}

[[nodiscard]] NoisePosition
WorleyNoise::distance(NoisePosition x, NoisePosition y, WorleyPoint point) {
    return std::sqrt(
        std::pow(x - point.x_position, 2) + std::pow(y - point.y_position, 2)
    );
}

double
WorleyNoise::get(NoisePosition x, NoisePosition y) const {
    NoiseTileIndex x_tile = x / tile_size_;
    NoiseTileIndex y_tile = y / tile_size_;

    auto worley_points = get_points(x_tile, y_tile);

    NoisePosition value = tile_size_ * 2;
    for (const auto& point : worley_points) {
        NoisePosition d = distance(x, y, point);
        if (d < value)
            value = d;
    }

    return value;
}

std::set<WorleyPoint>
WorleyNoise::get_points(NoiseTileIndex x_t, NoiseTileIndex y_t) const {
    std::set<WorleyPoint> out;
    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            NoisePosition x_position = get_double(0, x_t + dx, y_t + dy);
            NoisePosition y_position = get_double(1, x_t + dx, y_t + dy);
            bool positive = (get_double(2, x_t + dx, y_t + dy) < positive_chance_);
            WorleyPoint point({x_position, y_position, 1, positive});
            out.insert(WorleyPoint(point));
        }
    }
    return out;
}

double
AlternativeWorleyNoise::get(NoisePosition x, NoisePosition y) const {
    NoiseTileIndex x_t = x / tile_size_;
    NoiseTileIndex y_t = y / tile_size_;

    auto worley_points = get_points(x_t, y_t);

    NoisePosition value = 0;
    for (const auto& point : worley_points) {
        NoisePosition d = distance(x, y, point);
        if (point.positive)
            value += modified_cos_(d, point.radius);
        else
            value -= modified_cos_(d, point.radius);
    }

    return value;
}

double
AlternativeWorleyNoise::modified_cos_(
    NoisePosition distance, NoisePosition effective_radius
) const {
    if (distance > 2 * effective_radius) {
        return 0;
    }
    if (distance < 0) {
        return 0;
    }

    return std::cos(distance / effective_radius);
}

} // namespace generation

} // namespace terrain
