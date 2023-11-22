#include "worley_noise.hpp"

#include <numbers>

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
WorleyNoise::get_points_(NoiseTileIndex xt, NoiseTileIndex yt, NoiseTileIndex range)
    const {
    assert(range % 2 == 0 && "range must be even");
    std::set<WorleyPoint> out;
    for (int dx = 0; dx < range; dx++) {
        for (int dy = 0; dy < range; dy++) {
            // compute index of worley tile
            NoiseTileIndex x_index = xt + dx - range / 2;
            NoiseTileIndex y_index = yt + dy - range / 2;

            // determine where the worley point is in the tile
            NoisePosition x_position =
                (get_double(0, x_index, y_index) + x_index - .5) * tile_size_;
            NoisePosition y_position =
                (get_double(1, x_index, y_index) + y_index - .5) * tile_size_;
            bool positive =
                (get_double(2, x_index, y_index) < positive_chance_);
            WorleyPoint point({x_position, y_position, tile_size_ / 2, positive});
            out.insert(WorleyPoint(point));
        }
    }
    return out;
}

double
AlternativeWorleyNoise::get_noise(NoisePosition x, NoisePosition y) const {
    NoiseTileIndex xt = x / tile_size_;
    NoiseTileIndex yt = y / tile_size_;

    // the range is determined by the ratio between 
    // point_radius_, and tile_size_
    NoiseTileIndex range = (point_radius_/tile_size_ + 2) * 2;

    auto worley_points = get_points_(xt, yt, range);

    NoisePosition value = 0;
    for (const auto& point : worley_points) {
        NoisePosition d = distance_(x, y, point);
        // change is 1 when point.positive is T, and -1 when positive is F.
        double change = static_cast<double>(point.positive) * 2 - 1;
        value += modified_cos_(d, point_radius_) * change;
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
    return std::cos(std::numbers::pi * distance_ / (2 * effective_radius)) + 1;
}

} // namespace generation

} // namespace terrain
