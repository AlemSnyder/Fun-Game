#include "screen_data.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace array_buffer {

ScreenData::ScreenData() :
    // clang-format off
    vertex_array_(ArrayBuffer(
        {glm::vec3(-1, -1, 0),
         glm::vec3(-1, 1, 0),
         glm::vec3(1, -1, 0),
         glm::vec3(1, 1, 0)}
    )),
    // clang-format on
    num_vertices_(4) {}

} // namespace array_buffer

} // namespace gui
