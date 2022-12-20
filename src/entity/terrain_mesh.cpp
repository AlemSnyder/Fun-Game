
#include "terrain_mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace terrain {

void
TerrainMesh::init(entity::Mesh mesh) {
    init(
        mesh.indices_, mesh.indexed_vertices_, mesh.indexed_colors_,
        mesh.indexed_normals_
    );
}

TerrainMesh::TerrainMesh(entity::Mesh mesh) :
    TerrainMesh(
        mesh.indices_, mesh.indexed_vertices_, mesh.indexed_colors_,
        mesh.indexed_normals_
    ) {}

TerrainMesh::TerrainMesh(
    const std::vector<unsigned short>& indices,
    const std::vector<glm::ivec3>& indexed_vertices,
    const std::vector<glm::vec3>& indexed_colors,
    const std::vector<glm::i8vec3>& indexed_normals
) {
    init(indices, indexed_vertices, indexed_colors, indexed_normals);
};

void
TerrainMesh::init(
    const std::vector<unsigned short>& indices,
    const std::vector<glm::ivec3>& indexed_vertices,
    const std::vector<glm::vec3>& indexed_colors,
    const std::vector<glm::i8vec3>& indexed_normals
) {
    // A buffer for the vertex positions
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::ivec3),
        &indexed_vertices[0], GL_STATIC_DRAW
    );

    // A buffer for the colors
    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_colors.size() * sizeof(glm::vec3), &indexed_colors[0],
        GL_STATIC_DRAW
    );

    // Generate a buffer for the normal vectors
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::i8vec3),
        &indexed_normals[0], GL_STATIC_DRAW
    );

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0],
        GL_STATIC_DRAW
    );

    num_vertices_ = indices.size();
}

} // namespace terrain
