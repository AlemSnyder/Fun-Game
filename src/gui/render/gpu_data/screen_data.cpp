#include "screen_data.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

ScreenData::ScreenData() :
    // clang-format off
    vertex_array_(
        {glm::vec3(-1, -1, 0),
         glm::vec3(-1, 1, 0),
         glm::vec3(1, -1, 0),
         glm::vec3(1, 1, 0)}
    ),
    // clang-format on
    num_vertices_(4) {}

} // namespace gpu_data

} // namespace gui
