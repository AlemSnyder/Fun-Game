
#include "../types.hpp"

#include <vector>

namespace color {

// the amount to bitshift color int to gets each channel
constexpr auto bit_shift = sizeof(ColorInt) * 2;
// 1 for the size of the color channel
constexpr ColorInt bits_place = (1 << bit_shift) - 1;

inline ColorFloat
convert_color_data(ColorInt int_color) {
    uint32_t red = (int_color >> bit_shift * 3) & bits_place;
    uint32_t green = (int_color >> bit_shift * 2) & bits_place;
    uint32_t blue = (int_color >> bit_shift) & bits_place;
    uint32_t alpha = int_color & bits_place;
    // the last one >> 0 is A
    ColorFloat vector_color(
        {red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f}
    );
    return vector_color;
}

inline std::vector<ColorFloat>
convert_color_data(const std::vector<ColorInt>& color_map) {
    std::vector<ColorFloat> float_colors;
    for (ColorInt int_color : color_map) {
        float_colors.push_back(convert_color_data(int_color));
    }
    return float_colors;
}

} // namespace color
