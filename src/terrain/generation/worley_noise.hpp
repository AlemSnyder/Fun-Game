// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 */
/**
 * @file worley_noise.hpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines WorleyNoise class and AlternativeWorleyNoise class
 *
 * @ingroup Terrain Generation
 *
 */

#pragma once

#include "../../types.hpp"
#include "noise.hpp"

#include <cmath>
#include <cstdint>
#include <set>

namespace terrain {

namespace generation {

/**
 * @brief Worley Point is a struct that holds data about a point used for
 * Worley Noise generation.
 */
struct WorleyPoint {
    NoisePosition x_position;
    NoisePosition y_position;

    NoisePosition radius;

    bool positive;

    std::partial_ordering
    operator<=>(const WorleyPoint& other) const {
        return x_position <=> other.x_position;
    }
};

/**
 * @brief WorleyNoise is a noise generator that uses random points.
 *
 * @details This implementation uses one point per grid square and returns the
 * minimum distance to any point.
 */
class WorleyNoise : protected Noise {
 protected:
    // side length of area used to generate points
    NoisePosition tile_size_ = 1;
    // chance that any point is positive
    double positive_chance_ = 1;

 public:
    /**
     * @brief Create a new WorleyNoise object.
     *
     * @details Default constructor sets tile_size to 1;
     */
    inline WorleyNoise(NoisePosition tile_size = 1) : tile_size_(tile_size){};

    /**
     * @brief Get the noise value at given position.
     */
    [[nodiscard]] virtual double get_noise(NoisePosition x, NoisePosition y) const override;

 protected:
    [[nodiscard]] std::set<WorleyPoint>
    get_points_(NoiseTileIndex x_t, NoiseTileIndex y_t) const;

    [[nodiscard]] static double
    distance_(NoisePosition x, NoisePosition y, WorleyPoint);
};

/**
 * @brief AlternativeWorleyNoise is a noise generator that uses a cosine to
 * interpolate between positive and negative points
 * 
 * @details This implementation uses one point per grid square and returns a
 * sum of radial cosines centered at each point.
 */
class AlternativeWorleyNoise : public WorleyNoise {
 public:
     /**
     * @brief Create a new AlternativeWorleyNoise object.
     *
     * @details Default constructor sets tile_size to 1;
     */
    inline AlternativeWorleyNoise(NoisePosition tile_size = 1) :
        WorleyNoise(tile_size) {}

    [[nodiscard]] virtual double get_noise(NoisePosition x, NoisePosition y) const override;

 private:
    [[nodiscard]] double
    modified_cos_(NoisePosition distance, NoisePosition effective_radius) const;
};

} // namespace generation

} // namespace terrain