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

#include "../../logging.hpp"
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

LandGenerator::LandGenerator(const Json::Value& data) :
    current_region(0), current_sub_region(0) {
    for (const Json::Value& region : data) {
        std::string type = region["Type"].asString();
        char first_character = type.at(0);

        std::shared_ptr<stamps::JsonToTile> stamp_generator;
        switch (first_character) {
            case 'P':
            [[fallthrough]] case 'p':
                stamp_generator = std::make_shared<stamps::FromPosition>(region);
                break;
            case 'R':
            [[fallthrough]] case 'r':
                stamp_generator = std::make_shared<stamps::FromRadius>(region);
                break;
            case 'G':
            [[fallthrough]] case 'g':
                stamp_generator = std::make_shared<stamps::FromGrid>(region);
                break;
            [[unlikely]] default:
                LOG_WARNING(
                    logging::terrain_logger,
                    "Terrain stamp type {} is not valid. Must be one of Position, "
                    "Radius, Grid",
                    type
                );
                continue;
        }
        stamp_generators_.push_back(stamp_generator);
    }
}

size_t
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
LandGenerator::get_stamp() const {
    TileStamp out = stamp_generators_[current_region]->get_stamp(current_sub_region);

    return out;
}

void
LandGenerator::next() {
    current_sub_region++;
    if (current_sub_region == stamp_generators_[current_region]->num_sub_region()) {
        // move to next region
        current_region++;
        current_sub_region = 0;
    }
}

namespace stamps {

JsonToTile::JsonToTile(const Json::Value& data) :
    height_(data["Height"].asInt()), height_variance_(data["DH"].asInt()),
    width_(data["Size"].asInt()), width_variance_(data["DC"].asInt()),
    elements_can_stamp_(read_elements(data)),
    stamp_material_id_(data["Material_id"].asInt()),
    stamp_color_id_(data["Color_id"].asInt()) {}

TileStamp
JsonToTile::get_volume(
    glm::imat2x2 center, TerrainOffset Sxy, TerrainOffset Sz, TerrainOffset Dxy,
    TerrainOffset Dz
) const {
    TerrainOffset center_x = rand() % (center[1][0] - center[0][0] + 1) + center[0][0];
    TerrainOffset center_y = rand() % (center[1][1] - center[0][1] + 1) + center[0][1];
    TerrainOffset size_x = rand() % (2 * Dxy + 1) + Sxy - Dxy;
    TerrainOffset size_y = rand() % (2 * Dxy + 1) + Sxy - Dxy;
    TerrainOffset x_min = center_x - size_x / 2;
    TerrainOffset x_max = center_x + size_x / 2;
    TerrainOffset y_min = center_y - size_y / 2;
    TerrainOffset y_max = center_y + size_y / 2;

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

    TerrainOffset z_max = rand() % (Dz + 1) + Sz - Dz / 2;
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
JsonToTile::read_elements(const Json::Value& data) {
    std::set<std::pair<MaterialId, ColorId>> out;

    for (const Json::Value& material_data : data["Can_Stamp"]) {
        MaterialId E = material_data["E"].asInt();
        if (material_data["C"].isInt()) {
            ColorId C = material_data["C"].asInt();
            out.insert(std::make_pair(E, C));
        } else if (material_data["C"].asBool()) {
            out.insert(std::make_pair(E, -1));
        }
    }
    return out;
}

FromRadius::FromRadius(const Json::Value& data) :
    JsonToTile(data), radius_(data["Radius"]["radius"].asInt()),
    number_(data["Radius"]["number"].asInt()), center_variance_(data["DC"].asInt()) {}

TileStamp
FromRadius::get_stamp(size_t current_sub_region) const {
    double distance = static_cast<double>(8 * radius_) / number_ * current_sub_region;
    Side side{static_cast<int>(distance) / 2 / radius_};
    TerrainOffset x_center, y_center;

    switch (side) {
        case Side::TOP:
            x_center = -radius_;
            y_center = -radius_ + std::fmod(distance, 2 * radius_);
            break;
        case Side::RIGHT:
            x_center = -radius_ + std::fmod(distance, 2 * radius_);
            y_center = radius_;
            break;
        case Side::BOTTOM:
            x_center = radius_;
            y_center = radius_ - std::fmod(distance, 2 * radius_);
            break;
        case Side::LEFT:
            x_center = radius_ - std::fmod(distance, 2 * radius_);
            y_center = -radius_;
            break;
        [[unlikely]] default:
            // This should never happen. Not "its wrong for this to happen", but
            // index out of bounds should ont happen.
            // the only way for this to happen is if
            // current_sub_region > num_sub_region
            LOG_CRITICAL(logging::terrain_logger, "Something went horribly wrong");
            abort();
    }

    glm::imat2x2 center = {
        {x_center - center_variance_, y_center - center_variance_},
        {x_center + center_variance_, y_center + center_variance_}
    };

    return get_volume(center, width_, height_, width_variance_, height_variance_);
}

FromPosition::FromPosition(const Json::Value& data) :
    JsonToTile(data), center_variance_(data["DC"].asInt()) {
    points_.reserve(data["Positions"].size());

    for (const Json::Value& point : data["Positions"]) {
        points_.push_back({point[0].asInt(), point.asInt()});
    }
}

TileStamp
FromPosition::get_stamp(size_t current_sub_region) const {
    auto point = points_[current_sub_region];
    glm::imat2x2 center = {
        {point.x, point.y},
        {point.x, point.y}
    };

    return get_volume(center, width_, height_, width_variance_, height_variance_);
}

FromGrid::FromGrid(const Json::Value& data) :
    JsonToTile(data), radius_(data["Grid"]["radius"].asInt()),
    number_(data["Grid"]["number"].asInt()), center_variance_(data["DC"].asInt()) {}

TileStamp
FromGrid::get_stamp(size_t current_sub_region) const {
    TerrainOffset x_center =
        (1 + 2 * (current_sub_region % number_)) * (radius_ / number_) - radius_;
    TerrainOffset y_center =
        (1 + 2 * (current_sub_region / number_)) * (radius_ / number_) - radius_;

    glm::imat2x2 center = {
        {x_center - center_variance_, y_center - center_variance_},
        {x_center + center_variance_, y_center + center_variance_}
    };

    return get_volume(center, width_, height_, width_variance_, height_variance_);
}

} // namespace stamps

} // namespace generation

} // namespace terrain
