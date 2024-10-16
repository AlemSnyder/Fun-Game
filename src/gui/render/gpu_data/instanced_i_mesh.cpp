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
    NonInstancedIMeshGPU(mesh),
    transforms_array_(model_transforms, 1) {
    // InstancedInt does not have a color texture. One must inherit from this
    // class and define a method that creates a color texture, and sets its id
    // as color_texture_.
}

void
InstancedIMeshGPU::update_transforms_array(std::vector<glm::ivec4> data, uint offset) {
    transforms_array_.update(data, offset);
}

} // namespace gpu_data

} // namespace gui
