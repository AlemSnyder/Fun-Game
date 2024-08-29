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
#include "logging.hpp"
#include "terrain_genreration_types.hpp"
#include "tile_stamp.hpp"
#include "types.hpp"

#include <math.h>

#include <cstdint>
#include <iostream>
#include <map>
#include <random>
#include <string>

namespace terrain {

namespace generation {

LandGenerator::LandGenerator(const std::vector<generation_stamp_t>& data) :
    current_region(0), current_sub_region(0) {
    for (const generation_stamp_t& region : data) {
        generation_stamp_type type = region.type;

        std::shared_ptr<stamps::JsonToTile> stamp_generator;
        switch (type) {
            case generation_stamp_type::POSITION:
                stamp_generator = std::make_shared<stamps::FromPosition>(region);
                break;
            case generation_stamp_type::RADIUS:
                stamp_generator = std::make_shared<stamps::FromRadius>(region);
                break;
            case generation_stamp_type::GRID:
                stamp_generator = std::make_shared<stamps::FromGrid>(region);
                break;
            [[unlikely]] default:
                abort();
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
    std::uniform_int_distribution<TerrainOffset> center_x_dist(
        center[0][0], center[1][0]
    );
    TerrainOffset center_x = center_x_dist(rand_engine);
    // same with y
    std::uniform_int_distribution<TerrainOffset> center_y_dist(
        center[0][1], center[1][1]
    );
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
    std::uniform_int_distribution<> bool_dist(0, 1);
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
    // clang-format off
    return {
        x_min,
        y_min,
        0, // z_min = 0
        x_max,
        y_max,
        z_max,
        stamp_material_id_,
        stamp_color_id_,
        elements_can_stamp_,
    };
    // clang-format on
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

FromPosition::FromPosition(
    const generation_stamp_t& data, const stamp_generation_position_data_t& type_data
) :
    JsonToTile(data),
    center_variance_(data.DC) {
    points_.reserve(type_data.positions.size());

    for (const auto& [x, y] : type_data.positions) {
        points_.push_back({x, y});
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

AddToTop::AddToTop(const layer_effects_t& layer_effect_data) :
    elements_above_(layer_effect_data.above_colors),
    elements_can_overwrite_(layer_effect_data.can_override),

    stamp_material_id_(layer_effect_data.material_id),
    stamp_color_id_(layer_effect_data.color_id) {
    for (const layer_effect_data_t& range_data : layer_effect_data.how_to_add) {
        data_.insert(range_data);
    }
}

Dim
AddToTop::get_final_height(Dim height) const {
    auto add_data =
        data_.lower_bound(layer_effect_data_t(height, 0, 0, layer_effect_add::NONE));

    if (add_data == data_.end()) {
        return height;
    }

    switch (add_data->add_directions) {
        case layer_effect_add::TO:
            return add_data->data;
            break;
        case layer_effect_add::ADD:
            return height + add_data->data;
            break;

        default:
            return 0;
            break;
    }
}

} // namespace generation

} // namespace terrain
