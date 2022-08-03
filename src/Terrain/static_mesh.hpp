//! This is a test. The content tent of this class should be transferred into chunk

#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>

#include "../GUI/meshloader.hpp"

#ifndef __STATIC_MESH_HPP__
#define __STATIC_MESH_HPP__

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
    StaticMesh(const StaticMesh& obj){
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
    };
    StaticMesh& operator=(const StaticMesh& obj){
        vertex_buffer_ = obj.vertex_buffer_;
        color_buffer_ = obj.color_buffer_;
        normal_buffer_ = obj.normal_buffer_;
        element_buffer_ = obj.element_buffer_;
        transforms_buffer_ = obj.transforms_buffer_;
        num_vertices_ = obj.num_vertices_;
        num_models_ = obj.num_models_;
        return *this;
    }

    StaticMesh(){};

    StaticMesh(std::vector<unsigned short> &indices,
                std::vector<glm::vec3> &indexed_vertices,
                std::vector<glm::vec3> &indexed_colors,
                std::vector<glm::vec3> &indexed_normals,
                std::vector<glm::vec3> &model_transforms){
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

        /// Generate a buffer for the transforms
        glGenBuffers(1, &transforms_buffer_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, transforms_buffer_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        model_transforms.size() * sizeof(glm::vec3), &model_transforms[0],
                        GL_STATIC_DRAW);

        num_vertices_ = indices.size();
        num_models_ = model_transforms.size();
    }
    ~StaticMesh(){
        glDeleteBuffers(1, &vertex_buffer_);
        glDeleteBuffers(1, &color_buffer_);
        glDeleteBuffers(1, &normal_buffer_);
        glDeleteBuffers(1, &element_buffer_);
        glDeleteBuffers(1, &transforms_buffer_);
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

    GLuint get_model_transforms() const override {
        return transforms_buffer_;
    }

    unsigned int get_num_vertices() const override {
        return num_vertices_;
    }

    unsigned int get_num_models() const override {
        return num_models_;
    }
};

#endif
