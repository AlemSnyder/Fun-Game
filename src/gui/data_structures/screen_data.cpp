#include "screen_data.hpp"

#include <vector>

namespace gui {

ScreenData::ScreenData() {
    std::vector<glm::vec3> vertices;

    vertices.push_back(glm::vec3(-1, -1, 0));
    vertices.push_back(glm::vec3(-1, 1, 0));
    vertices.push_back(glm::vec3(1, 1, 0));
    vertices.push_back(glm::vec3(1, -1, 0));

    // A buffer for the vertex positions
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(),
        GL_STATIC_DRAW
    );

    // one vertex for each corner of the screen
    num_vertices_ = 4;

    std::vector<uint16_t> indices;

    indices.push_back(0U);
    indices.push_back(1U);
    indices.push_back(3U);
    indices.push_back(2U);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t),
        indices.data(), GL_STATIC_DRAW
    );
}

} // namespace gui
