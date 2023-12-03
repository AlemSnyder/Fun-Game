#include "environment.hpp"
#include "../../util/time.hpp"

#include <numbers>

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace gui {

namespace scene {

glm::vec3
Environment_Cycle::light_direction() const {
    std::chrono::milliseconds game_time = time_util::get_time();

    float f_time = game_time.count();

    float time = fmod(f_time / 1000.0, (day_seconds_ * year_days_));

    float earth_angle = 2 * std::numbers::pi * time / day_seconds_;
    float sun_angle = 2 * std::numbers::pi * time / ( day_seconds_ * year_days_);
    float total_angle = earth_angle + sun_angle;
    float mod_angle = fmod(total_angle, 2 * std::numbers::pi);

    glm::mat4 ident = glm::mat4(1);
    glm::vec3 z(0,0,1);
    glm::mat4 rotation_z = glm::rotate(ident, mod_angle, z);

    glm::vec3 y(0,1,0);
    glm::mat4 rotation_x = glm::rotate(ident, latitude_, y);

    glm::mat4 rotation = rotation_z * rotation_x;

    glm::vec4 light_direction_solar_system(glm::cos(sun_angle), glm::sin(sun_angle), 0, 1);

    glm::vec4 light_direction;
    light_direction = glm::transpose(rotation) * light_direction_solar_system;

    return glm::normalize(glm::vec3(light_direction.x, light_direction.y, light_direction.z));

}

} // namespace scene

} // namespace gui
