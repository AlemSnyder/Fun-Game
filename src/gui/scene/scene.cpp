#include "scene.hpp"

#include "../../entity/mesh.hpp"
#include "../handler.hpp"

#include <GLFW/glfw3.h>

#define SAMPLES 4

namespace gui {

Scene::Scene(
    screen_size_t window_width, screen_size_t window_height,
    uint32_t shadow_map_width_height
) :
    frame_buffer_multisample_(window_width, window_height, SAMPLES),
    shadow_map_(shadow_map_width_height, shadow_map_width_height),
    sky_renderer_(), quad_renderer_multisample_() {}

// add model attach functions.

void
Scene::update(GLFWwindow* window) {
    FrameBufferHandler::getInstance().bind_fbo(shadow_map_.get_frame_buffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& shadow : mid_ground_shadow_) {
        shadow->render_shadow_map(
            shadow_map_.get_shadow_width(),
            shadow_map_.get_shadow_height(),
            shadow_map_.get_frame_buffer()
        );
    }

    FrameBufferHandler::getInstance().bind_fbo(
        frame_buffer_multisample_.get_frame_buffer_name()
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sky_renderer_.render(window,
        frame_buffer_multisample_.get_depth_buffer_name()
    );

    for (const auto& render : mid_ground_frame_buffer_) {
        render->render_frame_buffer(
            window, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    for (const auto& render : mid_ground_frame_buffer_multisample_) {
        render->render_frame_buffer_multisample(
            window, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    quad_renderer_multisample_.render(
        frame_buffer_multisample_.get_width(),
        frame_buffer_multisample_.get_height(),
        frame_buffer_multisample_.get_num_samples(),
        frame_buffer_multisample_.get_texture_name(),
        frame_buffer_multisample_.get_frame_buffer_single()
    );
}

GLuint
Scene::get_scene() {
    return frame_buffer_multisample_.get_single_sample_texture();
}

GLuint
Scene::get_depth_texture() {
    return shadow_map_.get_depth_texture();
}

uint32_t
Scene::get_shadow_width() {
    return shadow_map_.get_shadow_width();
}

uint32_t
Scene::get_shadow_height() {
    return shadow_map_.get_shadow_height();
}

void
Scene::shadow_attach(const std::shared_ptr<render_to::ShadowMap>& shadow) {
    mid_ground_shadow_.push_back(shadow);
}

void
Scene::frame_buffer_attach(const std::shared_ptr<render_to::FrameBuffer>& render) {
    mid_ground_frame_buffer_.push_back(render);
}

void
Scene::frame_buffer_multisample_attach(
    const std::shared_ptr<render_to::FrameBufferMultisample>& render
) {
    mid_ground_frame_buffer_multisample_.push_back(render);
}

void
Scene::set_shadow_light_direction(glm::vec3 light_direction) {
    shadow_map_.set_light_direction(light_direction);
}

void
Scene::set_shadow_depth_projection_matrix(glm::mat4 depth_projection_matrix) {
    shadow_map_.set_depth_projection_matrix(depth_projection_matrix);
}

} // namespace gui
