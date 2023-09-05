// -*- lsst-c++ -*-
// Two-dimensional value noise based on Hugo Elias's description.
// I did not write this code, but the link to the original no longer exists.

/**
 * @file noise.hpp
 *
 * @brief Defines NoiseGenerator class.
 *
 * @ingroup Terrain
 *
 */

#pragma once

#include "../../types.hpp"

#include <cmath>
#include <cstdint>

namespace terrain {

namespace generation {

class Noise {
 protected:
    // The length of NoiseGenerator::primes
    static constexpr uint16_t NUM_PRIMES = 10;

    static constexpr int32_t PRIMES[NUM_PRIMES][3] = {
        {995615039, 600173719, 701464987},
        {831731269, 162318869, 136250887},
        {174329291, 946737083, 245679977},
        {362489573, 795918041, 350777237},
        {457025711, 880830799, 909678923},
        {787070341, 177340217, 593320781},
        {405493717, 291031019, 391950901},
        {458904767, 676625681, 424452397},
        {531736441, 939683957, 810651871},
        {997169939, 842027887, 423882827}
    };

 public:
    /**
     * @brief Returns a sem random double between [0, 1] depending on the seed
     * values given
     */
    static double get_double(size_t i, NoiseTileIndex x, NoiseTileIndex y);

}; // namespace noise

/**
 * @brief Generates two dimensional Perlin noise.
 *
 * @details NoiseGenerator generates two dimensional Perlin noise with cosine
 * interpolation, and geometric persistance. The noise consists of different
 * layers. The first layer is between -1, and 1. Subsequent have twice the
 * frequency, and amplitude of persistance times the previous amplitude.
 */
class NoiseGenerator : protected Noise {
    int num_octaves_ = 7;
    double persistence_ = 0.5;
    int primeIndex_ = 0;

 public:
    /**
     * @brief Construct a new Noise Generator object
     *
     * @param numOctaves the number of noise layers that are generated
     * @param persistence the strength of subsequent noise; between 0, and 1
     * @param primeIndex int for randomization (a worse version of a seed)
     */
    NoiseGenerator(int numOctaves, double persistence, int primeIndex) {
        num_octaves_ = numOctaves;
        persistence_ = persistence;
        primeIndex_ = primeIndex % NUM_PRIMES;
    }

    /**
     * @brief Get the value of the noise.
     *
     * @param x postion in x direction
     * @param y postion in y direction
     * @return double the value of the noise
     */
    double getValueNoise(NoisePosition x, NoisePosition y) const;

 private:
    double smoothed_noise_(size_t i, NoiseTileIndex x, NoiseTileIndex y) const;
    double interpolate_(NoisePosition a, NoisePosition b, NoisePosition x)
        const; // cosine interpolation
    double interpolated_noise_(size_t i, NoisePosition x, NoisePosition y) const;
};

} // namespace generation

} // namespace terrain
