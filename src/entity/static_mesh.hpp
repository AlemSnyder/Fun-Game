#include "../gui/meshloader.hpp"
#include "mesh.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <vector>

#pragma once

namespace terrain {

class StaticMesh : public MeshLoader::MultiComplexMesh {
 private:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    GLuint color_texture_;
    GLuint transforms_buffer_;
    unsigned int num_vertices_;
    unsigned int num_models_;

 public:
    inline StaticMesh(const StaticMesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        color_texture_ = obj.color_texture_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
    };

    // copy operator
    inline StaticMesh& operator=(const StaticMesh& obj) {
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        color_texture_ = obj.color_texture_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
        return *this;
    }

    inline StaticMesh(){};

    StaticMesh(entity::Mesh mesh, const std::vector<glm::ivec3>& model_transforms);

    StaticMesh(
        const std::vector<unsigned short>& indices,
        const std::vector<glm::ivec3>& indexed_vertices,
        const std::vector<uint16_t>& indexed_colors,
        const std::vector<glm::i8vec3>& indexed_normals,
        const std::vector<uint32_t>& color_texture,
        const std::vector<glm::ivec3>& model_transforms
    );

    inline ~StaticMesh() {
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
        glDeleteTextures(1, &color_texture_);
        glDeleteBuffers(1, &transforms_buffer_);
    }

    inline GLuint get_color_buffer() const override { return color_buffer_; }

    inline GLuint get_element_buffer() const override { return element_buffer_; }

    inline GLuint get_normal_buffer() const override { return normal_buffer_; }

    inline GLuint get_vertex_buffer() const override { return vertex_buffer_; }

    inline GLuint get_color_texture() const override { return color_texture_; }

    inline GLuint get_model_transforms() const override { return transforms_buffer_; }

    inline unsigned int get_num_vertices() const override { return num_vertices_; }

    inline unsigned int get_num_models() const override { return num_models_; }
};

} // namespace terrain
