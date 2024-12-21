#pragma once

#include "gui/render/structures/star_data.hpp"
#include "gui/scene/helio.hpp"
#include "util/files.hpp"

namespace world {

class Climate {
 private:
    gui::gpu_data::StarShape sun_data_;
    gui::gpu_data::StarData stars_data_;

 public:
    inline Climate() : stars_data_(files::get_data_path() / "stars.json") {}

    const auto*
    get_sun_data() const {
        return &sun_data_;
    }

    const auto
    get_stars_data() const {
        return &stars_data_;
    }
};

} // namespace world
