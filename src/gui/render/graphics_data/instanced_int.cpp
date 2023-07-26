#include "instanced_int.hpp"

#include "../../../entity/mesh.hpp"
#include "../../../types.hpp"
#include "../../meshloader.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace gui {

namespace data_structures {

InstancedInt::InstancedInt(
    const entity::Mesh& mesh, const std::vector<glm::ivec3>& model_transforms
) {

    update(mesh);

    //generate_color_texture(mesh);

    /// Generate a buffer for the transforms
    glGenBuffers(1, &transforms_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transforms_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, model_transforms.size() * sizeof(glm::ivec3),
        model_transforms.data(), GL_STATIC_DRAW
    );
}

} // namespace data_structures

} // namespace gui
