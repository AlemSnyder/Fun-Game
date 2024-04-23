#include "shadow_map.hpp"

#include "logging.hpp"
#include "../../handler.hpp"
#include "../../scene/controls.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <memory>
#include <stdexcept>

namespace gui {

namespace gpu_data {

void
ShadowMap::set_light_direction(glm::vec3 light_direction) {
    light_direction_ = std::move(light_direction);
    depth_view_matrix_ =
        glm::lookAt(light_direction_, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
}

void
ShadowMap::set_depth_projection_matrix(glm::mat4 depth_projection_matrix) {
    depth_projection_matrix_ = std::move(depth_projection_matrix);
}

void
ShadowMap::update() {
    glm::mat4 view_matrix = controls::get_view_matrix();
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 inverse_view_projection = glm::inverse(projection_matrix * view_matrix);

    // world space positions of back corners of view field.
    std::vector<glm::vec4> shadow_range_corners;
    // Average position of four corners
    glm::vec4 center(0, 0, 0, 0);

    for (int x = -1; x < 2; x += 2) {
        for (int y = -1; y < 2; y += 2) {
            for (int z = -1; z < 2; z += 2) {
                // corner in camera space
                glm::vec4 corner = {x, y, z, 1}; // w = 1
                // direction in world space
                glm::vec4 direction = inverse_view_projection * corner;
                direction = direction / direction.w;
                shadow_range_corners.emplace_back(direction);
                center = +direction;
            }
        }
    }

    center = center / center.w;
    glm::vec3 center3 = center;

    // update depth view matrix
    depth_view_matrix_ =
        glm::lookAt(light_direction_ + center3, center3, glm::vec3(0, 1, 0));

    for (size_t i = 0; i < shadow_range_corners.size(); i++) {
        shadow_range_corners[i] = depth_view_matrix_ * shadow_range_corners[i];
    }

    // try to generate a prism that covers the entire field of wiew.
    double x_max, x_min = shadow_range_corners[0].x;
    double y_max, y_min = shadow_range_corners[0].y;
    double z_max, z_min = shadow_range_corners[0].z;

    for (const glm::vec4& position : shadow_range_corners) {
        if (position.x > x_max) {
            x_max = position.x;
        }
        if (position.y > y_max) {
            y_max = position.y;
        }
        if (position.z > z_max) {
            z_max = position.z;
        }
        if (position.x < x_min) {
            x_min = position.x;
        }
        if (position.y < y_min) {
            y_min = position.y;
        }
        if (position.z < z_min) {
            z_min = position.z;
        }
    }

    depth_projection_matrix_ =
        glm::ortho(x_min, x_max, y_min, y_max, -z_max - 100.0, -z_min);
}

} // namespace gpu_data

} // namespace gui
