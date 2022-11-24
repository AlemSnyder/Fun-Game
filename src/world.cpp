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
 * @file world.cpp
 *
 * @author @AlemSnyder
 *
 * @brief Defines World class
 *
 *
 */

#include "world.hpp"

#include "entity/mesh.hpp"
#include "terrain/terrain.hpp"
#include "util/files.hpp"

#include <cstdint>
#include <fstream>
#include <string>

constexpr uint8_t HEX_COLOR_BASE = 16;

std::vector<int>
World::get_grass_grad_data(const Json::Value& materials_json) {
    std::vector<int> grass_grad_data;
    for (const auto& grad_level : materials_json["Dirt"]["Gradient"]["levels"]) {
        grass_grad_data.push_back(grad_level.asInt());
    }
    return grass_grad_data;
}

std::map<int, const terrain::Material>
World::init_materials(const Json::Value& material_data) {
    std::map<int, const terrain::Material> out;
    for (auto element_it = material_data.begin(); element_it != material_data.end();
         element_it++) {
        std::vector<std::pair<const std::string, uint32_t>> color_vector;

        Json::Value material = *element_it;
        std::string name = element_it.key().asString();
        for (auto& color_data : material["colors"]) {
            const std::string name = color_data["name"].asString();
            uint32_t color =
                std::stoll(color_data["hex"].asString(), nullptr, HEX_COLOR_BASE);

            color_vector.emplace_back(name, color);
        }

        terrain::Material mat{
            color_vector,                                    // color
            static_cast<uint8_t>(material["speed"].asInt()), // speed_multiplier
            material["solid"].asBool(),                      // solid
            static_cast<uint8_t>(material["id"].asInt()),    // element_id
            name,                                            // name
        };
        out.insert(std::make_pair(mat.element_id, mat));
    }
    return out;
}

World::World(const Json::Value& materials_json, const std::string& path) :
    materials(init_materials(materials_json)), terrain_main(path, &materials) {
    // @AlemSnyder WHAT IS DIS
    std::vector<int> grass_grad_data = get_grass_grad_data(materials_json);
}

World::World(
    const Json::Value& materials_json, const Json::Value& biome_data, uint32_t x_tiles,
    uint32_t y_tiles
) :
    materials(init_materials(materials_json)),
    terrain_main(
        x_tiles, y_tiles, macro_tile_size, height, 5, &materials, biome_data["Biome_1"],
        get_grass_grad_data(materials_json),
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    ) {}

World::World(
    const Json::Value& materials_json, const Json::Value& biome_data, int tile_type
) :
    materials(init_materials(materials_json)),
    terrain_main(3, 3, macro_tile_size, height, 5, tile_type, &materials, biome_data) {}
