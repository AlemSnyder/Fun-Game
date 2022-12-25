
#include "terrain_mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace terrain {

void
TerrainMesh::init(entity::Mesh mesh) {
    init(
        mesh.indices_, mesh.indexed_vertices_, mesh.indexed_color_ids_,
        mesh.indexed_normals_, mesh.color_map_
    );
}

TerrainMesh::TerrainMesh(entity::Mesh mesh) :
    TerrainMesh(
        mesh.indices_, mesh.indexed_vertices_, mesh.indexed_color_ids_,
        mesh.indexed_normals_, mesh.color_map_
    ) {}

TerrainMesh::TerrainMesh(
    const std::vector<unsigned short>& indices,
    const std::vector<glm::ivec3>& indexed_vertices,
    const std::vector<uint16_t>& indexed_colors,
    const std::vector<glm::i8vec3>& indexed_normals,
    const std::vector<uint32_t>& color_map
) {
    init(indices, indexed_vertices, indexed_colors, indexed_normals, color_map);
};

void
TerrainMesh::init(
    const std::vector<unsigned short>& indices,
    const std::vector<glm::ivec3>& indexed_vertices,
    const std::vector<uint16_t>& indexed_colors,
    const std::vector<glm::i8vec3>& indexed_normals,
    const std::vector<uint32_t>& color_map
) {
    // A buffer for the vertex positions
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::ivec3),
        indexed_vertices.data(), GL_STATIC_DRAW
    );

    // A buffer for the colors
    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_colors.size() * sizeof(glm::vec3),
        indexed_colors.data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the normal vectors
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::i8vec3),
        indexed_normals.data(), GL_STATIC_DRAW
    );
    
    // Generate a texture
    glGenTextures(1, &color_texture_);
    glBindTexture(GL_TEXTURE_2D, color_texture_);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, color_map.size(), 1, 0, GL_RGB, GL_UNSIGNED_BYTE, color_map.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
        indices.data(), GL_STATIC_DRAW
    );

    num_vertices_ = indices.size();
}

} // namespace terrain
