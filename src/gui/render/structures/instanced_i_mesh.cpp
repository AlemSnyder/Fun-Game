#include "instanced_i_mesh.hpp"

#include "types.hpp"
#include "util/mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace gpu_data {

InstancedIMeshGPU::InstancedIMeshGPU(
    const util::Mesh& mesh, const std::vector<glm::ivec4>& model_transforms, bool differed
) : TransformsArray_vec4_c(model_transforms),
    IMeshGPU(mesh, differed),
    num_models_(model_transforms.size()) {}

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
