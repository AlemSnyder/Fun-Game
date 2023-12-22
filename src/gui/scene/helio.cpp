#include "../../util/time.hpp"
#include "helio.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <math.h>

#include <numbers>

namespace gui {

namespace scene {

void
Helio::update() {
    std::chrono::milliseconds game_time = time_util::get_time();

    double f_time = static_cast<double>(game_time.count()) / 1000.0;

    double time = std::remainder(f_time, (day_seconds_ * year_days_));

    earth_angle = 2 * std::numbers::pi * time / day_seconds_;
    sun_angle = 2 * std::numbers::pi * time / (day_seconds_ * year_days_);
    total_angle = earth_angle + sun_angle;
    float mod_earth_angle = std::remainder(earth_angle, 2 * std::numbers::pi);
    float mod_sun_angle = std::remainder(sun_angle, 2 * std::numbers::pi);

    glm::mat4 ident = glm::mat4(1);

    // rotation due to sun
    glm::vec3 z(0, 0, 1);
    glm::mat4 sun_rotation = glm::rotate(ident, mod_sun_angle, z);

    // rotation due to earth and earth angle/position
    glm::vec3 earth_axis(0, sin(earth_tilt_), cos(earth_tilt_));
    glm::mat4 earth_rotation = glm::rotate(ident, mod_earth_angle, earth_axis);

    // rotation due to latitude;
    glm::vec3 y(0, 1, 0);
    glm::mat4 latitude_rotation = glm::rotate(ident, latitude_, y);

    glm::mat4 rotation = sun_rotation * earth_rotation * latitude_rotation;

    sky_rotation_ = glm::inverse(rotation);

    glm::vec4 light_direction_solar_system(
        glm::cos(sun_angle), glm::sin(sun_angle), 0, 1
    );

    // fancy linear algebra
    glm::vec4 light_direction_v4 =
        glm::transpose(rotation) * light_direction_solar_system;

    light_direction_ = glm::normalize(
        glm::vec3(light_direction_v4.x, light_direction_v4.y, light_direction_v4.z)
    );

    update_sunlight_color(light_direction_);
}

void
Helio::update_sunlight_color(glm::vec3 light_direction) {
    float slope = light_direction.z
                  / pow(pow(light_direction.x, 2) + pow(light_direction.y, 2), .5);

    // attenuation coefficient
    float ar = 1.07 / atmosphere_height;
    float ag = 1.25 / atmosphere_height;
    float ab = 1.46 / atmosphere_height;

    int test_points = 3;

    // light intensity
    float ir = 2.85 / test_points;
    float ig = 3.30 / test_points;
    float ib = 3.90 / test_points;

    sunlight_color_ = glm::vec3(0, 0, 0);
    for (int i = 0; i < test_points; i++) {
        float h = earth_radius + atmosphere_height * i / test_points;

        float c = pow(earth_radius + atmosphere_height, 2) - pow(h, 2);
        float a = 1 + pow(slope, 2);

        float length =
            (-2 * h * slope + pow(pow(2 * h * slope, 2) + 4 * a * c, .5)) / (2 * a);

        length =
            length * pow(1 + pow(slope, 2), .5) + atmosphere_height * i / test_points;

        glm::vec3 add_color(
            ir * exp(-ar * length), ig * exp(-ag * length), ib * exp(-ab * length)
        );

        sunlight_color_ += add_color;
    }
}

} // namespace scene

} // namespace gui
