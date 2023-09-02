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
#include "tile_stamp.hpp"

#include <json/json.h>

#include <math.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <string>

namespace terrain {

namespace generation {

LandGenerator::LandGenerator(const Json::Value data) :
    current_region(0), current_sub_region(0) {
    for (const Json::Value& region : data) {
        std::shared_ptr<JsonToTile> stamp_generator;
        if (region["type"] == "Positions") {
            stamp_generator = std::make_shared<FromPosition>(FromPosition(region));
        } else if (region["type"] == "Radius") {
            stamp_generator = std::make_shared<FromRadius>(FromRadius(region));
        } else if (region["type"] == "Grid") {
            stamp_generator = std::make_shared<FromGrid>(FromGrid(region));
        }
        stamp_generators_.push_back(stamp_generator);
    }
}

unsigned int
LandGenerator::get_num_stamps(const Json::Value& biome) {
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
    TileStamp out = stamp_generators_[current_region]->operator()(current_sub_region);

    return out;
}

void
LandGenerator::next() {
    current_sub_region++;
    if (current_sub_region == stamp_generators_[current_region]->num()) {
        current_region++;
        current_sub_region = 0;
    }
}

JsonToTile::JsonToTile(const Json::Value& data) :
    elements_can_stamp_(read_elements_can_stamp(data)),
    stamp_material_id_(data["Material_id"].asInt()),
    stamp_color_id_(data["Color_id"].asInt()) {
    width_ = data["Size"].asInt();
    height_ = data["Height"].asInt();

    width_variance_ = data["DC"].asInt();
    height_variance_ = data["DH"].asInt();
}

TileStamp
JsonToTile::get_volume(int center[2][2], int Sxy, int Sz, int Dxy, int Dz) const {
    int center_x = rand() % (center[1][0] - center[0][0] + 1) + center[0][0];
    int center_y = rand() % (center[1][1] - center[0][1] + 1) + center[0][1];
    int size_x = rand() % (2 * Dxy + 1) + Sxy - Dxy;
    int size_y = rand() % (2 * Dxy + 1) + Sxy - Dxy;
    int x_min = center_x - size_x / 2;
    int x_max = center_x + size_x / 2;
    int y_min = center_y - size_y / 2;
    int y_max = center_y + size_y / 2;

    if (size_x % 2 != 0) {
        if (rand() % 2 - 1)
            x_min--;
        else
            x_max--;
    }
    if (size_y % 2 != 0) {
        if (rand() % 2 - 1)
            y_min--;
        else
            y_max--;
    }

    int z_max = rand() % (Dz + 1) + Sz - Dz / 2;
    return {
        x_min,
        y_min,
        0,
        x_max,
        y_max,
        z_max,
        stamp_material_id_,
        stamp_color_id_,
        elements_can_stamp_
    };
}

std::set<std::pair<MaterialId, ColorId>>
JsonToTile::read_elements_can_stamp(const Json::Value& data) {
    std::set<std::pair<MaterialId, ColorId>> out;

    for (const Json::Value& material_data : data["Can_Stamp"]) {
        int E = material_data["E"].asInt();
        if (material_data["C"].isInt()) {
            int C = material_data["C"].asInt();
            out.insert(std::make_pair(E, C));
        } else if (material_data["C"].asBool()) {
            out.insert(std::make_pair(E, -1));
        }
    }
    return out;
}

FromRadius::FromRadius(const Json::Value& data) : JsonToTile(data) {
    number_ = data["Radius"]["number"].asInt();
    radius_ = data["Radius"]["radius"].asInt();

    center_variance_ = data["DC"].asInt();
}

TileStamp
FromRadius::operator()(int current_sub_region) const {
    double distance = static_cast<double>(8 * radius_) / number_ * current_sub_region;
    int side = static_cast<int>(distance) / 2 / radius_;
    int x_center, y_center;

    if (side == 0) {
        x_center = -radius_;
        y_center = -radius_ + std::fmod(distance, 2 * radius_);
    } else if (side == 1) {
        x_center = -radius_ + std::fmod(distance, 2 * radius_);
        y_center = radius_;
    } else if (side == 2) {
        x_center = radius_;
        y_center = radius_ - std::fmod(distance, 2 * radius_);
    } else if (side == 3) {
        x_center = radius_ - std::fmod(distance, 2 * radius_);
        y_center = -radius_;
    } else {
        throw std::invalid_argument("Something went horribly wrong");
    }

    int center[2][2] = {
        {x_center - center_variance_, y_center - center_variance_},
        {x_center + center_variance_, y_center + center_variance_}
    };

    return get_volume(center, width_, height_, width_variance_, height_variance_);
}

FromPosition::FromPosition(const Json::Value& data) : JsonToTile(data) {
    points_.reserve(data["Positions"].size());

    for (const Json::Value& point : data["Positions"]) {
        points_.push_back({point[0].asInt(), point.asInt()});
    }

    center_variance_ = data["DC"].asInt();
}

TileStamp
FromPosition::operator()(int current_sub_region) const {
    auto point = points_[current_sub_region];
    int center[2][2]{
        {point.x, point.y},
        {point.x, point.y}
    };

    return get_volume(center, width_, height_, width_variance_, height_variance_);
}

FromGrid::FromGrid(const Json::Value& data) : JsonToTile(data) {
    number_ = data["Radius"]["number"].asInt();
    radius_ = data["Radius"]["radius"].asInt();

    center_variance_ = data["DC"].asInt();
}

TileStamp
FromGrid::operator()(int current_sub_region) const {
    int x_center =
        (1 + 2 * (current_sub_region % number_)) * (radius_ / number_) - radius_;
    int y_center =
        (1 + 2 * (current_sub_region / number_)) * (radius_ / number_) - radius_;

    int center[2][2] = {
        {x_center - center_variance_, y_center - center_variance_},
        {x_center + center_variance_, y_center + center_variance_}
    };

    return get_volume(center, width_, height_, width_variance_, height_variance_);
}

} // namespace generation

} // namespace terrain
