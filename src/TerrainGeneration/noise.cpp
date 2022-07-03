#include <cmath>
#include "noise.hpp"
//#include "noise_.hpp"

#define maxPrimeIndex 10

double NoiseGenerator::getValueNoise(double x, double y) {
    double total = 0,
            frequency = pow(2, num_octaves_),
            amplitude = 1;
    for (int i = 0; i < num_octaves_; ++i) {
        frequency /= 2;
        amplitude *= persistence_;
        total += interpolated_noise_((primeIndex_ + i) % maxPrimeIndex,
            x / frequency, y / frequency) * amplitude;
    }
    return total / frequency;
}

double NoiseGenerator::noise_(int i, int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int a = primes[i][0], b = primes[i][1], c = primes[i][2];
    int t = (n * (n * n * a + b) + c) & 0x7fffffff;
    return 1.0 - (double)(t)/1073741824.0;
}

double NoiseGenerator::smoothed_noise_(int i, int x, int y) {
    double corners = (noise_(i, x-1, y-1) + noise_(i, x+1, y-1) +
                        noise_(i, x-1, y+1) + noise_(i, x+1, y+1)) / 16,
            sides = (noise_(i, x-1, y) + noise_(i, x+1, y) + noise_(i, x, y-1) +
                    noise_(i, x, y+1)) / 8,
            center = noise_(i, x, y) / 4;
    return corners + sides + center;
}

double NoiseGenerator::interpolate_(double a, double b, double x) {  // cosine interpolation
    double ft = x * 3.1415927,
            f = (1 - cos(ft)) * 0.5;
    return  a*(1-f) + b*f;
}

double NoiseGenerator::interpolated_noise_(int i, double x, double y) {
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
