#include "instanced_i_mesh.hpp"

#include "entity/mesh.hpp"
#include "types.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace data_structures {

InstancedIMeshGPU::InstancedIMeshGPU(
    const entity::Mesh& mesh, const std::vector<glm::ivec4>& model_transforms
) :
    NonInstancedIMeshGPU(mesh),
    transforms_array_(model_transforms, 1, buffer_type::ARRAY_BUFFER),
    num_models_(model_transforms.size()) {
    // InstancedInt does not create a color texture. One must inherit from this
    // class and define a method that creates a color texture, and sets its id
    // as color_texture_.
}

} // namespace data_structures

} // namespace gui
