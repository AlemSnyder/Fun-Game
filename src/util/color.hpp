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
 * @file color.hpp
 *
 * @brief Defines Color type conversions.
 *
 * @ingroup Util
 *
 */

#pragma once

#include "bits.hpp"
#include "types.hpp"

#include <vector>

namespace color {

static_assert(sizeof(color_t) == sizeof(ColorInt));

union color_converter {
    color_t as_vec;
    ColorInt as_uint;

    color_converter() : as_vec() {}
};

inline auto
color_to_uint32(color_t color) {
    color_converter c;
    c.as_vec = color;
    return bits::swap(c.as_uint);
}

inline auto
uint32_to_color(ColorInt num) {
    color_converter c;
    c.as_uint = bits::swap(num);
    return c.as_vec;
}

// the amount to bitshift color int to gets each channel
constexpr auto bit_shift = sizeof(ColorInt) * 2;
// 1 for the size of the color channel
constexpr ColorInt bits_place = (1 << bit_shift) - 1;

inline ColorFloat
convert_color_data(ColorInt int_color) {
    glm::vec4 color = uint32_to_color(int_color);
    return color / 255.0f;
}

inline std::vector<ColorFloat>
convert_color_data(const std::vector<ColorInt>& color_map) {
    std::vector<ColorFloat> float_colors;
    for (ColorInt int_color : color_map) {
        float_colors.push_back(convert_color_data(int_color));
    }
    return float_colors;
}

constexpr ColorFloat black({0, 0, 0, 1});

} // namespace color
