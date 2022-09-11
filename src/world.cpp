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

// void World::save(){
//     terrain_main.qb_save(path);
// }

const terrain::Material*
World::get_material(int material_id) const {
    return &materials.at(material_id);
}

void
World::init_materials(Json::Value material_data) {
    for (auto element_it = material_data.begin(); element_it != material_data.end();
         element_it++) {
        std::vector<std::pair<const std::string, uint32_t>> color_vector;

        Json::Value material = *element_it;
        std::string name = element_it.key().asString();
        for (unsigned int i = 0; i < material["colors"].size(); i++) {
            const std::string string = material["colors"][i]["name"].asString();
            uint32_t color = std::stoll(material["colors"][i]["hex"].asString(), 0, 16);
            color_vector.push_back(std::make_pair(string, color));
        }

        terrain::Material mat{
            color_vector,                                    // color
            static_cast<uint8_t>(material["speed"].asInt()), // speed_multiplier
            material["solid"].asBool(),                      // solid
            static_cast<uint8_t>(material["id"].asInt()),    // element_id
            name};                                           // name
        materials.insert(std::make_pair(mat.element_id, mat));
    }
}

void
World::get_mesh_greedy(
    std::vector<unsigned short>& indices, std::vector<glm::vec3>& indexed_vertices,
    std::vector<glm::vec3>& indexed_colors, std::vector<glm::vec3>& indexed_normals
) const {
    entity::generate_mesh(
        terrain_main, indices, indexed_vertices, indexed_colors, indexed_normals
    );
}

World::World() {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    init_materials(materials_json);

    std::vector<int> grass_grad_data;
    for (unsigned int i = 0; i < materials_json["Dirt"]["Gradient"]["levels"].size();
         i++) {
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt(
        ));
    }

    Json::Value biome_data;
    std::ifstream biome_file = files::open_data_file("biome_data.json");
    biome_file >> biome_data;

    std::cout << "start of terrain\n";
    terrain_main = terrain::Terrain();
}

World::World(const std::string path) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    init_materials(materials_json);

    std::vector<int> grass_grad_data;
    for (unsigned int i = 0; i < materials_json["Dirt"]["Gradient"]["levels"].size();
         i++) {
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt(
        ));
    }

    std::cout << "start of terrain\n";

    terrain_main = terrain::Terrain(path, &materials);
}

World::World(Json::Value materials_json, Json::Value biome_data, unsigned int x_tiles, unsigned int y_tiles) {
    init_materials(materials_json);

    std::vector<int> grass_grad_data;
    for (unsigned int i = 0; i < materials_json["Dirt"]["Gradient"]["levels"].size();
         i++) {
        grass_grad_data.push_back(materials_json["Dirt"]["Gradient"]["levels"][i].asInt(
        ));
    }

    std::cout << "start of terrain\n";

    terrain_main = terrain::Terrain(
        x_tiles, y_tiles, 32, 128, 5, &materials, biome_data["Biome_1"], grass_grad_data,
        materials_json["Dirt"]["Gradient"]["midpoint"].asInt()
    );
}

World::World(Json::Value biome_data, int tile_type) {
    Json::Value materials_json;
    std::ifstream materials_file = files::open_data_file("materials.json");
    materials_file >> materials_json;

    init_materials(materials_json);

    terrain_main =
        terrain::Terrain(3, 3, 32, 128, 5, tile_type, &materials, biome_data);
}
