#include "screen_data.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace data_structures {

ScreenData::ScreenData() :
    vertex_array_(ArrayBuffer(std::vector<glm::vec3>(
        {glm::vec3(-1, -1, 0), glm::vec3(-1, 1, 0), glm::vec3(1, 1, 0),
         glm::vec3(1, -1, 0)}
    ))),
    element_array_(ArrayBuffer(std::vector<unsigned short>({0, 1, 3, 2}))) {}

} // namespace data_structures

} // namespace gui
