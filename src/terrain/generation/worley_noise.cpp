#include "worley_noise.hpp"

namespace terrain {

namespace generation {

NoisePosition
WorleyNoise::distance_(NoisePosition x, NoisePosition y, WorleyPoint point) {
    return std::sqrt(
        std::pow(x - point.x_position, 2) + std::pow(y - point.y_position, 2)
    );
}

double
WorleyNoise::get_noise(NoisePosition x, NoisePosition y) const {
    NoiseTileIndex x_tile = x / tile_size_;
    NoiseTileIndex y_tile = y / tile_size_;

    auto worley_points = get_points_(x_tile, y_tile);

    NoisePosition value = tile_size_ * 2;
    for (const auto& point : worley_points) {
        NoisePosition d = distance_(x, y, point);
        if (d < value)
            value = d;
    }

    return value;
}

std::set<WorleyPoint>
WorleyNoise::get_points_(NoiseTileIndex xt, NoiseTileIndex yt) const {
    std::set<WorleyPoint> out;
    for (int dx = 0; dx < 2; dx++) {
        for (int dy = 0; dy < 2; dy++) {
            NoisePosition x_position = get_double(0, xt + dx, yt + dy);
            NoisePosition y_position = get_double(1, xt + dx, yt + dy);
            bool positive = (get_double(2, xt + dx, yt + dy) < positive_chance_);
            WorleyPoint point({x_position, y_position, 1, positive});
            out.insert(WorleyPoint(point));
        }
    }
    return out;
}

double
AlternativeWorleyNoise::get_noise(NoisePosition x, NoisePosition y) const {
    NoiseTileIndex xt = x / tile_size_;
    NoiseTileIndex yt = y / tile_size_;

    auto worley_points = get_points_(xt, yt);

    NoisePosition value = 0;
    for (const auto& point : worley_points) {
        NoisePosition d = distance_(x, y, point);
        // change is 1 when point.positive is T, and -1 when positive is F.
        uint8_t change = static_cast<uint8_t>(point.positive)* 2 - 1;
        value += modified_cos_(d, point.radius) * change;
    }

    return value;
}

double
AlternativeWorleyNoise::modified_cos_(
    NoisePosition distance_, NoisePosition effective_radius
) const {
    // only use first period of cos
    // 2 * effective radius is the maximum possible distance from any get point
    // to a worley point that is not generated.
    // that un used worley point must give zero in the function. This check
    // garnitures that.
    if (distance_ > 2 * effective_radius)
        return 0;
    // distance under 0 should not exist
    if (distance_ < 0)
        return 0;

    // return value between 0 and 2
    return std::cos(M_PI * distance_ / (2 * effective_radius))  + 1;
}

} // namespace generation

} // namespace terrain
