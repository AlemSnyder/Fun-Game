#include "shadow_map.hpp"

#include "gui/handler.hpp"
#include "gui/scene/controls.hpp"
#include "logging.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <algorithm>
#include <memory>
#include <stdexcept>

namespace gui {

namespace gpu_data {

ShadowMap::ShadowMap(screen_size_t w, screen_size_t h, FrameBufferSettings settings) :
    FrameBufferBase(w, h, settings) {
    TextureSettings depth_texture_settings{
        .internal_format = GPUPixelStorageFormat::DEPTH_16,
        .read_format = GPUPixelReadFormat::DEPTH_COMPONENT,
        .type = GPUPixelType::HALF_FLOAT,
        .min_filter = GL_LINEAR};

    connect_depth_texture(
        std::make_shared<Texture2D>(width_, height_, depth_texture_settings, false)
    );

    // No color output in the bound framebuffer, only depth.
    glNamedFramebufferDrawBuffer(frame_buffer, GL_NONE);

    // Always check that our framebuffer is ok
    status_check();
}

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
ShadowMap::set_inverse_view_projection(glm::mat4 inverse_view_projection) {
    inverse_view_projection_ = std::move(inverse_view_projection);
}

void
ShadowMap::update_depth_projection_matrix() {
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
                glm::vec4 direction = inverse_view_projection_ * corner;
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
    float x_max = shadow_range_corners[0].x;
    float x_min = shadow_range_corners[0].x;
    float y_max = shadow_range_corners[0].y;
    float y_min = shadow_range_corners[0].y;
    float z_max = shadow_range_corners[0].z;
    float z_min = shadow_range_corners[0].z;

    for (const glm::vec4& position : shadow_range_corners) {
        x_max = std::max(position.x, x_max);
        y_max = std::max(position.x, y_max);
        z_max = std::max(position.x, z_max);

        x_min = std::min(position.x, x_min);
        y_min = std::min(position.x, y_min);
        z_min = std::min(position.x, z_min);
    }

    depth_projection_matrix_ =
        glm::ortho(x_min, x_max, y_min, y_max, -z_max - 100.0f, -z_min);
}

} // namespace gpu_data

} // namespace gui
