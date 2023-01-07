
#include "terrain_mesh.hpp"
#include "../logging.hpp"

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
    const std::vector<uint16_t>& indexed_color_ids,
    const std::vector<glm::i8vec3>& indexed_normals,
    const std::vector<uint32_t>& color_map
) {
    init(indices, indexed_vertices, indexed_color_ids, indexed_normals, color_map);
};

void
TerrainMesh::init(
    const std::vector<unsigned short>& indices,
    const std::vector<glm::ivec3>& indexed_vertices,
    const std::vector<uint16_t>& indexed_color_ids,
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
        GL_ARRAY_BUFFER, indexed_color_ids.size() * sizeof(uint16_t),
        indexed_color_ids.data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the normal vectors
    glGenBuffers(1, &normal_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
    glBufferData(
        GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::i8vec3),
        indexed_normals.data(), GL_STATIC_DRAW
    );

    // Generate a buffer for the indices as well
    glGenBuffers(1, &element_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short),
        indices.data(), GL_STATIC_DRAW
    );

    num_vertices_ = indices.size();

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
    
    // Generate a texture
    glGenTextures(1, &color_texture_);
    glBindTexture(GL_TEXTURE_1D, color_texture_);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load and generate the texture
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, float_colors.size(), 0, GL_RGBA, GL_FLOAT, float_colors.data());
    glGenerateMipmap(GL_TEXTURE_1D);
}

} // namespace terrain
