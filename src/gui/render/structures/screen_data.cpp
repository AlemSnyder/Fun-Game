#include "screen_data.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

ScreenData::ScreenData() :
    // clang-format off
    vertex_array_{
        glm::vec3(-1, -1, 0),
        glm::vec3(-1, 1, 0),
        glm::vec3(1, -1, 0),
        glm::vec3(1, 1, 0)
    },
    // clang-format on
    num_vertices_(4) {
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this]() {
        vertex_array_object_.bind();
        vertex_array_.attach_to_vertex_attribute(0);
        vertex_array_object_.release();
    });
}

} // namespace gpu_data

} // namespace gui
