// Two-dimensional value noise_ based on Hugo Elias's description:
//   http://freespace.virgin.net/hugo.elias/models/m_perlin.htm

#ifndef __NOISE_HPP__
#define __NOISE_HPP__

#include <cmath>

#define maxPrimeIndex 10

class NoiseGenerator {
public:
    NoiseGenerator(int numOctaves, double persistence, int primeIndex){
        num_octaves_ = numOctaves;
        persistence_ = persistence;
        primeIndex_ = primeIndex % maxPrimeIndex;
    }

    double getValueNoise(double x, double y);

private:

    int num_octaves_ = 7;
    double persistence_ = 0.5;
    int primeIndex_ = 0;

    int primes[maxPrimeIndex][3] = {
    { 995615039, 600173719, 701464987 },
    { 831731269, 162318869, 136250887 },
    { 174329291, 946737083, 245679977 },
    { 362489573, 795918041, 350777237 },
    { 457025711, 880830799, 909678923 },
    { 787070341, 177340217, 593320781 },
    { 405493717, 291031019, 391950901 },
    { 458904767, 676625681, 424452397 },
    { 531736441, 939683957, 810651871 },
    { 997169939, 842027887, 423882827 }
    };

    double noise_(int i, int x, int y);
    double smoothed_noise_(int i, int x, int y);
    double interpolate_(double a, double b, double x); // cosine interpolation
    double interpolated_noise_(int i, double x, double y);
};

#endif
