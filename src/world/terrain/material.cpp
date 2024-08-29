#include "material.hpp"

#include "gui/render/gpu_data/texture.hpp"
#include "logging.hpp"

namespace terrain {

// color id to color for all materials
std::vector<ColorInt> TerrainColorMapping::color_ids_map;
// 8 bit color to color id
std::unordered_map<ColorInt, MatColorId> TerrainColorMapping::colors_inverse_map;

void
TerrainColorMapping::assign_color_mapping(
    const std::map<MaterialId, const material_t>& materials
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
            ColorInt color = color_data.hex_color;
            if (std::find(color_ids_map.begin(), color_ids_map.end(), color)
                == color_ids_map.end()) {
                // voxel_colors[i] is not in colors
                // we should add it to colors
                // the index is the length of colors because it will be appended
                MatColorId j = color_ids_map.size();
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

MaterialGroup::MaterialGroup(const std::vector<generation::material_designation_t>& data
) {
    // want to return a group that represents the given data
    // There will be elements with no requirements on the color
    //    std::set<MaterialId> materials;
    // amd some with requirements on the color
    //    std::map<MaterialId, std::set<ColorId>> materials_w_color;

    for (const generation::material_designation_t& material_data : data) {
        // read the material id from the data
        // MaterialId mat_id = material_data.material;

        const auto& material = material_data.material;
        const auto& color = material_data.color;

        std::unordered_set<ColorId> colors;
        std::unordered_set<MaterialId> materials;

        {
            switch (material.index()) {
                case 0:
                    { // bool

                        bool all_materials = std::get<bool>(material);
                        if (all_materials) {
                            set_all();
                            return;
                        } else {
                            // wut? why would you do this?
                        }
                        break;
                    }
                case 1:
                    { // MaterialId
                        materials.insert(std::get<MaterialId>(material));
                        break;
                    }
                case 2:
                    { // vector

                        const auto& materials_to_add =

                            std::get<std::vector<MaterialId>>(material);

                        materials.insert(
                            materials_to_add.begin(), materials_to_add.end()
                        );

                        break;
                    }

                default:
                    break;
            }
        }

        {
            switch (color.index()) {
                case 0: // bool
                    {
                        bool all_colors = std::get<bool>(color);
                        if (all_colors)
                            insert_(materials);

                        continue;
                    }
                case 1: // ColorId
                    {
                        colors.insert(std::get<ColorId>(color));
                        break;
                    }
                case 2:
                    { // vector
                        const auto& colors_to_add =
                            std::get<std::vector<ColorId>>(color);

                        colors.insert(colors_to_add.begin(), colors_to_add.end());

                        break;
                    }

                default:
                    break;
            }
        }
        insert_(materials, colors);
    }
}

void
MaterialGroup::insert_(std::unordered_set<MaterialId> material_id) {
    materials_no_color_requirement_.insert(material_id.begin(), material_id.end());
}

void
MaterialGroup::insert_(
    std::unordered_set<MaterialId> material_id, std::unordered_set<ColorId> color_ids
) {
    for (MaterialId id : material_id) {
        materials_with_color_requirement_[id] = color_ids;
    }
}

} // namespace terrain
