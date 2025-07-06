#include "scene.hpp"

#include "../handler.hpp"
#include "logging.hpp"
#include "world/entity/mesh.hpp"
#include "world/entity/object_handler.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <chrono>
#include "util/time.hpp"

namespace gui {

// add model attach functions.

void
Scene::update(screen_size_t width, screen_size_t height) {
    GlobalContext& context = GlobalContext::instance();
    // wait for all tasks that may queue to opengl calls
    // context.wait_for_tasks();
    // not doing this they will just run in the background
    // run all opengl calls
    std::chrono::nanoseconds then = time_util::get_time_nanoseconds();
    context.run_opengl_queue();

    queue_time = time_util::get_time_nanoseconds() - then;

    then = time_util::get_time_nanoseconds();
    shadow_map_.update();
    environment_->update();
    render::PixelProjection::update(width, height);

    update_time = time_util::get_time_nanoseconds() - then;

    then = time_util::get_time_nanoseconds();

    FrameBufferHandler::instance().bind_fbo(shadow_map_.get_frame_buffer_id());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (const auto& shadow : mid_ground_shadow_) {
        shadow->render(
            shadow_map_.get_shadow_width(), shadow_map_.get_shadow_height(),
            shadow_map_.get_frame_buffer_id()
        );
    }

    shadow_render_time = time_util::get_time_nanoseconds() - then;

    then = time_util::get_time_nanoseconds();

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

    background_render_time = time_util::get_time_nanoseconds() - then;

    then = time_util::get_time_nanoseconds();

    glClear(GL_DEPTH_BUFFER_BIT);

    // mid ground
    for (const auto& render : mid_ground_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    mid_ground_render_time = time_util::get_time_nanoseconds() - then;

    then = time_util::get_time_nanoseconds();


    glClear(GL_DEPTH_BUFFER_BIT);
    // foreground
    for (const auto& render : foreground_frame_buffer_) {
        render->render(
            width, height, frame_buffer_multisample_.get_depth_buffer_name()
        );
    }

    foreground_render_time = time_util::get_time_nanoseconds() - then;

}

} // namespace gui
