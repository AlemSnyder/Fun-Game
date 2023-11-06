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
#include "../../types.hpp"
#include "../material.hpp"
#include "tile_stamp.hpp"

#include <json/json.h>

#include <math.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <random>
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
                [[fallthrough]];
            case 'p':
                stamp_generator = std::make_shared<stamps::FromPosition>(region);
                break;
            case 'R':
                [[fallthrough]];
            case 'r':
                stamp_generator = std::make_shared<stamps::FromRadius>(region);
                break;
            case 'G':
                [[fallthrough]];
            case 'g':
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

TileStamp
JsonToTile::get_volume(glm::imat2x2 center, std::default_random_engine& rand_engine)
    const {
    // center_x between center[1][0] and center[0][0] inclusive
    std::uniform_int_distribution<TerrainOffset> center_x_dist(center[0][0], center[1][0]);
    TerrainOffset center_x = center_x_dist(rand_engine);
    // same with y
    std::uniform_int_distribution<TerrainOffset> center_y_dist(center[0][1], center[1][1]);
    TerrainOffset center_y = center_y_dist(rand_engine);
    // size 'centered' at width and can be between width-width_variance to width +
    // width_variance
    std::uniform_int_distribution<TerrainOffset> size_dist(
        -width_variance_, width_variance_
    );
    TerrainOffset size_x = width_ + size_dist(rand_engine);
    TerrainOffset size_y = width_ + size_dist(rand_engine);
    // convert center and side off sets to rectangle edge values
    TerrainOffset x_min = center_x - size_x / 2;
    TerrainOffset x_max = center_x + size_x / 2;
    TerrainOffset y_min = center_y - size_y / 2;
    TerrainOffset y_max = center_y + size_y / 2;

    // mode edges for randomness
    // I have long since forgotten the exact reason, but it has to do with randomness
    std::uniform_int_distribution<> bool_dist(0,1);
    if (size_x % 2 != 0) {
        if (bool_dist(rand_engine))
            x_min--;
        else
            x_max--;
    }
    if (size_y % 2 != 0) {
        if (bool_dist(rand_engine))
            y_min--;
        else
            y_max--;
    }

    // z_max centered at height
    std::uniform_int_distribution<TerrainOffset> height_dist(
        -height_variance_, height_variance_
    );
    TerrainOffset z_max = height_ + height_dist(rand_engine);
    return {
        x_min,
        y_min,
        0, // y_min = 0
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

    for (const Json::Value& material_data : data) {
        MaterialId mat_id;
        if (material_data["E"].isInt())
            mat_id = material_data["E"].asInt();
        else
            mat_id = MAT_ANY_MATERIAL;
        if (material_data["C"].isInt()) {
            ColorId color_id = material_data["C"].asInt();
            out.insert(std::make_pair(mat_id, color_id));
        } else if (material_data["C"].asBool()) {
            out.insert(std::make_pair(mat_id, -1));
        }
    }
    return out;
}

TileStamp
FromRadius::get_stamp(
    size_t current_sub_region, std::default_random_engine& rand_engine
) const {
    double distance = static_cast<double>(8 * radius_) / number_ * current_sub_region;
    Side side{static_cast<uint8_t>(distance / 2 / radius_)};
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

    return get_volume(center, rand_engine);
}

FromPosition::FromPosition(const Json::Value& data) :
    JsonToTile(data), center_variance_(data["DC"].asInt()) {
    points_.reserve(data["Positions"].size());

    for (const Json::Value& point : data["Positions"]) {
        points_.push_back({point[0].asInt(), point.asInt()});
    }
}

TileStamp
FromPosition::get_stamp(
    size_t current_sub_region, std::default_random_engine& rand_engine
) const {
    auto point = points_[current_sub_region];
    glm::imat2x2 center = {
        {point.x, point.y},
        {point.x, point.y}
    };

    return get_volume(center, rand_engine);
}

TileStamp
FromGrid::get_stamp(size_t current_sub_region, std::default_random_engine& rand_engine)
    const {
    TerrainOffset x_center =
        (1 + 2 * (current_sub_region % number_)) * (radius_ / number_) - radius_;
    TerrainOffset y_center =
        (1 + 2 * (current_sub_region / number_)) * (radius_ / number_) - radius_;

    glm::imat2x2 center = {
        {x_center - center_variance_, y_center - center_variance_},
        {x_center + center_variance_, y_center + center_variance_}
    };

    return get_volume(center, rand_engine);
}

} // namespace stamps

AddToTop::AddToTop(const Json::Value& json_data) :
    elements_above_(stamps::JsonToTile::read_elements(json_data["above_colors"])),
    elements_can_overwrite_(stamps::JsonToTile::read_elements(json_data["Can_Overwrite"]
    )),
    stamp_material_id_(json_data["Material_id"].asInt()),
    stamp_color_id_(json_data["Color_id"].asInt()) {
    for (const Json::Value& range_data : json_data["how_to_add"]) {
        Dim start_height = range_data["from"][0].asInt();
        Dim end_height = range_data["from"][1].asInt();
        Dim data_value = range_data["data"].asInt();

        std::string type = range_data["Type"].asString();
        char first_character = type.at(0);

        AddMethod adder;
        switch (first_character) {
            case 'A':
                [[fallthrough]];
            case 'a':
                adder = {start_height, end_height, data_value, AddDirections::Add};
                break;

            case 'T':
                [[fallthrough]];
            case 't':
                adder = {start_height, end_height, data_value, AddDirections::To};
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
        data_.insert(adder);
    }
}

Dim
AddToTop::get_final_height(Dim height) const {
    auto add_data = data_.lower_bound(AddMethod(height, 0, 0, AddDirections::None));

    if (add_data == data_.end()) {
        return height;
    }

    switch (add_data->add_directions) {
        case AddDirections::To:
            return add_data->data;
            break;
        case AddDirections::Add:
            return height + add_data->data;
            break;

        default:
            return 0;
            break;
    }
}

} // namespace generation

} // namespace terrain
