#include "i_mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

namespace gpu_data {

void
IMeshGPU::update(const world::entity::Mesh& mesh) {
    num_vertices_ = mesh.get_indices().size();
    do_render_ = (num_vertices_ != 0);

    vertex_array_.update(mesh.get_indexed_vertices(), 0);
    color_array_.update(mesh.get_indexed_color_ids(), 0);
    normal_array_.update(mesh.get_indexed_normals(), 0);
    element_array_.update(mesh.get_indices(), 0);
}

void
IMeshGPU::bind() const {
    vertex_array_.bind(0, 0);
    element_array_.bind(-1, -1); // why not have a bind with no parameters that asserts
                                 // that the array is the elements array?
    color_array_.bind(1, 1);
    normal_array_.bind(2, 2);
}

void
IMeshGPU::release() const {
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

} // namespace gpu_data

} // namespace gui
