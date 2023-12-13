#include "environment.hpp"
#include "../../util/time.hpp"

#include <numbers>

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gui {

namespace scene {

glm::vec3
Environment_Cycle::light_direction() {
    std::chrono::milliseconds game_time = time_util::get_time();

    double f_time = static_cast<double>(game_time.count()) / 1000.0;

    double time = std::remainder(f_time, (day_seconds_ * year_days_));

    earth_angle = 2 * std::numbers::pi * time / day_seconds_;
    sun_angle = 2 * std::numbers::pi * time / ( day_seconds_ * year_days_);
    total_angle = earth_angle + sun_angle;
    mod_angle = std::remainder(total_angle, 2 * std::numbers::pi);

    glm::mat4 ident = glm::mat4(1);

    // rotation due to sun and earth angle/position
    glm::vec3 z(0,0,1);
    glm::mat4 rotation_z = glm::rotate(ident, mod_angle, z);

    // rotation due to latitude;
    glm::vec3 y(0,1,0);
    glm::mat4 rotation_y = glm::rotate(ident, latitude_, y);
    glm::mat4 inverse_rotation_y = glm::rotate(ident, latitude_, y);

    float mod_earth_angle = std::remainder(earth_angle, 2 * std::numbers::pi);

    glm::mat4 sky_rotation_z = glm::rotate(ident, -mod_earth_angle, z);

    sky_rotation_ = inverse_rotation_y * sky_rotation_z;

    glm::mat4 rotation = rotation_z * rotation_y;

    glm::vec4 light_direction_solar_system(glm::cos(sun_angle), glm::sin(sun_angle), 0, 1);

    glm::vec4 light_direction;
    light_direction = glm::transpose(rotation) * light_direction_solar_system;

    return glm::normalize(glm::vec3(light_direction.x, light_direction.y, light_direction.z));

}

} // namespace scene

} // namespace gui
