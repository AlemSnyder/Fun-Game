//! This is a test. The content tent of this class should be transferred into chunk

#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "../GUI/meshloader.hpp"

#pragma once

namespace terrain {

class TerrainMesh : public MeshLoader::SingleComplexMesh {
private:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    unsigned int num_vertices_;
public:
    inline TerrainMesh(const TerrainMesh& obj){
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
    };
    // copy operator
    inline TerrainMesh& operator=(const TerrainMesh& obj){
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
        return *this;
    }

    inline TerrainMesh(){};

    TerrainMesh(std::vector<unsigned short> &indices,
                std::vector<glm::vec3> &indexed_vertices,
                std::vector<glm::vec3> &indexed_colors,
                std::vector<glm::vec3> &indexed_normals);
    
    inline GLuint get_color_buffer() const override {
        return color_buffer_;
    }

    inline GLuint get_element_buffer() const override {
        return element_buffer_;
    }

    inline GLuint get_normal_buffer() const override {
        return normal_buffer_;
    }

    inline GLuint get_vertex_buffer() const override {
        return vertex_buffer_;
    }

    inline unsigned int get_num_vertices() const override {
        return num_vertices_;
    }
};

}
