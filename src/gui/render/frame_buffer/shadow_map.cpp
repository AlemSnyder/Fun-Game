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

namespace frame_buffer {

ShadowMap::ShadowMap(screen_size_t w, screen_size_t h) {
    shadow_width_ = w;
    shadow_height_ = h;

    glGenFramebuffers(1, &frame_buffer_id_);
    FrameBufferHandler::instance().bind_fbo(frame_buffer_id_);

    // Depth texture. Slower than a depth buffer, but you can sample it later in
    // your shader

    glGenTextures(1, &depth_texture_id_);
    glBindTexture(GL_TEXTURE_2D, depth_texture_id_);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_width_, shadow_height_, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, 0
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture_id_, 0);

    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_CRITICAL(logging::opengl_logger, "Framebuffer is not ok");
        abort();
    }
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
ShadowMap::update() {
    glm::mat4 view_matrix = controls::get_view_matrix();
    glm::mat4 projection_matrix = controls::get_projection_matrix();
    glm::mat4 inverse_view_projection = glm::inverse(projection_matrix * view_matrix);

    std::vector<glm::vec4> shadow_range_corners;
    glm::vec4 center(0, 0, 0, 0);

    for (int x = -1; x < 2; x += 2) {
        for (int y = -1; y < 2; y += 2) {
            for (int z = -1; z < 2; z += 2) {
                glm::vec4 corner = {x, y, z, 1}; // w = 1

                glm::vec4 direction = inverse_view_projection * corner;
                direction = direction / direction.w;
                shadow_range_corners.emplace_back(direction);
                center = +direction;
            }
        }
    }

    center = center / center.w;
    glm::vec3 center3 = center;

    depth_view_matrix_ =
        glm::lookAt(light_direction_ + center3, center3, glm::vec3(0, 1, 0));

    for (size_t i = 0; i < shadow_range_corners.size(); i++) {
        shadow_range_corners[i] = depth_view_matrix_ * shadow_range_corners[i];
    }

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

} // namespace array_buffer

} // namespace gui
