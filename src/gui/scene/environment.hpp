#pragma once

#include <glm/glm.hpp>

namespace gui {

namespace scene {

class Environment_Cycle {
 private:
    double earth_tilt_;
    double year_days_;
    double day_seconds_;

    float latitude_;

    float earth_radius = 6.0;
    float atmosphere_height = 0.5;

    glm::mat4 sky_rotation_;

    glm::vec3 light_direction_;
    glm::vec3 sunlight_color_;

 public:
    float earth_angle;
    float sun_angle;
    float total_angle;

    Environment_Cycle(
        double earth_tilt, double year_days, double day_seconds, double latitude
    ) :
        earth_tilt_(earth_tilt),
        year_days_(year_days), day_seconds_(day_seconds), latitude_(latitude) {}

    void update();
    void update_sunlight_color(glm::vec3 light_direction);

    [[nodiscard]] inline glm::vec3
    get_diffuse_light_intensity() const {
        return glm::vec3(1, 1, 1);
    }

    [[nodiscard]] inline glm::vec3
    get_light_direction() const {
        return light_direction_;
    };

    /**
     * @brief Get the sunlight color
     */
    [[nodiscard]] inline glm::vec3
    get_specular_light() const {
        return sunlight_color_;
    }

    inline glm::mat4
    get_sky_rotation() const {
        return sky_rotation_;
    }

    // moon
};

} // namespace scene

} // namespace gui
