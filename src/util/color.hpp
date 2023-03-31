
#include "../constants.hpp"

#include <vector>

namespace color {

inline Color_float_t
convert_color_data(Color_int_t int_color) {
    uint32_t red = (int_color >> 24) & 0xFF;
    uint32_t green = (int_color >> 16) & 0xFF;
    uint32_t blue = (int_color >> 8) & 0xFF;
    uint32_t alpha = (int_color)&0xFF;
    // the last one >> 0 is A
    Color_float_t vector_color(
        {red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f}
    );
    return vector_color;
}

inline std::vector<Color_float_t>
convert_color_data(const std::vector<Color_int_t>& color_map) {
    std::vector<Color_float_t> float_colors;
    for (Color_int_t int_color : color_map) {
        float_colors.push_back(convert_color_data(int_color));
    }
    return float_colors;
}

} // namespace color
