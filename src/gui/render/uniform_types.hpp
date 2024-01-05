#pragma once

#include <glm/glm.hpp>

namespace gui {

namespace render {

class LightEnvironment {
 private:
 public:
    virtual ~LightEnvironment(){};

    virtual glm::vec3 get_light_direction() const = 0;
    virtual glm::vec3 get_diffuse_light() const = 0;
    virtual glm::vec3 get_specular_light() const = 0;
};

class StarRotation {
 public:
    virtual ~StarRotation(){};
    virtual glm::mat4 get_sky_rotation() const = 0;
};

} // namespace render

} // namespace gui
