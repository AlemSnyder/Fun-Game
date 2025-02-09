#pragma once

#include "../render/structures/uniform_types.hpp"

#include <glm/glm.hpp>

namespace gui {

namespace scene {

class Helio : public render::LightEnvironment, public render::StarRotation {
 private:
    double earth_tilt_;  // angle of earth relative to z axis
    double year_days_;   // number times earth rotates around its axis before
                         // it rotates around the sun.
    double day_seconds_; // number of seconds for earth to rotate around its
                         // axis

    float latitude_;

    float earth_radius = 6.0;
    float atmosphere_height = 0.5;
    glm::vec3 color_intensities_ = glm::vec3(0.8, 0.85, 0.9);

    glm::mat4 sky_rotation_;

    glm::vec3 light_direction_;
    glm::vec3 sunlight_color_;

    bool manual_light_direction_ = false;

 public:
    // gives a slight insight to the internal workings
    // updated every frame, but never read.
    float earth_angle;
    float sun_angle;
    float total_angle;

    Helio(double earth_tilt, double year_days, double day_seconds, double latitude) :
        earth_tilt_(earth_tilt), year_days_(year_days), day_seconds_(day_seconds),
        latitude_(latitude) {}

    bool&
    control_lighting() {
        return manual_light_direction_;
    }

    /**
     * @brief Updates sun position and light colors
     */
    void update();
    /**
     * @brief Get the sunlight color
     */
    void update_sunlight_color(glm::vec3 light_direction);

    /**
     * @brief Color of background light caused by sunlight
     */
    [[nodiscard]] inline glm::vec3
    get_diffuse_light() const override {
        return 2 * glm::length(sunlight_color_) * color_intensities_;
    }

    /**
     * @brief Get the sunlight direction. Equivalent to sun position.
     */
    [[nodiscard]] inline glm::vec3
    get_light_direction() const override {
        return light_direction_;
    };

    /**
     * @brief Get the sunlight direction. Equivalent to sun position.
     */
    [[nodiscard]] inline glm::vec3&
    control_light_direction() {
        return light_direction_;
    };

    /**
     * @brief Get the sunlight color
     */
    [[nodiscard]] inline glm::vec3
    get_specular_light() const override {
        return sunlight_color_;
    }

    /**
     * @brief Get the sky rotation matrix.
     *
     * @details Used to rotate stars for viewer on earth.
     */
    [[nodiscard]] inline glm::mat4
    get_sky_rotation() const override {
        return sky_rotation_;
    }

    // moon
};

} // namespace scene

} // namespace gui
