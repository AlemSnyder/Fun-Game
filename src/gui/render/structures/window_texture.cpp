#include "window_texture.hpp"

#include "logging.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace render {

WindowTexture::WindowTexture() :
    // clang-format off
    gl_positions_{
        glm::vec3(-1, -1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, -1, 0),
        glm::vec3(1, 1, 0)
    },
    screen_positions_{
        glm::vec2(-1, -1),
        glm::vec2(-1, 1),
        glm::vec2(1, -1),
        glm::vec2(0, 0)},
    // clang-format on
    num_vertices_(4) {
    LOG_DEBUG(logging::main_logger, "Initializing a WindowTexture");
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this]() {
        vertex_array_object_.bind();
        gl_positions_.attach_to_vertex_attribute(0);
        screen_positions_.attach_to_vertex_attribute(1);
        vertex_array_object_.release();
    });
}

} // namespace render

} // namespace gui
