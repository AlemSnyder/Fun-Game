
#include "terrain_mesh.hpp"

#include "../../logging.hpp"
#include "../../terrain/material.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

terrain::TerrainMesh::TerrainMesh(const entity::Mesh& mesh) {
    init(mesh);
}

void
terrain::TerrainMesh::init(const entity::Mesh& mesh) {
    color_texture_ = terrain::TerrainColorMapping::get_color_texture();
    // A buffer for the vertex positions
    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, mesh.get_indexed_vertices().size() * sizeof(glm::ivec3),
        mesh.get_indexed_vertices().data(), GL_STATIC_DRAW
    );

    // A buffer for the colors
    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, mesh.get_indexed_color_ids().size() * sizeof(uint16_t),
        mesh.get_indexed_color_ids().data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the normal vectors
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, mesh.get_indexed_normals().size() * sizeof(glm::i8vec3),
        mesh.get_indexed_normals().data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, mesh.get_indices().size() * sizeof(unsigned short),
        mesh.get_indices().data(), GL_STATIC_DRAW
    );

    num_vertices_ = mesh.get_indices().size();
}