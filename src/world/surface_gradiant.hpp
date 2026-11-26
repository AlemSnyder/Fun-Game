// -*- lsst-c++ -*-
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

/**
 * @file biome.hpp
 *
 * @brief Defines Biome class.
 *
 * @ingroup Terrain generation
 *
 */

#include "terrain/material.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"
#include <glaze/glaze.hpp>
#pragma clang diagnostic pop
#include <optional>

#include <vector>

#pragma once

namespace terrain {

/*struct grass_data_t {
    std::vector<int> levels;
    int midpoint;
};*/

namespace generation {

/**
 * @brief Contains grass colors and pattern for a biome.
 */
class GrassData {
 private:
    // vector that determines grass color from edge distance
    std::vector<ColorId> gradient_colors_;
    // gradient index of grass not by an edge
    uint8_t gradient_midpoint_location_;

    friend struct glz::meta<generation::GrassData>;

 public:
    /**
     * @brief Get the grass gradient length
     *
     * @return length of grass gradient
     */
    [[nodiscard]] inline uint8_t
    get_grass_grad_length() const noexcept {
        return gradient_colors_.size();
    }

    /**
     * @brief Get index where grass gradient splits
     *
     * @return grass mid point
     */
    [[nodiscard]] inline uint8_t
    get_grass_mid() const {
        return gradient_midpoint_location_;
    }

    /**
     * @brief Get grass gradient
     *
     * @return vector of grass color ids
     */
    [[nodiscard]] inline const std::vector<ColorId>&
    get_grass_colors() const {
        return gradient_colors_;
    }
};
}
}

template <>
struct glz::meta<terrain::generation::GrassData> {
    using T = terrain::generation::GrassData;
        static constexpr auto value = glz::object(
        "color_ids", &T::gradient_colors_,
        "midpoint_location", &T::gradient_midpoint_location_
    );
};
