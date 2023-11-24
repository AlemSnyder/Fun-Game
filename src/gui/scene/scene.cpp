#include "scene.hpp"

#include "../../entity/mesh.hpp"
#include "../handler.hpp"

#include <GLFW/glfw3.h>

namespace gui {

// add model attach functions.

void
Scene::update(screen_size_t width, screen_size_t height) {
    FrameBufferHandler::getInstance().bind_fbo(shadow_map_.get_frame_buffer_id());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& shadow : mid_ground_shadow_) {
        shadow->render_shadow_map(
            shadow_map_.get_shadow_width(), shadow_map_.get_shadow_height(),
            shadow_map_.get_frame_buffer_id()
        );
    }

    FrameBufferHandler::getInstance().bind_fbo(
        frame_buffer_multisample_.get_frame_buffer_id()
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sky_renderer_.render(
        width, height, frame_buffer_multisample_.get_depth_buffer_name()
    );

    for (const auto& render : mid_ground_frame_buffer_) {
        render->render_frame_buffer(
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    for (const auto& render : mid_ground_frame_buffer_multisample_) {
        render->render_frame_buffer_multisample(
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    quad_renderer_multisample_.render(
        frame_buffer_multisample_.get_width(), frame_buffer_multisample_.get_height(),
        frame_buffer_multisample_.get_num_samples(),
        frame_buffer_multisample_.get_texture_name(),
        frame_buffer_multisample_.get_frame_buffer_single_id()
    );
}

} // namespace gui
