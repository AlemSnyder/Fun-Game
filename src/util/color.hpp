
#include "../types.hpp"

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
    return c.as_uint;
}

inline auto
uint32_to_color(ColorInt num) {
    color_converter c;
    c.as_uint = num;
    return c.as_vec;
}

// the amount to bitshift color int to gets each channel
constexpr auto bit_shift = sizeof(ColorInt) * 2;
// 1 for the size of the color channel
constexpr ColorInt bits_place = (1 << bit_shift) - 1;

inline ColorFloat
convert_color_data(ColorInt int_color) {
    glm::vec4 color = glm::vec4(uint32_to_color(int_color)) / 255.0f;
    return {color.a, color.b, color.g, color.r};
}

inline std::vector<ColorFloat>
convert_color_data(const std::vector<ColorInt>& color_map) {
    std::vector<ColorFloat> float_colors;
    for (ColorInt int_color : color_map) {
        glm::vec4 color = glm::vec4(uint32_to_color(int_color)) / 255.0f;
        float_colors.push_back({color.a, color.b, color.g, color.r});
    }
    return float_colors;
}

} // namespace color
