#include "static_mesh.hpp"

#include "../gui/meshloader.hpp"
#include "../logging.hpp"
#include "mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

namespace terrain {

StaticMesh::StaticMesh(
    entity::Mesh mesh, const std::vector<glm::ivec3>& model_transforms
) :
    StaticMesh(
        mesh.indices_, mesh.indexed_vertices_, mesh.indexed_color_ids_,
        mesh.indexed_normals_, mesh.color_map_, model_transforms
    ) {}

StaticMesh::StaticMesh(
    const std::vector<unsigned short>& indices,
    const std::vector<glm::ivec3>& indexed_vertices,
    const std::vector<uint16_t>& indexed_color_ids,
    const std::vector<glm::i8vec3>& indexed_normals,
    const std::vector<uint32_t>& color_map,
    const std::vector<glm::ivec3>& model_transforms
) :
    num_vertices_(indices.size()),
    num_models_(model_transforms.size()) {
    assert(glGetError() == GL_NO_ERROR);

    // A buffer for the vertex positions
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::ivec3),
        indexed_vertices.data(), GL_STATIC_DRAW
    );
    assert(glGetError() == GL_NO_ERROR);

    // A buffer for the colors
    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_color_ids.size() * sizeof(uint16_t),
        indexed_color_ids.data(), GL_STATIC_DRAW
    );
    assert(glGetError() == GL_NO_ERROR);

    // Generate a buffer for the normal vectors
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::i8vec3),
        indexed_normals.data(), GL_STATIC_DRAW
    );
    assert(glGetError() == GL_NO_ERROR);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
        indices.data(), GL_STATIC_DRAW
    );
    assert(glGetError() == GL_NO_ERROR);

    // Generate a texture
    std::vector<std::array<float, 4>> float_colors;
    for( uint32_t int_color: color_map){
        uint32_t red = (int_color >> 24) & 0xFF;
        uint32_t green = (int_color >> 16) & 0xFF;
        uint32_t blue = (int_color >> 8) & 0xFF;
        uint32_t alpha = (int_color) & 0xFF;
        // the last one >> 0 is A
        std::array<float, 4> vector_color({
            red / 255.0f, green / 255.0f, blue / 255.0f, alpha / 255.0f
        });
        float_colors.push_back(vector_color);
    }

    assert(glGetError() == GL_NO_ERROR);
    LOG_DEBUG(logging::opengl_logger, "float_colors {}", float_colors);
    //LOG_DEBUG(logging::opengl_logger, "color texture id before {}", color_texture_);
    glGenTextures(1, &color_texture_);
    assert(glGetError() == GL_NO_ERROR);
    //LOG_DEBUG(logging::opengl_logger, "color texture id after binding {}", color_texture_);
    glBindTexture(GL_TEXTURE_1D, color_texture_);
    assert(glGetError() == GL_NO_ERROR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    assert(glGetError() == GL_NO_ERROR);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    assert(glGetError() == GL_NO_ERROR);
    // load and generate the texture
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, float_colors.size(), 0, GL_RGBA, GL_FLOAT, float_colors.data());
    assert(glGetError() == GL_NO_ERROR);
    glGenerateMipmap(GL_TEXTURE_1D);
    //assert(glGetError() == GL_NO_ERROR);

    /// Generate a buffer for the transforms
    glGenBuffers(1, &transforms_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transforms_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, model_transforms.size() * sizeof(glm::ivec3),
        model_transforms.data(), GL_STATIC_DRAW
    );
}

} // namespace terrain
