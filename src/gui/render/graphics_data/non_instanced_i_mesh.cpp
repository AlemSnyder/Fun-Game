#include "non_instanced_i_mesh.hpp"

#include "../../../terrain/material.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

namespace data_structures {

void
NonInstancedIMeshGPU::update(const entity::Mesh& mesh) {
    num_vertices_ = mesh.get_indices().size();
    do_render_ = (num_vertices_ != 0);

    vertex_array_.update(mesh.get_indexed_vertices(), 0);
    color_array_.update(mesh.get_indexed_color_ids(), 0);
    normal_array_.update(mesh.get_indexed_normals(), 0);
    element_array_.update(mesh.get_indices(), 0);
}

void
NonInstancedIMeshGPU::bind() const {
    vertex_array_.bind(0, 0);
    element_array_.bind(-1, -1);
    color_array_.bind(1, 1);
    normal_array_.bind(2, 2);
}

void
NonInstancedIMeshGPU::release() const {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

} // namespace data_structures

} // namespace gui
