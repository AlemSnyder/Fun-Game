// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "terrain_mesh.hpp"

TerrainMesh::TerrainMesh(std::vector<unsigned short> &indices,
                std::vector<glm::vec3> &indexed_vertices,
                std::vector<glm::vec3> &indexed_colors,
                std::vector<glm::vec3> &indexed_normals){
        // A buffer for the vertex positions
        glGenBuffers(1, &vertex_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
        glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3),
                    &indexed_vertices[0], GL_STATIC_DRAW);

        // A buffer for the colors
        glGenBuffers(1, &color_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
        glBufferData(GL_ARRAY_BUFFER, indexed_colors.size() * sizeof(glm::vec3),
                    &indexed_colors[0], GL_STATIC_DRAW);

        // Generate a buffer for the normal vectors
        glGenBuffers(1, &normal_buffer_);
        glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_);
        glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3),
                    &indexed_normals[0], GL_STATIC_DRAW);

        // Generate a buffer for the indices as well
        glGenBuffers(1, &element_buffer_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    indices.size() * sizeof(unsigned short), &indices[0],
                    GL_STATIC_DRAW);

        num_vertices_ = indices.size();
    }