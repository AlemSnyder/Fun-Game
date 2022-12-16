#include "../gui/meshloader.hpp"
#include "mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

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
    inline TerrainMesh(const TerrainMesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
    };

    // copy operator
    inline TerrainMesh& operator=(const TerrainMesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        num_vertices_ = obj.num_vertices_;
        return *this;
    }

    inline TerrainMesh(){};
    TerrainMesh(entity::Mesh mesh);
    void init(
        const std::vector<unsigned short>& indices,
        const std::vector<glm::ivec3>& indexed_vertices,
        const std::vector<glm::vec3>& indexed_colors,
        const std::vector<glm::ivec3>& indexed_normals
    );
    void init(entity::Mesh mesh);

    TerrainMesh(
        const std::vector<unsigned short>& indices,
        const std::vector<glm::ivec3>& indexed_vertices,
        const std::vector<glm::vec3>& indexed_colors,
        const std::vector<glm::ivec3>& indexed_normals
    );

    inline ~TerrainMesh() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
    }

    inline GLuint get_color_buffer() const override { return color_buffer_; }

    inline GLuint get_element_buffer() const override { return element_buffer_; }

    inline GLuint get_normal_buffer() const override { return normal_buffer_; }

    inline GLuint get_vertex_buffer() const override { return vertex_buffer_; }

    inline unsigned int get_num_vertices() const override { return num_vertices_; }
};

} // namespace terrain
