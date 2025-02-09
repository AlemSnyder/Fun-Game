#include "instanced_i_mesh.hpp"

#include "types.hpp"
#include "world/entity/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

InstancedIMeshGPU::InstancedIMeshGPU(
    const world::entity::Mesh& mesh, const std::vector<glm::ivec4>& model_transforms
) :
    IMeshGPU(mesh, false),
    transforms_array_(model_transforms, 1), num_models_(model_transforms.size()) {
    // InstancedInt does not have a color texture. One must inherit from this
    // class and define a method that creates a color texture, and sets its id
    // as color_texture_.
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this]() { initialize(); });
}

void
InstancedIMeshGPU::update_transforms_array(std::vector<glm::ivec4> data, uint offset) {
    transforms_array_.update(data, offset);
}

void
InstancedIMeshGPU::attach_all() {
    IMeshGPU::attach_all();
    transforms_array_.attach_to_vertex_attribute(6);
}

} // namespace gpu_data

} // namespace gui
