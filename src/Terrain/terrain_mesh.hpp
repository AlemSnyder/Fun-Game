//! This is a test. The content tent of this class should be transferred into chunk

#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "../GUI/meshloader.hpp"

#ifndef __TERRAIN_MESH_HPP__
#define __TERRAIN_MESH_HPP__

class TerrainMesh : public MeshLoader::SingleComplexMesh {
private:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    unsigned int num_vertices_;
public:
    TerrainMesh();

    TerrainMesh(std::vector<unsigned short> &indices,
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
    ~TerrainMesh(){
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
    }

    GLuint get_color_buffer() const override {
        return color_buffer_;
    }

    GLuint get_element_buffer() const override {
        return element_buffer_;
    }

    GLuint get_normal_buffer() const override {
        return normal_buffer_;
    }

    GLuint get_vertex_buffer() const override {
        return vertex_buffer_;
    }

    unsigned int get_num_vertices() const override {
        return num_vertices_;
    }
};

#endif
