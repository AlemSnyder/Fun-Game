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

/*
int main(int argc, char** args) {
    int numX = 512,
    numY = 512,
    numOctaves = 7;
    double persistence = 0.5;
    int primeIndex = 0;
    if (argc >= 3) {
        numX = atoi(args[1]);
        numY = atoi(args[2]);
    }
    if (argc >= 4) {
        numOctaves = atoi(args[3]);
    }
    if (argc >= 5) {
        persistence = atof(args[4]);
    }
    if (argc >= 6) {
        primeIndex = atoi(args[5]) % maxPrimeIndex;
    }

    Noise noise_generator = Noise(numOctaves, persistence, primeIndex);
    fprintf(stderr, "numX: %d, numY: %d, numOctaves: %d, persistence: %.5f, ",
        numX, numY, numOctaves, persistence);
    fprintf(stderr, "primeIndex: %d\n", primeIndex);
    printf("var rawNoise = [\n");
    for (int y = 0; y < numY; ++y) {
        for (int x = 0; x < numX; ++x) {
        double noise = noise_generator.ValueNoise_2D(x, y);
        if (x == 0) {
            printf("  [");
        }
        printf("%.5f", noise);
        if (x == numX-1) {
            printf("]");
            if (y == numY-1) {
            printf("\n];\n");
            } else {
            printf(",\n");
            }
        } else {
            printf(", ");
        }
        }
    }
    return 0;
}
*/