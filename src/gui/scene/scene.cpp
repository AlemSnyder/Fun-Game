#include "scene.hpp"

#include "../../entity/mesh.hpp"
#include "../handler.hpp"

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#define SAMPLES 4

namespace gui {

Scene::Scene(
    uint32_t window_width, uint32_t window_height, uint32_t shadow_map_width_height
) :
    fbo(window_width, window_height, SAMPLES),
    shadow_map_(shadow_map_width_height, shadow_map_width_height), SR(), QRMS() {}

// add model attach functions.

void
Scene::update(GLFWwindow* window) {
    FrameBufferHandler::bind_fbo(shadow_map_.get_frame_buffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& shadow : SMR) {
        shadow->render_shadow_map(
            shadow_map_.get_shadow_width(), shadow_map_.get_shadow_height(),
            shadow_map_.get_frame_buffer()
        );
    }

    FrameBufferHandler::bind_fbo(fbo.get_frame_buffer_name());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SR.render(window, fbo.get_depth_buffer_name());

    for (const auto& render : FBR) {
        render->render_frame_buffer(window, fbo.get_depth_buffer_name());
    }

    for (const auto& render : FBMR) {
        render->render_frame_buffer_multisample(window, fbo.get_depth_buffer_name());
    }

    QRMS.render(
        fbo.get_width(), fbo.get_height(), fbo.get_num_samples(),
        fbo.get_texture_name(), fbo.get_frame_buffer_single()
    );
}

GLuint
Scene::get_scene() {
    return fbo.get_single_sample_texture();
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
Scene::shadow_attach(const std::shared_ptr<render_to::shadow_map>& shadow) {
    SMR.push_back(shadow);
}

void
Scene::frame_buffer_attach(const std::shared_ptr<render_to::frame_buffer>& render) {
    FBR.push_back(render);
}

void
Scene::frame_buffer_multisample_attach(
    const std::shared_ptr<render_to::frame_buffer_multisample>& render
) {
    FBMR.push_back(render);
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
