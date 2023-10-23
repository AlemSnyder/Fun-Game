// -*- lsst-c++ -*-
// Two-dimensional value noise based on Hugo Elias's description.
// I did not write this code, but the link to the original no longer exists.

#include "noise.hpp"

#include <cmath>
#include <numbers>

namespace terrain {

namespace generation {

double
generation::FractalNoise::get_noise(NoisePosition x, NoisePosition y) const {
    double total = 0, frequency = pow(2, num_octaves_), amplitude = 1;
    for (int i = 0; i < num_octaves_; ++i) {
        frequency /= 2;
        amplitude *= persistence_;
        total += interpolated_noise_(
                     (primeIndex_ + i) % NUM_PRIMES, x / frequency, y / frequency
                 )
                 * amplitude;
    }
    return total / frequency;
}

// returns a value between [0, 1)
double
Noise::get_double(size_t i, NoiseTileIndex x, NoiseTileIndex y) {
    int32_t n = x * 53 + y * 59;
    n = (n << 13) ^ n;
    int32_t a = PRIMES[i][0];
    int32_t b = PRIMES[i][1];
    int32_t c = PRIMES[i][2];
    int32_t t = (n * (n * n * a + b) + c) & INT32_MAX;
    return static_cast<double>(t) / INT32_MAX;
}

double
generation::FractalNoise::smoothed_noise_(size_t i, NoiseTileIndex x, NoiseTileIndex y)
    const {
    // clang-format off
    double corners = (get_double(i, x - 1, y - 1)
                    + get_double(i, x + 1, y - 1)
                    + get_double(i, x - 1, y + 1)
                    + get_double(i, x + 1, y + 1)
                    ) / 16;
    double sides = (get_double(i, x - 1, y)
                  + get_double(i, x + 1, y)
                  + get_double(i, x, y - 1)
                  + get_double(i, x, y + 1)
                  ) / 8;
    // clang-format on
    double center = get_double(i, x, y) / 4;
    return corners + sides + center;
}

double
generation::FractalNoise::interpolate_(
    NoisePosition a, NoisePosition b, NoisePosition x
) const { // cosine interpolation
    double ft = x * std::numbers::pi;
    double f = (1 - cos(ft)) / 2.0;
    return a * (1 - f) + b * f;
}

double
generation::FractalNoise::interpolated_noise_(
    size_t i, NoisePosition x, NoisePosition y
) const {
    int integer_X = x;
    double fractional_X = x - integer_X;
    int integer_Y = y;
    double fractional_Y = y - integer_Y;

    double v1 = smoothed_noise_(i, integer_X, integer_Y),
           v2 = smoothed_noise_(i, integer_X + 1, integer_Y),
           v3 = smoothed_noise_(i, integer_X, integer_Y + 1),
           v4 = smoothed_noise_(i, integer_X + 1, integer_Y + 1),
           i1 = interpolate_(v1, v2, fractional_X),
           i2 = interpolate_(v3, v4, fractional_X);
    return interpolate_(i1, i2, fractional_Y);
}

} // namespace generation

} // namespace terrain
