#include "floating_instanced_i_mesh.hpp"

#include "types.hpp"
#include "world/entity/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

FloatingInstancedIMeshGPU::FloatingInstancedIMeshGPU(
    const world::entity::Mesh& mesh, const std::vector<glm::mat4>& model_transforms
) :
    IMeshGPU(mesh, false),
    transforms_array_(model_transforms, 1) {
    // InstancedInt does not have a color texture. One must inherit from this
    // class and define a method that creates a color texture, and sets its id
    // as color_texture_.
    GlobalContext& context = GlobalContext::instance();
    context.push_opengl_task([this]() { initialize(); });
}

void
FloatingInstancedIMeshGPU::update_transforms_array(
    std::vector<glm::mat4> data, uint offset
) {
    transforms_array_.update(data, offset);
}

void
FloatingInstancedIMeshGPU::attach_all() {
    IMeshGPU::attach_all();
    transforms_array_.attach_to_vertex_attribute(6);
}

} // namespace gpu_data

} // namespace gui
