
#include "material.hpp"

#include "../entity/mesh.hpp"
#include "../logging.hpp"

#include <GL/glew.h>

namespace terrain {

// color id to color for all materials
std::vector<ColorInt> TerrainColorMapping::color_ids_map;
// 8 bit color to color id
std::unordered_map<ColorInt, uint16_t> TerrainColorMapping::colors_inverse_map;
// id of the color texture sent to opengl
GLuint_p TerrainColorMapping::color_texture_;

void
TerrainColorMapping::assign_color_mapping(
    const std::map<uint8_t, const Material>& materials
) {
    color_ids_map.clear();
    colors_inverse_map.clear();

    // TODO @AlemSnyder
    // GetIntegerv(GL_MAX_TEXTURE_SIZE, *mas_texture_size)
    // The above size is probably a better upper bound as it comes from the
    // texture size.
    // then use:
    // max_size = min((1U << 15), mas_texture_size);
    // but if I fix this then I should probably switch from a 1D texture
    // to a 2D texture, and that seems like a lot of work.

    if (materials.size() > (1U << 15)) {
        LOG_WARNING(
            logging::terrain_logger,
            "Too many colors in material file ({}). Only using first {}",
            materials.size(), (1U << 15)
        );
    }

    // empty should always have index 0;
    color_ids_map.push_back(0);
    colors_inverse_map[0] = 0;
    for (auto const& [id, material] : materials) {
        for (auto color_data : material.color) {
            uint32_t color = color_data.second;
            if (std::find(color_ids_map.begin(), color_ids_map.end(), color)
                == color_ids_map.end()) {
                // voxel_colors[i] is not in colors
                // we should add it to colors
                // the index is the length of colors because it will be appended
                uint16_t j = color_ids_map.size();
                // add the color
                color_ids_map.push_back(color);
                // add to inverse colors
                colors_inverse_map[color] = j;
                if (color_ids_map.size() > (1U << 15) - 1)
                    return;
            }
        }
    }
}

GLuint_p
TerrainColorMapping::assign_color_texture() {
    glDeleteTextures(1, &color_texture_);

    // Generate a texture
    std::vector<ColorFloat> float_colors = color::convert_color_data(color_ids_map);

    // Generate a texture
    glGenTextures(1, &color_texture_);
    glBindTexture(GL_TEXTURE_1D, color_texture_);
    // set the texture wrapping/filtering options (on the currently bound texture
    // object)
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    glTexImage1D(
        GL_TEXTURE_1D, 0, GL_RGBA32F, float_colors.size(), 0, GL_RGBA, GL_FLOAT,
        float_colors.data()
    );
    glGenerateMipmap(GL_TEXTURE_1D);

    return color_texture_;
}

} // namespace terrain
