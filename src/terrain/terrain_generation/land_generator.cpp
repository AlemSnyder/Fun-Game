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

#include "land_generator.hpp"

#include "../material.hpp"
#include "tilestamp.hpp"

#include <json/json.h>

#include <math.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <string>

namespace terrain {

namespace terrain_generation {

LandGenerator::LandGenerator(
    const std::map<int, const Material>* materials_, Json::Value data
) {
    materials = materials_;
    data_ = data;
    current_region = 0;
    current_sub_region = 0;
}

LandGenerator::LandGenerator() {
    current_region = 0;
    current_sub_region = 0;
}

unsigned int
LandGenerator::get_num_stamps(Json::Value biome) {
    if (biome["Type"].asString() == "Positions") {
        return biome["Positions"].size();
    } else if (biome["Type"].asString() == "Grid") {
        int num = biome[biome["Type"].asCString()]["number"].asInt();
        return num * num;
    } else {
        return biome[biome["Type"].asString()]["number"].asInt();
    }
}

TileStamp
LandGenerator::get_this_stamp() const {
    TileStamp out;
    out.mat = &(*materials).at(data_[current_region]["Material_id"].as<int>());
    out.color_id = data_[current_region]["Color_id"].asInt();
    for (Json::Value::ArrayIndex i = 0; i < data_[current_region]["Can_Stamp"].size();
         i++) {
        int E = data_[current_region]["Can_Stamp"][i]["E"].asInt();
        if (data_[current_region]["Can_Stamp"][i]["C"].isInt()) {
            int C = data_[current_region]["Can_Stamp"][i]["C"].asInt();
            out.elements_can_stamp.insert(std::make_pair(E, C));
        } else if (data_[current_region]["Can_Stamp"][i]["C"].asBool()) {
            for (unsigned int C = 0; C < (*materials).at(E).color.size(); C++) {
                out.elements_can_stamp.insert(std::make_pair(E, C));
            }
        }
    }
    std::string type = data_[current_region]["Type"].asString();

    if (type == "Positions") {
        from_positions(current_region, current_sub_region, out);
    } else if (type == "Radius") {
        from_radius(current_region, current_sub_region, out);
    } else if (type == "Grid") {
        from_grid(current_region, current_sub_region, out);
    }
    return out;
}

void
LandGenerator::from_radius(int cr, int csr, TileStamp& ts) const {
    int radius = data_[cr]["Radius"]["radius"].asInt();
    int number = data_[cr]["Radius"]["number"].asInt();

    double distance = static_cast<double>(8 * radius) / number * csr;
    int side = static_cast<int>(distance) / 2 / radius;
    int x_center, y_center;

    if (side == 0) {
        x_center = -radius;
        y_center = -radius + std::fmod(distance, 2 * radius);
    } else if (side == 1) {
        x_center = -radius + std::fmod(distance, 2 * radius);
        y_center = radius;
    } else if (side == 2) {
        x_center = radius;
        y_center = radius - std::fmod(distance, 2 * radius);
    } else if (side == 3) {
        x_center = radius - std::fmod(distance, 2 * radius);
        y_center = -radius;
    } else {
        throw std::invalid_argument("Something went horribly wrong");
    }

    int DC = data_[cr]["Radius"]["DC"].asInt();
    int center[2][2] = {
        {x_center - DC, y_center - DC},
        {x_center + DC, y_center + DC}
    };

    std::array<int, 6> volume = get_volume(
        center, data_[cr]["Size"].asInt(), data_[cr]["Hight"].asInt(),
        data_[cr]["DS"].asInt(), data_[cr]["DH"].asInt()
    );

    ts.x_start = volume[0];
    ts.y_start = volume[1];
    ts.z_start = volume[2];
    ts.x_end = volume[3];
    ts.y_end = volume[4];
    ts.z_end = volume[5];
}

void
LandGenerator::from_grid(int cr, int csr, TileStamp& ts) const {
    int number = data_[cr]["Grid"]["number"].asInt();
    int radius = data_[cr]["Grid"]["radius"].asInt();

    int x_center = (1 + 2 * (csr % number)) * (radius / number) - radius;
    int y_center = (1 + 2 * (csr / number)) * (radius / number) - radius;
    int DC = data_[cr]["Grid"]["DC"].asInt();

    int center[2][2] = {
        {x_center - DC, y_center - DC},
        {x_center + DC, y_center + DC}
    };

    std::array<int, 6> volume = get_volume(
        center, data_[cr]["Size"].asInt(), data_[cr]["Hight"].asInt(),
        data_[cr]["DS"].asInt(), data_[cr]["DH"].asInt()
    );

    ts.x_start = volume[0];
    ts.y_start = volume[1];
    ts.z_start = volume[2];
    ts.x_end = volume[3];
    ts.y_end = volume[4];
    ts.z_end = volume[5];
}

void
LandGenerator::from_positions(int cr, int csr, TileStamp& ts) const {
    Json::Value xy_positions = data_[cr]["Positions"][csr];
    int center[2][2] = {
        {xy_positions[0].asInt(), xy_positions[1].asInt()},
        {xy_positions[0].asInt(), xy_positions[1].asInt()}
    };

    std::array<int, 6> volume = get_volume(
        center, data_[cr]["Size"].asInt(), data_[cr]["Hight"].asInt(),
        data_[cr]["DS"].asInt(), data_[cr]["DH"].asInt()
    );

    ts.x_start = volume[0];
    ts.y_start = volume[1];
    ts.z_start = volume[2];
    ts.x_end = volume[3];
    ts.y_end = volume[4];
    ts.z_end = volume[5];
}

std::array<int, 6>
LandGenerator::get_volume(int center[2][2], int Sxy, int Sz, int Dxy, int Dz) const {
    int center_x = rand() % (center[1][0] - center[0][0] + 1) + center[0][0];
    int center_y = rand() % (center[1][1] - center[0][1] + 1) + center[0][1];
    int size_x = rand() % (2 * Dxy + 1) + Sxy - Dxy;
    int size_y = rand() % (2 * Dxy + 1) + Sxy - Dxy;
    int x_min = center_x - size_x / 2;
    int x_max = center_x + size_x / 2;
    int y_min = center_y - size_y / 2;
    int y_max = center_y + size_y / 2;

    if (!(size_x % 2)) {
        if (rand() % 2 - 1) {
            x_min--;
        } else {
            x_max--;
        }
    }
    if (!(size_y % 2)) {
        if (rand() % 2 - 1) {
            y_min--;
        } else {
            y_max--;
        }
    }

    int z_max = rand() % (Dz + 1) + Sz - Dz / 2;
    return {x_min, y_min, 0, x_max, y_max, z_max};
}

} // namespace terrain_generation

} // namespace terrain
