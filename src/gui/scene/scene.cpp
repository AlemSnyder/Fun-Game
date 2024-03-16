#include "scene.hpp"
#include "logging.hpp"

#include "../handler.hpp"
#include "world/entity/mesh.hpp"
#include "world/entity/object_handler.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

// add model attach functions.

void
Scene::update(screen_size_t width, screen_size_t height) {
    world::entity::ObjectHandler& object_handler =
        world::entity::ObjectHandler::instance();

    // send all data to gpu
    object_handler.update();

    GlobalContext& context = GlobalContext::instance();
    // wait for all tasks that may queue to opengl calls
    context.wait_for_tasks();
    // run all opengl calls
    context.run_opengl_queue();

    shadow_map_.update();
    environment_->update();
    render::PixelProjection::update(width, height);

    FrameBufferHandler::instance().bind_fbo(shadow_map_.get_frame_buffer_id());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& shadow : mid_ground_shadow_) {
        shadow->render(
            shadow_map_.get_shadow_width(), shadow_map_.get_shadow_height(),
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
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }
    glClear(GL_DEPTH_BUFFER_BIT);

    // mid ground
    for (const auto& render : mid_ground_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    // foreground
    for (const auto& render : foreground_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }
}

} // namespace gui
