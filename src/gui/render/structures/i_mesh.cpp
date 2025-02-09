#include "i_mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace gui {

namespace gpu_data {

void
IMeshGPU::attach_all() {
    vertex_array_.attach_to_vertex_attribute(0);
    color_array_.attach_to_vertex_attribute(1);
    normal_array_.attach_to_vertex_attribute(2);
    element_array_.bind();
}

void
IMeshGPU::initialize() {
    vertex_array_object_.bind();
    attach_all();
    vertex_array_object_.release();
}

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
    vertex_array_object_.bind();
}

void
IMeshGPU::release() const {
    vertex_array_object_.release();
}

} // namespace gpu_data

} // namespace gui
