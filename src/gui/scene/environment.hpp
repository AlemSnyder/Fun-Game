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



 public:
    float earth_angle;
    float sun_angle;
    float total_angle;
    float mod_angle;

    glm::vec3 light_direction();
    double diffuse_light_intensity() const;

    Environment_Cycle(
        double earth_tilt, double year_days, double day_seconds, double latitude
    ) :
        earth_tilt_(earth_tilt),
        year_days_(year_days), day_seconds_(day_seconds), latitude_(latitude) {}

    // moon
};

} // namespace scene

} // namespace gui
