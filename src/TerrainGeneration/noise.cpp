#include <cmath>
#include "noise.hpp"
//#include "noise.hpp"

#define maxPrimeIndex 10

double NoiseGenerator::GetValueNoise(double x, double y) {
    double total = 0,
            frequency = pow(2, numOctaves),
            amplitude = 1;
    for (int i = 0; i < numOctaves; ++i) {
        frequency /= 2;
        amplitude *= persistence;
        total += InterpolatedNoise((primeIndex + i) % maxPrimeIndex,
            x / frequency, y / frequency) * amplitude;
    }
    return total / frequency;
}

double NoiseGenerator::noise(int i, int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int a = primes[i][0], b = primes[i][1], c = primes[i][2];
    int t = (n * (n * n * a + b) + c) & 0x7fffffff;
    return 1.0 - (double)(t)/1073741824.0;
}

double NoiseGenerator::SmoothedNoise(int i, int x, int y) {
    double corners = (noise(i, x-1, y-1) + noise(i, x+1, y-1) +
                        noise(i, x-1, y+1) + noise(i, x+1, y+1)) / 16,
            sides = (noise(i, x-1, y) + noise(i, x+1, y) + noise(i, x, y-1) +
                    noise(i, x, y+1)) / 8,
            center = noise(i, x, y) / 4;
    return corners + sides + center;
}

double NoiseGenerator::Interpolate(double a, double b, double x) {  // cosine interpolation
    double ft = x * 3.1415927,
            f = (1 - cos(ft)) * 0.5;
    return  a*(1-f) + b*f;
}

double NoiseGenerator::InterpolatedNoise(int i, double x, double y) {
    int integer_X = x;
    double fractional_X = x - integer_X;
    int integer_Y = y;
    double fractional_Y = y - integer_Y;

    double v1 = SmoothedNoise(i, integer_X, integer_Y),
            v2 = SmoothedNoise(i, integer_X + 1, integer_Y),
            v3 = SmoothedNoise(i, integer_X, integer_Y + 1),
            v4 = SmoothedNoise(i, integer_X + 1, integer_Y + 1),
            i1 = Interpolate(v1, v2, fractional_X),
            i2 = Interpolate(v3, v4, fractional_X);
    return Interpolate(i1, i2, fractional_Y);
}
