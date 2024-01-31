#include "screen_data.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace array_buffer {

ScreenData::ScreenData() :
    vertex_array_(ArrayBuffer(std::vector<glm::vec3>(
        {glm::vec3(-1, -1, 0), glm::vec3(-1, 1, 0), glm::vec3(1, -1, 0),
         glm::vec3(1, 1, 0)}
    ))),
    num_vertices_(4) {}

} // namespace array_buffer

} // namespace gui
