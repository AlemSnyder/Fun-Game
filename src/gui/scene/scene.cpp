#include "scene.hpp"

#include "../handler.hpp"
#include "logging.hpp"
#include "manifest/object_handler.hpp"
#include "util/mesh.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

// add model attach functions.

void
Scene::update(screen_size_t width, screen_size_t height) {
    GlobalContext& context = GlobalContext::instance();
    // wait for all tasks that may queue to opengl calls
    // context.wait_for_tasks();
    // not doing this they will just run in the background
    // run all opengl calls
    context.run_opengl_queue();

    shadow_map_.update();
    environment_->update();
    render::PixelProjection::update(width, height);

    FrameBufferHandler::instance().bind_fbo(shadow_map_.get_frame_buffer_id());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& shadow : mid_ground_shadow_) {
        shadow->render(
            shadow_map_.get_width(), shadow_map_.get_height(),
            shadow_map_.get_frame_buffer_id()
        );
    }

    FrameBufferHandler::instance().bind_fbo(
        frame_buffer_multisample_.get_frame_buffer_id()
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // background
    for (const auto& render : background_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer()->value()
        );
    }
    glClear(GL_DEPTH_BUFFER_BIT);

    // mid ground
    for (const auto& render : mid_ground_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer()->value()
        );
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    // foreground
    for (const auto& render : foreground_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer()->value()
        );
    }
}

} // namespace gui
