#include "noise.hpp"

#include <cmath>

#define maxPrimeIndex 10

namespace terrain {

namespace generation {

double
generation::NoiseGenerator::getValueNoise(double x, double y) const {
    double total = 0, frequency = pow(2, num_octaves_), amplitude = 1;
    for (int i = 0; i < num_octaves_; ++i) {
        frequency /= 2;
        amplitude *= persistence_;
        total += interpolated_noise_(
                     (primeIndex_ + i) % maxPrimeIndex, x / frequency, y / frequency
                 )
                 * amplitude;
    }
    return total / frequency;
}

// returns a value between [0, 1)
double
Noise::get_double(int i, int x, int y) const {
    int32_t n = x * 53 + y * 59;
    n = (n << 13) ^ n;
    int32_t a = PRIMES[i][0];
    int32_t b = PRIMES[i][1];
    int32_t c = PRIMES[i][2];
    int32_t t = (n * (n * n * a + b) + c) & INT32_MAX;
    return static_cast<double>(t) / INT32_MAX;
}

double
generation::NoiseGenerator::smoothed_noise_(int i, int x, int y) const {
    double corners = (get_double(i, x - 1, y - 1) + get_double(i, x + 1, y - 1)
                      + get_double(i, x - 1, y + 1) + get_double(i, x + 1, y + 1))
                     / 16,
           sides = (get_double(i, x - 1, y) + get_double(i, x + 1, y)
                    + get_double(i, x, y - 1) + get_double(i, x, y + 1))
                   / 8,
           center = get_double(i, x, y) / 4;
    return corners + sides + center;
}

double
generation::NoiseGenerator::interpolate_(double a, double b, double x)
    const { // cosine interpolation
    double ft = x * 3.1415927, f = (1 - cos(ft)) * 0.5;
    return a * (1 - f) + b * f;
}

double
generation::NoiseGenerator::interpolated_noise_(int i, double x, double y)
    const {
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
