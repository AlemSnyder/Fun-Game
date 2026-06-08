#include "floating_instanced_i_mesh.hpp"

#include "types.hpp"
#include "util/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

FloatingInstancedIMeshGPU::FloatingInstancedIMeshGPU(
    const util::Mesh& mesh, const std::vector<glm::mat4>& model_transforms, bool differed
) : TransformsArray_mat4_c(model_transforms), IMeshGPU(mesh, differed) {}

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
