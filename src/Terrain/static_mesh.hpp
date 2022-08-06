#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "../GUI/meshloader.hpp"

#ifndef __TERRAIN_TERRAIN_GENERATION_STATIC_MESH_HPP__
#define __TERRAIN_TERRAIN_GENERATION_STATIC_MESH_HPP__

class StaticMesh : public MeshLoader::MultiComplexMesh {
private:
    GLuint vertex_buffer_;
    GLuint color_buffer_;
    GLuint normal_buffer_;
    GLuint element_buffer_;
    GLuint transforms_buffer_;
    unsigned int num_vertices_;
    unsigned int num_models_;
public:
    inline StaticMesh(const StaticMesh& obj){
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
    };
    // copy operator
    inline StaticMesh& operator=(const StaticMesh& obj){
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
        return *this;
    }

    inline StaticMesh(){};

    StaticMesh(std::vector<unsigned short> &indices,
                std::vector<glm::vec3> &indexed_vertices,
                std::vector<glm::vec3> &indexed_colors,
                std::vector<glm::vec3> &indexed_normals,
                std::vector<glm::vec3> &model_transforms);
    inline ~StaticMesh(){
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
        glDeleteBuffers(1, &transforms_buffer_);
    }

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

    inline GLuint get_model_transforms() const override {
        return transforms_buffer_;
    }

    inline unsigned int get_num_vertices() const override {
        return num_vertices_;
    }

    inline unsigned int get_num_models() const override {
        return num_models_;
    }
};

#endif
